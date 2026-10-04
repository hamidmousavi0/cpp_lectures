/*
=====================================================================================
Lecture 1: Welcome to C++
=====================================================================================

1) Why C++? "The invisible foundation of everything."
   Operating systems, browsers, game engines, databases and compilers are written in C++.
   C++ is great for handling lots of data very efficiently, while staying readable.
   The language is standardized; the latest version is C++26.

2) Where does C++ come from?
   1. Assembly
      + simple instructions, extremely fast, complete control over the machine
      - a lot of code, hard to understand, not portable (tied to one CPU)
   2. C
      + fast, simple, cross-platform: a compiler translates the source code to assembly
        for each machine
      - no objects or classes, hard to write generic (template) code,
        tedious to write large programs
   3. C++
      + express ideas and intent directly in the code
      + enforce safety at compile time
      + do not waste time or space
      "I like my code to be elegant and efficient. I hate to have to choose between them."
      (Bjarne Stroustrup, creator of C++)

3) C++ design philosophy
   1) Readability  2) Safety  3) Efficiency  4) Abstraction  5) Programmer choice

4) Questions C++ helps us answer while coding
   1. Am I using objects the way they are meant to be used? ---> type checking, type safety
   2. Am I using memory efficiently?                         ---> reference/copy semantics,
                                                                 move semantics
   3. Am I modifying something I am not supposed to?         ---> const and const correctness
   Many other languages relax these restrictions; C++ lets the compiler check them.

"Nobody should call themselves a professional if they only know one language."
(Bjarne Stroustrup)
*/

#include <iostream>  // <...> = search only the system (compiler) include folders
#include <memory>    // std::make_unique, std::unique_ptr
#include <string>    // std::string

// ---------------------------------------------------------------------------------
// Hello World, C++ style: a std::string on the heap, owned by a smart pointer.
// (Named main_1 so it does not clash with main below; it only runs if main calls it.)
// ---------------------------------------------------------------------------------
int main_1() {
    // std::make_unique<std::string>("Hello World") does three things:
    //   1. Allocate : reserve memory on the heap for one std::string
    //   2. Construct: build the string there, passing "Hello World" to its constructor
    //   3. Return   : a std::unique_ptr<std::string> that owns the heap object
    //
    //   Stack (main_1)                      Heap
    //   str : unique_ptr<string>  ------->  std::string "Hello World"
    //
    // When str goes out of scope at the closing }, the string is deleted automatically.
    auto str = std::make_unique<std::string>("Hello World");
    std::cout << *str << std::endl;  // *str  ---> the string itself (dereference)
    std::cout << str->size();        // str-> ---> call a member function through the pointer
}

// ---------------------------------------------------------------------------------
// Hello World, C style.
// ---------------------------------------------------------------------------------
#include "stdio.h"   // C standard input/output: printf, scanf, fopen. C++ version: <cstdio>
                     // "..." = search the current folder first, then the system folders
#include "stdlib.h"  // C general utilities: malloc/free, exit, atoi, EXIT_SUCCESS.
                     // C++ version: <cstdlib>

// argc ("argument count")  ---> number of command-line arguments, including the program name
// argv ("argument vector") ---> array of C strings (char*): argv[0] is the program name,
//                               argv[1], argv[2], ... are the arguments the user typed
// e.g. ./welcome hi 42  --->  argc = 3, argv = {"./welcome", "hi", "42"}
int main(int argc, char** argv) {
    printf("%s", "Hello World");  // %s is replaced by the next argument (a C string)
    return EXIT_SUCCESS;          // macro (usually 0): tells the OS the program succeeded
}