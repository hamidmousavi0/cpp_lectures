// g++ main.cpp -o main --> the output is a binary. 
// How it works:
/*
1- preprocessing --> expnad #include and #define and macros --> produce expanded source (.i)
2- compilation ---> translate C++ into assembly (.s) 
3- Assembly --> convert assembly to machine code (.o) --> object file --> compiled machine code for one 
				translation unit (.cpp file).
				when we have bunch of .o files that other pepole and project might want to use --->
				we can bundle them into library. 
				1- static library (.a on linux, .lib windows)---> a archive of .o files. 
				no external dependency at runtime and he linker copies the needed machine code directly into your executable
				2- Shared library (.so on linux, .dll on windows) ---> The .so file must be present at run time. 
				# Build a shared lib
				g++ -fPIC -shared foo.cpp bar.cpp -o libfoo.so
				# Link your program against it
				g++ main.cpp -L. -lfoo -o main
				# -L. = "look in current dir for libraries"
				# -lfoo = "link against libfoo.so" (lib prefix and extension are implied) 
4- Linking ---> combines object files (+libraries) into final executable
*/
// ./main


// Makefiles and make

// make ---> build system program help us to compile. 
// we do not need to compile every single file in the project. 
// we can specify what compiler we want to use. 
// to use make we need to have Makefile. 
// Make tracks which file is changed since last compile. 


// Makefile
/*
# Compiler
CXX = g++

# Compiler flags 
CXXFLAGS = -std=c++20

#Source files and target
SRCS = $(wildcard *.cpp) 	
TARGET = main

#Default target
all: 
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Clean up 
clean:
	rm -rf $(TARGET)
*/

// CMake ---> build system generator. 
// Use to generate Makefile
// Higher level abstraction of makefile. 


// CMakeLists.txt
/*
cmake_minimum_required(VERSION 3.10)
project(cs106l_classes)
set(CMAKE_CXX_STANDARD 20)
file(GLOB SRC_FILES "*.cpp") // wildcard search for all files that have .cpp
add_executable(main ${SRC_FILES})
find_package(OpenSSL REQUIRED) ---> find_package searches standard system paths for an already-built library + its headers
target_link_libraries(main PRIVATE OpenSSL::SSL) --> writes into the target

*/


// how to use
/*
1- make CMakeLists.txt
2- make build folder whithin project
3- go into build folder
4- cmake .. ---> this run cmake on CMakeLists.txt and generate Makefile
5- run make
6- execute your program. 


*/