# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = 

# Detect operating system
ifeq ($(OS),Windows_NT)
    LDFLAGS += -luser32 -lgdi32
    TARGET = mouse_gestures.exe
    RM = del /Q
    MKDIR = mkdir
else
    TARGET = mouse_gestures
    RM = rm -f
    MKDIR = mkdir -p
endif

# Directories
SRCDIR = .
OBJDIR = obj
BINDIR = bin

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%=$(OBJDIR)/%.o)

# Main target
all: directories $(BINDIR)/$(TARGET)

# Create necessary directories
directories:
	@$(MKDIR) $(OBJDIR) $(BINDIR)

# Link the executable
$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile C++ source files
$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C source files
$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(BINDIR)/$(TARGET)

# Clean and rebuild
rebuild: clean all

.PHONY: all clean rebuild directories 