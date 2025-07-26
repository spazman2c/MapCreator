#!/bin/bash

# Cryptic Dungeon Map Editor Build Script
# This script automates the build process for macOS

set -e

echo "🚀 Building Cryptic Dungeon Map Editor..."

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "❌ This script is designed for macOS only"
    exit 1
fi

# Check for Xcode Command Line Tools
if ! command -v clang &> /dev/null; then
    echo "📦 Installing Xcode Command Line Tools..."
    xcode-select --install
    echo "Please complete the Xcode installation and run this script again"
    exit 1
fi

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "📦 Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Install dependencies
echo "📦 Installing dependencies..."
brew install cmake glfw glm assimp

# Create build directory
echo "🔨 Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "⚙️ Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "🔨 Building project..."
make -j$(nproc)

echo "✅ Build complete!"
echo "🎮 Run the editor with: ./MapEditor" 