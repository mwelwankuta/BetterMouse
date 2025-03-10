cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MouseGestures VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(mouse_gestures
    "mouse gestures.cpp"
    main.cpp
    "mouse files.c"
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Windows-specific configurations
if(WIN32)
    # Add Windows-specific libraries if needed
    target_link_libraries(mouse_gestures user32 gdi32)
endif()

# Install rules
install(TARGETS mouse_gestures
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 