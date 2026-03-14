#!/usr/bin/env python3
"""
CanFly User-Mode Application Packaging Tool

Creates packaged binary images from ELF files for CanFly user-mode applications.
Adds a 32-byte header and SHA256 hash for integrity verification.

Usage:
    python3 package_usermode.py -e <elf_file> -o <output_dir> [-n <image_name>]

Example:
    python3 package_usermode.py -e firmware/att_au_nz.elf -o images
"""

import argparse
import hashlib
import os
import struct
import sys
from pathlib import Path
from typing import Tuple

try:
    from elftools.elf.elffile import ELFFile
except ImportError:
    print("ERROR: pyelftools library not found. Install with: pip install pyelftools", file=sys.stderr)
    sys.exit(1)


def extract_binary_from_elf(elf_path: str) -> Tuple[bytearray, int]:
    """
    Extract loadable segments from ELF file.

    Returns:
        Tuple of (binary_data, entry_point)
    """
    try:
        with open(elf_path, 'rb') as f:
            elf = ELFFile(f)

            # Get entry point
            entry_point = elf.header['e_entry']

            # Extract all LOAD segments
            segments = []
            min_addr = None
            max_addr = 0

            for segment in elf.iter_segments():
                if segment['p_type'] == 'PT_LOAD':
                    vaddr = segment['p_vaddr']
                    size = segment['p_filesz']
                    data = segment.data()

                    if min_addr is None or vaddr < min_addr:
                        min_addr = vaddr

                    if vaddr + size > max_addr:
                        max_addr = vaddr + size

                    segments.append((vaddr, data))

            if not segments:
                raise ValueError("No loadable segments found in ELF file")

            # Create binary with segments at correct offsets
            total_size = max_addr - min_addr
            binary_data = bytearray(total_size)

            for vaddr, data in segments:
                offset = vaddr - min_addr
                binary_data[offset:offset + len(data)] = data

            # Remove trailing zeros
            while len(binary_data) > 0 and binary_data[-1] == 0:
                binary_data.pop()

            # Align to 32 bytes
            while len(binary_data) % 32 != 0:
                binary_data.append(0x00)

            return binary_data, entry_point

    except Exception as e:
        raise RuntimeError(f"Failed to extract binary from ELF {elf_path}: {e}")


def create_header(image_size: int, image_name: str) -> bytes:
    """
    Create the 32-byte user-mode image header.

    Format (matches pic32_image_header_t in pic32_tcb.h):
        0-3:   Reserved1 (0x10000008) - branch forward 32 bytes
        4-7:   Reserved2 (0x00000c00) - EHB (execution hazard barrier)
        8-11:  Image length (big-endian, includes this 32-byte header)
        12-15: Reserved
        16-31: Image name (16 bytes, null-terminated ASCII string)
    """
    if len(image_name) > 16:
        raise ValueError("Image name must be 16 characters or less")

    header = bytearray(32)

    # Reserved1: Branch forward 32 bytes (0x10000008)
    # MIPS: b +8 instructions (8 * 4 = 32 bytes)
    struct.pack_into('<I', header, 0, 0x10000008)

    # Reserved2: EHB (execution hazard barrier) (0x00000c00)
    struct.pack_into('<I', header, 4, 0x00000c00)

    # Image length (big-endian) - includes the 32-byte header
    struct.pack_into('>I', header, 8, image_size + 32)

    # Reserved (12-15): zeros

    # Image name (16 bytes, ASCII, null-terminated)
    name_bytes = image_name.encode('ascii')
    header[16:16+len(name_bytes)] = name_bytes
    # Remaining bytes are already 0x00 (null padding)

    return bytes(header)


def create_packaged_image(header: bytes, image_data: bytes) -> bytes:
    """
    Create the complete packaged image with SHA256 hash.

    Format:
        - 32-byte header
        - Variable-length image (32-byte aligned)
        - 28-byte signature type string ("sig-sha256")
        - 4-byte signature length (big-endian)
        - 32-byte SHA256 hash
        - Padding to 32-byte alignment
    """
    output = bytearray()

    # Header
    output.extend(header)

    # Image data
    output.extend(image_data)

    # Calculate SHA256 of header + image
    sha256_hash = hashlib.sha256(header + image_data).digest()

    # Signature type (28 bytes)
    sig_type = b"sig-sha256"
    output.extend(sig_type)
    output.extend(b'\x00' * (28 - len(sig_type)))

    # Signature length (big-endian)
    output.extend(struct.pack('>I', len(sha256_hash)))

    # SHA256 hash
    output.extend(sha256_hash)

    # Pad to 32-byte alignment
    while len(output) % 32 != 0:
        output.append(0x00)

    return bytes(output)


def main():
    parser = argparse.ArgumentParser(
        description='Package CanFly user-mode application images',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Example:
  %(prog)s -e firmware/att_au_nz.elf -o images -n att_au_nz
        """
    )

    parser.add_argument('-e', '--elf', required=True,
                        help='ELF file to package')
    parser.add_argument('-o', '--output', required=True,
                        help='Output directory for packaged image')
    parser.add_argument('-n', '--name', required=False,
                        help='Image name (default: derived from ELF filename)')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Verbose output')

    args = parser.parse_args()

    # Validate inputs
    if not os.path.exists(args.elf):
        parser.error(f"ELF file not found: {args.elf}")

    # Determine image name
    if args.name:
        image_name = args.name
    else:
        # Use ELF filename without extension
        image_name = Path(args.elf).stem

    # Limit to 16 characters
    if len(image_name) > 16:
        print(f"WARNING: Image name '{image_name}' truncated to 16 characters", file=sys.stderr)
        image_name = image_name[:16]

    # Create output directory
    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Output filename
    bin_path = output_dir / f"{Path(args.elf).stem}.bin"

    if args.verbose:
        print(f"CanFly User-Mode Application Packager")
        print(f"======================================")
        print(f"ELF file:   {args.elf}")
        print(f"Image name: {image_name}")
        print(f"Output:     {bin_path}")
        print()

    try:
        # Extract binary from ELF
        if args.verbose:
            print(f"Extracting binary from ELF...")

        image_data, entry_point = extract_binary_from_elf(args.elf)

        if args.verbose:
            print(f"  Image size: {len(image_data)} bytes")
            print(f"  Entry point: 0x{entry_point:08X}")

        # Create header
        if args.verbose:
            print("Creating image header...")

        header = create_header(len(image_data), image_name)

        # Create packaged image with SHA256
        if args.verbose:
            print("Creating packaged image with SHA256 hash...")

        packaged_image = create_packaged_image(header, image_data)

        # Write output
        with open(bin_path, 'wb') as f:
            f.write(packaged_image)

        if args.verbose:
            print(f"  Total size: {len(packaged_image)} bytes")
            print()
            print(f"✓ Image created successfully: {bin_path}")
        else:
            print(f"Created: {bin_path} ({len(packaged_image)} bytes)")

        return 0

    except Exception as e:
        print(f"\n✗ ERROR: {e}", file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
