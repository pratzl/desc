#!/bin/bash
# Format all source files using clang-format

# Find all C++ source and header files
find include tests examples benchmark -type f \( -name "*.hpp" -o -name "*.cpp" \) -exec clang-format -i {} +

echo "Formatting complete."
