# Mouse Gestures

A C++ application for detecting and handling mouse gestures.

## Description

This project implements mouse gesture recognition functionality, allowing users to perform actions by drawing patterns with their mouse movements.

## Mouse Commands (Supports both mice with side buttons )

### Basic Commands

The Action button by default is mouse button 5, though can be changed to mouse button 2
1. **Action + Mouse Wheel Up / Mouse Wheel Down**
   - Action: Switch between windows ( (Hold Alt) +Tab, While Holding Alt Right)
   - Description: Hold the right mouse button and scroll the wheel up to switch between open applications
   - Note: If i was previously moving left and right is clicked instead of running the entire action + mouse wheel up action, just replace the right with left in the command so it feels like Ctrl + Alt, Left / Or

1. **Action + Mouse Wheel Up / Mouse Wheel Down**
   - Action: Switch between windows ( (Hold Alt) + Tab, While Holding Alt Left)
   - Description: Hold the right mouse button and scroll the wheel up to switch between open applications

2. **Action + Move Mouse Downwards**
   - Action: Windows + Tab if no window focused otherwise Show Desktop (Windows+D)
   - Description: Hold the right mouse button and scroll the wheel down to minimize all windows and show the desktop

3. **Action + Move Mouse Upwards**
   - Action: Switch between windows (Windows+Tab)
   - Description: Hold left and right mouse buttons simultaneously to switch between open applications

3. **Action + Move Mouse Leftwards**
   - Action: Switch between windows (Windows+Ctrl+Left)
   - Description: Hold action button and drag mouse towards the left to switch between virtual desktops

3. **Action + Move Mouse Rightwards**
   - Action: Switch between windows (Windows+Ctrl+Right)
   - Description: Hold action button and drag mouse towards the right to switch between virtual desktops

### Notes
- The mouse wheel sensitivity is automatically adjusted when the application starts
- The original mouse wheel sensitivity is restored when the application closes
- All commands work system-wide once the application is running

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