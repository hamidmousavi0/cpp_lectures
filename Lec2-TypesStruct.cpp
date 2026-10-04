/*
=====================================================================================
Lecture 2: Types and Structs
=====================================================================================

1) Data types in C++
   - Fundamental (built-in) types ---> int, float, double, char, bool, void
   - Compound (derived) types      ---> arrays, pointers, references, functions
   - User-defined types            ---> class, struct, union, enum
   - Type aliases (a new NAME for an existing type, not a new type) ---> typedef, using

2) Compiled vs interpreted languages
   - Interpreted (e.g. Python): the interpreter reads the source and executes it
     while the program runs, statement by statement. No separate build step.
   - Compiled (e.g. C++): the source is translated to machine code BEFORE it runs,
     producing an executable file.

   What does "g++ main.cpp utils.cpp -o main" actually do? Four steps:

     main.cpp  --preprocess--> main.i  --compile--> main.s  --assemble--> main.o  --+
     utils.cpp --preprocess--> utils.i --compile--> utils.s --assemble--> utils.o --+--link--> main
                                                          libraries (.a / .so) ----+

   - preprocess : expand #include and #define (plain text copy-paste)
   - compile    : translate C++ into assembly (.s)
   - assemble   : translate assembly into machine code ---> object file (.o)
                  An object file holds the machine code of ONE .cpp file. Calls to functions
                  in other files are left as unresolved placeholders.
   - link       : combine all .o files + libraries into one executable and fill in the
                  placeholders.
   Libraries: static (.a / .lib) are copied into the executable at link time;
              shared (.so / .dll) are loaded when the program starts.
   (g++ does all four steps at once and deletes the intermediate files; use -c to stop at .o)

3) When are errors found?
   - Python: type errors appear at RUN time, only when that line is executed.
   - C++:    type errors appear at COMPILE time, before the program ever runs.
             (C++ can still have run-time errors, e.g. dividing by zero or a bad index.)

4) Static vs dynamic typing
   A type is the category of a value: what it can hold and which operations are allowed.
   - C++ is STATICALLY typed: every variable has a type that is known at compile time
     and can never change. The compiler checks all types before generating machine code.
   - Python is DYNAMICALLY typed: the type belongs to the value, not the variable, and
     is checked at run time. The same variable can hold an int now and a string later.
   Why static typing? 1) more efficient code  2) easier to read  3) better error checking.

   int vs size_t: int is a signed integer (can be negative); size_t is an unsigned
   integer (never negative), used for sizes and indices, e.g. vec.size().

   ---> C++ is a compiled, statically typed language.

5) Aside: function overloading
   Two or more functions with the same name but different parameter lists. The compiler
   picks the right one from the argument types:
       int    max(int a, int b);
       double max(double a, double b);

6) Structs
   Q: How can a function return more than one value?
   A: A struct bundles several named variables (members) into one new type, so a
      function can return them together.

7) std and namespaces
   - std is the namespace of the C++ Standard Library: library types (std::string,
     std::vector, std::pair), functions (std::sort, std::sqrt) and objects (std::cout).
     Built-in types like int and double are part of the language, NOT in std.
   - A namespace is a named scope that groups names so they do not clash: your own max()
     and std::max() can both exist because they live in different namespaces.
   - :: is the scope resolution operator. A::B means "the B that belongs to A", where A
     is a namespace (std::cout) or a class (Photo::area).

8) Why do we need #include?
   The compiler reads each .cpp file on its own, top to bottom, and every name must be
   DECLARED before it is used. #include (a preprocessor instruction) copy-pastes a header
   file into your code so the compiler sees those declarations:
       #include <iostream> ---> std::cout, std::cin
       #include <string>   ---> std::string
       #include <utility>  ---> std::pair
       #include <cmath>    ---> std::sqrt
   The header only DECLARES std::cout (its name and type). The actual object/code lives in
   the compiled standard library (libstdc++), which the LINKER adds automatically.
   A header declares a focused part of the library. Match each common type or object to the header
that declares it.
*/
#include <cmath>
#include <iostream>
#include <string>
#include <utility>

// A struct bundles related variables (members) into one new type.
struct StanfordID {
    std::string name;
    std::string sunet;
    int idNumber;
};

// Type aliases with "using": short, readable names for long types (no new type is created).
using Zeros = std::pair<double, double>;  // the two roots {x1, x2}
using solution = std::pair<bool, Zeros>;  // {found real roots?, roots}

// Solves a*x^2 + b*x + c = 0.
// Returns {true, {x1, x2}} if there are real roots, {false, {0, 0}} otherwise.
solution solveQuadratic(double a, double b, double c) {
    if (a == 0) {  // not a quadratic: dividing by 2*a would give inf/nan
        return {false, {0.0, 0.0}};
    }
    double discriminant = (b * b) - (4 * a * c);  // b^2 - 4ac decides how many real roots
    if (discriminant < 0) {                       // negative ---> no real roots
        return {false, {0.0, 0.0}};
    }
    double s1 = (-b + std::sqrt(discriminant)) / (2 * a);
    double s2 = (-b - std::sqrt(discriminant)) / (2 * a);
    return {true, {s1, s2}};
}

// Calls solveQuadratic and prints the result.
// result.first = found?, result.second.first = x1, result.second.second = x2
void testQuadratic(double a, double b, double c) {
    std::pair<bool, std::pair<double, double>> result = solveQuadratic(a, b, c);
    std::cout << a << "x^2 + " << b << "x + " << c << " = 0  --->  ";
    if (result.first) {
        std::cout << "x1 = " << result.second.first << ", x2 = " << result.second.second << "\n";
    } else {
        std::cout << "no real solution\n";
    }
}

int main() {
    // Way 1: create the struct first, then assign each member with the dot operator.
    // (Until assigned, idNumber holds garbage: built-in types are not initialized by default.)
    StanfordID id;
    id.name = "Hamid";
    id.sunet = "1944444";
    id.idNumber = 11111;

    // Way 2: brace (uniform) initialization: values are given in the order the members
    // are declared in the struct: name, sunet, idNumber.
    StanfordID id1 = {"mahdis", "dfff", 1656};

    // Testing solveQuadratic

    testQuadratic(1, -3, 2);   // two roots: 2 and 1
    testQuadratic(1, 2, 1);    // one double root: -1 and -1
    testQuadratic(1, 0, 1);    // no real roots (discriminant < 0)
    testQuadratic(2, -4, -6);  // two roots: 3 and -1
    testQuadratic(0, 2, 1);    // a == 0: not a quadratic

    // Same thing with structured bindings (C++17): unpack the pair into named variables
    auto [found, roots] = solveQuadratic(1, -5, 6);
    if (found) {
        auto [x1, x2] = roots;
        std::cout << "x^2 - 5x + 6 = 0  --->  x1 = " << x1 << ", x2 = " << x2 << "\n";
    }
}