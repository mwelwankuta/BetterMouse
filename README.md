# Mouse Gestures

A C++ application for detecting and handling mouse gestures.

## Description

This project implements mouse gesture recognition functionality, allowing users to perform actions by drawing patterns with their mouse movements.

## Prerequisites

- C++ compiler (supporting C++11 or later)
- CMake (version 3.10 or later)
- GNU Make (for Makefile build)
- Code::Blocks IDE (optional)

## Building the Project

### Using CMake (Recommended)
```bash
# Create a build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the project
cmake --build .

# The executable will be in build/bin/mouse_gestures
```

### Using Make
```bash
# Build the project
make

# Clean build files
make clean

# Rebuild everything
make rebuild

# The executable will be in bin/mouse_gestures
```

### Using Code::Blocks
1. Open the project file in Code::Blocks
2. Build and run the project using the IDE

### Manual Build
```bash
g++ -o mouse_gestures mouse\ gestures.cpp main.cpp -std=c++11
```

## Project Structure

- `mouse gestures.cpp` - Main gesture recognition implementation
- `main.cpp` - Program entry point
- `mouse files.c` - Additional mouse handling functionality
- `CMakeLists.txt` - CMake build configuration
- `Makefile` - Make build configuration
- `.gitignore` - Git ignore rules
- `LICENSE` - MIT License file

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details. 