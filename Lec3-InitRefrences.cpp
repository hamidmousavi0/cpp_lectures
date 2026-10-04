/*
=====================================================================================
Lecture 3: Initialization and References
=====================================================================================

1) Recap
   - auto   ---> "Hey compiler, figure out this type for me." (deduced at compile time)
   - struct ---> "Group these variables together into one new type, please."

2) Initialization
   Initialization = giving a variable its value at the moment it is created (constructed),
   not assigning it later. Three ways:
   1. Direct initialization  : int x = 12;   int x(12);
      - Silently allows NARROWING conversions: int x = 12.7; ---> x is 12 (data lost!)
   2. Uniform (brace) init   : int x{12};    int x = {12};
      - SAFE: narrowing is a compile-time ERROR: int x{12.7}; ---> does not compile.
      - Works for every type: ints, structs, std::vector, std::map, ...
   3. Structured binding (C++17): auto [a, b, c] = someTuple;
      - Initializes several variables at once from a fixed-size data structure
        (std::pair, std::tuple, struct, array). Great for functions that return
        multiple values.
      - The number of elements must be known at compile time.

3) References
   - A reference is an ALIAS: another name for an already-existing object.
     It does not create a new object or a copy; it refers to the same memory.
   - Syntax: Type& name = existingVariable;   e.g. int& r = x;
   - Must be initialized when declared, and cannot be re-bound to another object later.
   - Why important? Function calls:
       pass by value     : void f(int n)  ---> f gets a COPY;   changes do not affect the caller
       pass by reference : void f(int& n) ---> f gets an ALIAS; changes DO affect the caller
                                               (and large objects are not copied ---> faster)

4) l-values and r-values
   - l-value ("locator value"): has a name and a memory address that lasts; can appear on
     the LEFT or the right of =.                       e.g. x, vec, nums[0]
   - r-value ("read value"): a temporary with no lasting address; can only appear on the
     RIGHT of =.                                       e.g. 5, x + 1, getClassInfo()
   - A non-const reference (T&) can only bind to an l-value:  int& r = 5;       // ERROR
   - A const reference (const T&) can bind to an r-value too: const int& r = 5;  // OK

5) const
   - A const object cannot be modified after it is initialized.
   - A const reference (const T&) is a read-only alias: you can read the object through it,
     but not change it. Use it to pass large objects to functions without copying them.
   - You cannot bind a non-const reference to a const object (it would allow modifying it).

6) Compiling C++ programs
   source.cpp ---> compiler (g++, clang++) ---> machine code (executable)
   g++ -std=c++23 main.cpp -o main
   (-std=c++23 selects the C++ standard version; -o main names the output file)
*/
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// =====================================================================================
// 1) Direct initialization: narrowing is allowed (dangerous)
// =====================================================================================
void checkCool(float temperature) {
    if (temperature > 100.0) {
        std::cout << "emergency cooling activated" << '\n';
    } else {
        std::cout << "temperature normal" << '\n';
    }
}

int main1() {
    int numOne = 12.0;  // Direct init. 12.0 is a double, but it is silently converted to int.
    int numTwo(12.0);   // Direct init (constructor syntax), same silent conversion.
    std::cout << "Num one is: " << numOne << std::endl;
    std::cout << "Num two is: " << numTwo << std::endl;

    // Critical bug: 100.8 is narrowed to 100 (the .8 is cut off) WITHOUT any error,
    // so checkCool sees 100, which is NOT > 100 ---> prints "normal" while it is overheating!
    int temperature = 100.8;
    checkCool(temperature);
    return 0;
}

// =====================================================================================
// 2) Uniform (brace) initialization: narrowing is a compile-time error (safe)
// =====================================================================================
int main2() {
    int numOne = {12};  // Uniform init (with =)
    int numTwo{12};     // Uniform init (without =)
    // int numThree{12.0};  // ERROR: narrowing conversion from double to int ---> caught early!
    std::cout << "Num one is: " << numOne << std::endl;
    std::cout << "Num two is: " << numTwo << std::endl;
    return 0;
}

// Uniform init also works for containers.
int main3() {
    std::map<std::string, int> ages{{"Alice", 25}, {"Bob", 30}};  // each {key, value} is a pair
    std::cout << "Alice's age is: " << ages["Alice"] << '\n';

    std::vector<int> numbers{1, 2, 3, 4, 5};
    for (int num : numbers) {  // range-based for loop: num takes each element in turn
        std::cout << num << " ";
    }
    std::cout << std::endl;
    return 0;
}

// =====================================================================================
// 3) Structured binding: unpack several values at once
// =====================================================================================
std::tuple<std::string, std::string, std::string> getClassInfo() {
    std::string className = "CS106L";
    std::string buildingName = "A";
    std::string language = "C++";
    return {className, buildingName, language};  // uniform init builds the tuple
}

int main4() {
    // Structured binding: the 3 tuple elements are unpacked into 3 named variables, in order.
    auto [className, buildingName, language] = getClassInfo();
    std::cout << className << " is in building " << buildingName << " and uses " << language
              << '\n';
    return 0;
}

// =====================================================================================
// 4) References: another name for the same object
// =====================================================================================
int main5() {
    int miToMoon = 238855;  // a normal variable: memory is reserved and the value is stored there
    std::cout << "Moon is " << miToMoon << " mi away" << '\n';

    // ISS is a reference: NO new memory is created. ISS is just another name for miToMoon
    // (same address), so there is no difference between using ISS and using miToMoon.
    int& ISS = miToMoon;
    ISS -= 254;  // changes miToMoon too!
    std::cout << "After ISS -= 254, miToMoon is " << miToMoon << '\n';
    return 0;
}

// Pass by reference: the function works on the caller's variable.
void squareN(int& n) { n = n * n; }  // n is a reference (alias) to the caller's num

int main6() {
    int num = 5;
    squareN(num);  // num is updated: now 25
    std::cout << "Pass by reference: num = " << num << '\n';
    // squareN(5);  // ERROR: 5 is an r-value; a non-const reference (int&) needs an l-value
    return 0;
}

// Pass by value: the function works on a copy.
void squareN_v(int n) { n = n * n; }  // n is a copy of the caller's num

int main7() {
    int num = 5;
    squareN_v(num);  // num is NOT updated: still 5 (only the copy changed)
    std::cout << "Pass by value: num = " << num << '\n';
    return 0;
}

// =====================================================================================
// A classic reference bug: structured binding copies by default
// =====================================================================================
// BUG: "auto [num1, num2]" makes a COPY of each pair, so only the copies are incremented;
// nums itself is NOT modified (even though nums was passed by reference).
void shift(std::vector<std::pair<int, int>>& nums) {
    for (auto [num1, num2] : nums) {
        num1++;
        num2++;
    }
}

// FIX: "auto& [num1, num2]" binds to each pair by REFERENCE, so nums IS modified.
void shift_solved(std::vector<std::pair<int, int>>& nums) {
    for (auto& [num1, num2] : nums) {
        num1++;
        num2++;
    }
}

int main_shift() {
    std::vector<std::pair<int, int>> nums{{1, 1}, {2, 3}};
    shift(nums);
    std::cout << "After shift:        first pair = {" << nums[0].first << ", " << nums[0].second
              << "}\n";  // still {1, 1}
    shift_solved(nums);
    std::cout << "After shift_solved: first pair = {" << nums[0].first << ", " << nums[0].second
              << "}\n";  // now {2, 2}
    return 0;
}

// =====================================================================================
// 5) l-values and r-values
// =====================================================================================
int main8() {
    int x = 5;  // x is an l-value (it has a name and an address): it can be on the left or right
    // int 5 = x;  // ERROR: 5 is an r-value, so it can only appear on the right
    int y = x;  // x used on the right: fine
    std::cout << "x = " << x << ", y = " << y << '\n';
    return 0;
}

// =====================================================================================
// 6) const and const references
// =====================================================================================
int main9() {
    std::vector<int> vec{1, 2, 3};                  // normal vector: can be modified
    const std::vector<int> const_vec{1, 2, 3};      // const vector: read-only
    std::vector<int>& ref_vec{vec};                 // reference to vec: can modify vec
    const std::vector<int>& const_ref{vec};         // const reference to vec: read-only view

    vec.push_back(3);      // OK
    // const_vec.push_back(3);  // ERROR: const_vec is const
    ref_vec.push_back(3);  // OK: modifies vec through the reference
    // const_ref.push_back(3);  // ERROR: cannot modify through a const reference

    // Reading is fine through all of them. vec now has 5 elements (two push_backs).
    std::cout << "vec size = " << vec.size() << ", const_ref size = " << const_ref.size()
              << ", const_vec size = " << const_vec.size() << '\n';
    return 0;
}

int main10() {
    const int a = 5;
    // int& b = a;  // ERROR: a non-const reference to a const object would allow changing a
    const int& b = a;  // OK: a const reference to a const object
    // b++;            // ERROR: b is a const reference (read-only)
    std::cout << "b = " << b << '\n';
    return 0;
}

// =====================================================================================
// 7) Putting it together: loop over a container without copying (const auto&)
// =====================================================================================
// Essay counts every copy made of it: its copy constructor (the function C++ calls to
// make a copy) adds 1 to the global counter. How copy constructors work is for a later lecture.
int copies = 0;
struct Essay {
    std::string text;
    Essay(std::string words) : text(words) {}                       // normal constructor
    Essay(const Essay& other) : text(other.text) { copies += 1; }  // copy constructor: counts
};

using List = std::vector<Essay>;  // type alias (Lecture 2)

int main_essay() {
    // Uniform init of a const vector. Building it from {...} copies each Essay into the
    // vector (3 copies), so we reset the counter to count only what the loops do.
    const List essays{Essay("C++ is fast"), Essay("References avoid copies"),
                      Essay("const keeps them safe")};

    // Version 1: loop variable BY VALUE ---> each element is COPIED into "essay".
    copies = 0;
    std::size_t letters = 0;
    for (Essay essay : essays) {
        letters += essay.text.size();
    }
    std::cout << "By value:     letters read: " << letters << ", copies made: " << copies
              << '\n';  // 55 letters, 3 copies (one per element, wasted work)

    // Version 2: loop variable BY CONST REFERENCE ---> "essay" is an alias to each element.
    // No copies, and const guarantees the loop cannot change the essays.
    copies = 0;
    letters = 0;
    for (const Essay& essay : essays) {
        letters += essay.text.size();
    }
    std::cout << "By const ref: letters read: " << letters << ", copies made: " << copies
              << '\n';  // 55 letters, 0 copies

    // for (Essay& essay : essays) { ... }  // ERROR: essays is const, so a non-const reference
    //                                      // to its elements is not allowed (same rule as main10)
    return 0;
}
// Rule of thumb for range-based for loops:
//   for (auto x : c)        ---> copy each element  (fine for small types like int)
//   for (auto& x : c)       ---> modify the elements in place
//   for (const auto& x : c) ---> read large elements (strings, structs) without copying

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 1) Direct init ---\n";
    main1();
    std::cout << "--- 2) Uniform init ---\n";
    main2();
    main3();
    std::cout << "--- 3) Structured binding ---\n";
    main4();
    std::cout << "--- 4) References ---\n";
    main5();
    main6();
    main7();
    main_shift();
    std::cout << "--- 5) l-values and r-values ---\n";
    main8();
    std::cout << "--- 6) const ---\n";
    main9();
    main10();
    std::cout << "--- 7) const reference in a loop ---\n";
    main_essay();
    return 0;
}
