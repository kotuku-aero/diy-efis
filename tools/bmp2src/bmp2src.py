#!/usr/bin/env python3
"""Convert an image file to a canfly bitmap C source file.

Usage:
  bmp2src.py <input> [output] [--width W] [--size WxH] [--name NAME] [--include PATH]

The color format matches the canfly RGBA macro: (R<<24)|(G<<16)|(B<<8)|A.
"""

import argparse
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow is required. Install with: pip install Pillow", file=sys.stderr)
    sys.exit(1)


def parse_size(s):
    parts = s.lower().split('x')
    if len(parts) != 2:
        raise argparse.ArgumentTypeError(f"Size must be WxH (e.g. 32x24), got '{s}'")
    try:
        return (int(parts[0]), int(parts[1]))
    except ValueError:
        raise argparse.ArgumentTypeError(f"Size dimensions must be integers, got '{s}'")


def convert(input_path, output_path=None, target_size=None, var_name=None, include_path=None):
    input_path = Path(input_path).resolve()

    img = Image.open(input_path).convert('RGBA')
    orig_w, orig_h = img.size

    if target_size:
        img = img.resize(target_size, Image.LANCZOS)

    width, height = img.size
    raw = img.tobytes()  # RGBA bytes, 4 per pixel

    if var_name is None:
        var_name = input_path.stem.replace('-', '_').replace(' ', '_')

    if output_path is None:
        output_path = input_path.with_suffix('.c')
    output_path = Path(output_path)

    if include_path is None:
        include_path = "../../include/sys_canfly.h"

    with open(output_path, 'w', newline='\n') as f:
        f.write(f'// This file was automatically generated from\n')
        f.write(f'// {input_path}\n')
        if target_size:
            f.write(f'// Scaled from {orig_w}x{orig_h} to {width}x{height}\n')
        f.write(f'//\n')
        f.write(f'#include "{include_path}"\n\n')
        f.write(f'static const color_t bitmap_bits[] = {{\n')

        num_pixels = width * height
        for i in range(num_pixels):
            r, g, b, a = raw[i*4], raw[i*4+1], raw[i*4+2], raw[i*4+3]
            if i % 4 == 0:
                f.write('\t')
            color = (r << 24) | (g << 16) | (b << 8) | a
            f.write(f'0x{color:08x}, ')
            if (i + 1) % 4 == 0:
                f.write('\n')

        if num_pixels % 4 != 0:
            f.write('\n')

        f.write('\t};\n')
        f.write(f'const bitmap_t  {var_name}_bitmap = {{ {width}, {height}, bitmap_bits }};\n')

    print(f"Written {width}x{height} bitmap ({width * height} pixels) to {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description='Convert image to canfly bitmap C source file.',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument('input', help='Input image file (BMP, PNG, JPG, etc.)')
    parser.add_argument('output', nargs='?', help='Output .c file (default: input path with .c extension)')
    size_group = parser.add_mutually_exclusive_group()
    size_group.add_argument('--width', type=int, metavar='W',
                            help='Scale to this width, preserving aspect ratio')
    size_group.add_argument('--size', type=parse_size, metavar='WxH',
                            help='Scale to exact size (e.g. 32x24)')
    parser.add_argument('--name', metavar='NAME',
                        help='C variable name (default: input filename stem)')
    parser.add_argument('--include', default='../../include/sys_canfly.h', metavar='PATH',
                        help='#include path in generated file (default: ../../include/sys_canfly.h)')

    args = parser.parse_args()

    target_size = None
    if args.size:
        target_size = args.size
    elif args.width:
        img = Image.open(args.input)
        w, h = img.size
        target_size = (args.width, round(h * args.width / w))

    convert(args.input, args.output, target_size, args.name, args.include)


if __name__ == '__main__':
    main()
