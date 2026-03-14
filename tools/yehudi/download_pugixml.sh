#!/bin/bash
# download_pugixml.sh - Downloads pugixml library for Yehudi

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PUGIXML_VERSION="1.14"
PUGIXML_URL="https://raw.githubusercontent.com/zeux/pugixml/v${PUGIXML_VERSION}/src"

echo "Downloading pugixml v${PUGIXML_VERSION}..."
echo

cd "$SCRIPT_DIR"

# Download header
echo "Downloading pugixml.hpp..."
curl -L -o pugixml.hpp "${PUGIXML_URL}/pugixml.hpp"

# Download implementation
echo "Downloading pugixml.cpp..."
curl -L -o pugixml.cpp "${PUGIXML_URL}/pugixml.cpp"

# Download config header
echo "Downloading pugiconfig.hpp..."
curl -L -o pugiconfig.hpp "${PUGIXML_URL}/pugiconfig.hpp"

echo
echo "✓ pugixml downloaded successfully!"
echo "  Files:"
echo "    - pugixml.hpp"
echo "    - pugixml.cpp"
echo "    - pugiconfig.hpp"
echo
echo "You can now build Yehudi:"
echo "  cd ../../build"
echo "  cmake .."
echo "  make yehudi"
