/*
=====================================================================================
Lecture 10: Type Safety, std::optional, Enums and Exceptions
=====================================================================================

0) Recap: move semantics
   - std::move enables moving, it does not itself move any data.
         std::vector<int> a = {1, 2, 3};
         std::vector<int> b = std::move(a);
         // b can take ownership of a's resources.
         // a is still valid, but its state is unspecified.
   - Plan for today:
         1. Type safety:   how can types help us catch bugs?
         2. std::optional: how can we represent a missing value?
         3. Enums:         how do we give a set of choices its own type?
         4. Exceptions:    how do we react when something goes wrong at run time?

1) Type safety
   - Type safety: how much a language prevents type errors,
                  how much a language guarantees the behavior of the program,
                  how much a function signature guarantees the behavior of the function.
   - What can the compiler catch before our code runs? A problem caught at compile time:
         int divideByThree(int x) { return x / 3; }
         auto result = divideByThree("Hello");   // ERROR: a string is not an int
   - Key idea: types communicate which operations are allowed.
   - What does this function do?
         void removeOddsFromEnd(std::vector<int>& v) {
             while (v.back() % 2 == 1) {
                 v.pop_back();
             }
         }
     vector::back()     returns a reference to the last element of the vector.
     vector::pop_back() is the opposite of push_back(elem): it removes the last element.
   - What if the vector is empty? Calling back() on an empty vector is UNDEFINED BEHAVIOR:
     the program could crash, give us garbage, or accidentally give us some real value.
         std::vector<int> a = {5, 7, 9};
         removeOddsFromEnd(a);   // all elements are removed... then the loop checks a.back()!
   - One fix: short-circuit evaluation. Check empty() before calling back():
         while (!v.empty() && v.back() % 2 == 1) { v.pop_back(); }
   - Example: see section 1 of the code below.

2) What should back() return?
   - Under the hood, back() is roughly:
         valueType& vector<valueType>::back() { return *(begin() + size() - 1); }
   - One idea: return a pair {"is there a value?", value}:
         std::pair<bool, valueType&> vector<valueType>::back() {
             if (empty()) return {false, valueType()};   // valueType() = default constructor
             return {true, *(begin() + size() - 1)};
         }
   - Problems: what if valueType has no default constructor? Even if it has one, calling the
     constructor can be expensive. So:
         ??? vector<valueType>::back() {
             if (empty()) return ??;
             return *(begin() + size() - 1);
         }

3) Sentinel values
   - What if a function might not have a value to return? Consider a search function:
         int findIndex(const std::vector<int>& v, int target) {
             for (size_t i = 0; i < v.size(); ++i) {
                 if (v[i] == target) return i;
             }
             return -1;
         }
         findIndex({2, 4, 6}, 4);   // returns 1
         findIndex({2, 4, 6}, 9);   // returns -1
   - What does -1 really mean? -1 is not a valid index: the caller must remember our
     convention. A SENTINEL VALUE is easy to forget and does not appear in the return type.
   - What if the return type could say "maybe there is a value"?
   - Example: see section 3 of the code below.

4) std::optional
   - std::optional<T> (#include <optional>) is a template class that either contains a value
     of type T, or contains nothing (std::nullopt).
         std::optional<int> a = 42;             // a contains an int, a.has_value() is true
         std::optional<int> b = std::nullopt;   // b contains nothing, b.has_value() is false
   - nullopt is different from nullptr:
         nullptr: an object that can convert to a value of any POINTER type.
         nullopt: an object that can convert to a value of any OPTIONAL type.
         std::optional<int> x = nullptr;        // ERROR
         int* p = std::nullopt;                 // ERROR: nullopt is not a pointer
   - Creating and resetting optionals:
         std::optional<int> score = std::nullopt;   // empty ({} works too)
         score = 106;                               // now it has a value
         score = std::nullopt;                      // now it does not
   - Example: see section 4 of the code below.

5) Checking and getting the value
   - .has_value()  returns true if it contains a value, false otherwise.
   - if (opt) is shorthand for if (opt.has_value()):
         std::optional<int> result = 5;
         if (result)  { std::cout << "Found something"; }
         if (!result) { std::cout << "No value"; }
   - Getting the value inside:
         std::optional<int> num = 106;
         std::cout << num.value();   // 106
         std::cout << *num;          // 106
   - On an EMPTY optional:
         .value() throws std::bad_optional_access (an exception, see section 10).
         *opt     is undefined behavior. Always check before using *opt!
   - Using a default with .value_or(val): returns the value, or val if it is empty.
         std::optional<int> a = 8;
         std::optional<int> b = std::nullopt;
         std::cout << a.value_or(99);   // 8
         std::cout << b.value_or(99);   // 99
   - Example: see section 5 of the code below.

6) Returning an optional from a function
   - A safer alternative to vector::back() (a hypothetical helper, not an STL method):
         std::optional<int> safeBack(const std::vector<int>& v) {
             if (v.empty()) return std::nullopt;
             return v.back();
         }
         std::vector<int> v;
         v.back();                 // undefined behavior if v is empty
         auto last = safeBack(v);  // empty optional when v is empty
   - Revisiting removeOddsFromEnd: we cannot do arithmetic with an optional, we need to get
     its value if it exists:
         while (safeBack(v).has_value() && safeBack(v).value() % 2 == 1) { v.pop_back(); }
   - Revisiting findIndex: now the signature communicates that the search might fail:
         std::optional<size_t> findIndex(const std::vector<int>& v, int target) {
             for (size_t i = 0; i < v.size(); ++i) {
                 if (v[i] == target) return i;
             }
             return std::nullopt;
         }
         std::optional<size_t> index = findIndex({2, 4, 6}, 4);
         if (index) { ... found ... } else { ... not found ... }
   - Example: see section 6 of the code below.

7) When is std::optional useful? Trade-offs
   - Searching:     a result may be missing.
   - Parsing:       an input might be invalid.
   - Configuration: a setting might not be provided.
   - Advantages: no need for arbitrary sentinel values like -1.
                 The return type tells us that a value might not exist.
   - Trade-offs: callers must handle the possibility of an empty optional.
                 .value() can throw an exception, and dereferencing an empty optional causes
                 undefined behavior.

8) A C++23 aside: chaining optionals
   - These helpers let us compose operations while preserving missing values:
         .and_then(f)   f returns an OPTIONAL. Returns f(value) if there is a value,
                        otherwise nullopt.
         .transform(f)  f returns a plain VALUE. Returns an optional holding f(value) if
                        there is a value, otherwise nullopt.
         .or_else(f)    f returns an OPTIONAL. Returns the optional itself if it has a value,
                        otherwise the result of calling f() (a fallback).
         std::optional<int> x = 8;
         auto squared = x.transform([](int n) { return n * n; });   // optional holding 64
   - Compile with -std=c++23 to run the example (the project default is C++20).
   - Example: see section 8 of the code below.

9) Enums
   - Problem: magic numbers.
         int trafficLight = 2;
         if (trafficLight == 2) { std::cout << "GO"; }   // what does 2 mean?
   - What if each choice had a meaningful name? An ENUM CLASS defines a new type with a fixed
     set of named values:
         enum class TrafficLight { Red, Yellow, Green };
         TrafficLight light = TrafficLight::Green;
   - Enums improve type safety: scoped enum values cannot be implicitly assigned from
     integers:
         light = 2;   // does not compile
   - Plain (unscoped) enum vs enum class (scoped):
         enum Color { Red, Blue };
         Color c = Red;
         int n = c;            // allowed, and the names leak into the surrounding scope

         enum class Color { Red, Blue };
         Color c = Color::Red;
         int n = c;            // ERROR: scoped names and stronger checking
   - Enums can have explicit values. Under the hood, an enum class does not implicitly
     become an int, we must cast it:
         enum class ErrorCode { None = 0, NotFound = 404, ServerError = 500 };
         auto code = ErrorCode::NotFound;
         int n = static_cast<int>(code);   // 404
   - Example: see section 9 of the code below.

10) Exceptions: throw and catch
   - Handle errors so the code keeps running instead of crashing.
         try {
             // code that we check for exceptions
         }
         catch ([exception type] e1) {
             // behavior when we encounter an error of this type
         }
         catch ([exception type] e2) {
             // "else if"
         }
         catch (...) {
             // "else": catch-all
         }
   - throw sends a value (an int, a std::string, a std::exception, ...) to the first catch
     block whose type matches. The rest of the try block is skipped.
   - The standard library throws exceptions too: an empty optional's .value() throws
     std::bad_optional_access, and vector::at() throws std::out_of_range.
   - Example: see section 10 of the code below.

11) Summary
   - Types communicate which operations are allowed, and the compiler checks them for us.
   - Calling back() on an empty vector is undefined behavior: check empty() first.
   - Sentinel values like -1 are easy to forget and do not appear in the return type.
   - std::optional<T> either holds a T or nothing (std::nullopt): the signature says that a
     value might be missing.
   - Check with has_value() or if (opt). .value() throws when empty, *opt is undefined
     behavior when empty, .value_or(val) gives a default.
   - C++23: and_then, transform and or_else chain optionals.
   - enum class gives a set of choices its own type: no magic numbers, no implicit ints.
   - try / throw / catch handle errors at run time without crashing.
*/
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

// =====================================================================================
// 1) Type safety
// =====================================================================================
int divideByThree(int x) { return x / 3; }

// Undefined behavior when v becomes empty: back() is called on an empty vector.
void removeOddsFromEndUnsafe(std::vector<int>& v) {
    while (v.back() % 2 == 1) {
        v.pop_back();
    }
}

// Short-circuit evaluation: back() is only called when the vector is not empty.
void removeOddsFromEnd(std::vector<int>& v) {
    while (!v.empty() && v.back() % 2 == 1) {
        v.pop_back();
    }
}

int main1() {
    // auto result = divideByThree("Hello");   // ERROR: caught at compile time
    std::cout << "  divideByThree(9): " << divideByThree(9) << '\n';

    std::vector<int> a = {2, 4, 5, 7, 9};
    removeOddsFromEnd(a);
    std::cout << "  {2, 4, 5, 7, 9} -> size " << a.size() << '\n';  // 2

    std::vector<int> b = {5, 7, 9};
    // removeOddsFromEndUnsafe(b);   // undefined behavior: b.back() on an empty vector
    removeOddsFromEnd(b);
    std::cout << "  {5, 7, 9} -> size " << b.size() << '\n';  // 0, and no crash
    return 0;
}

// =====================================================================================
// 3) Sentinel values
// =====================================================================================
int findIndexSentinel(const std::vector<int>& v, int target) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] == target) return static_cast<int>(i);
    }
    return -1;  // sentinel: "not found". The return type does not tell us this!
}

int main3() {
    std::vector<int> v = {2, 4, 6};
    std::cout << "  findIndex(4): " << findIndexSentinel(v, 4) << '\n';  // 1
    std::cout << "  findIndex(9): " << findIndexSentinel(v, 9) << '\n';  // -1
    // Forgetting the convention: v[-1] would be undefined behavior.
    return 0;
}

// =====================================================================================
// 4) std::optional
// =====================================================================================
int main4() {
    std::optional<int> num1 = {};  // num1 does not have a value
    std::cout << "  num1.has_value(): " << std::boolalpha << num1.has_value() << '\n';
    num1 = 1;  // now it does
    std::cout << "  num1.has_value(): " << num1.has_value() << '\n';
    num1 = std::nullopt;  // now it does not
    std::cout << "  num1.has_value(): " << num1.has_value() << '\n';

    // nullptr vs nullopt
    int* p = nullptr;
    if (p == nullptr) std::cout << "  p is a null pointer\n";
    std::optional<int> x = std::nullopt;
    if (!x) std::cout << "  x is an empty optional\n";
    // std::optional<int> y = nullptr;   // ERROR
    // int* q = std::nullopt;            // ERROR
    return 0;
}

// =====================================================================================
// 5) Checking and getting the value
// =====================================================================================
int main5() {
    std::optional<int> a = 5;
    std::optional<int> b = std::nullopt;
    std::cout << "  a.has_value(): " << std::boolalpha << a.has_value() << '\n';
    std::cout << "  b.has_value(): " << b.has_value() << '\n';

    if (a) std::cout << "  a.value(): " << a.value() << ", *a: " << *a << '\n';
    if (!b) std::cout << "  b has no value\n";

    std::cout << "  a.value_or(99): " << a.value_or(99) << '\n';  // 5
    std::cout << "  b.value_or(99): " << b.value_or(99) << '\n';  // 99

    // std::cout << *b;        // undefined behavior!
    // std::cout << b.value(); // throws std::bad_optional_access (see section 10)
    return 0;
}

// =====================================================================================
// 6) Returning an optional from a function
// =====================================================================================
std::optional<int> safeBack(const std::vector<int>& v) {
    if (v.empty()) return std::nullopt;
    return v.back();
}

void removeOddsFromEndOptional(std::vector<int>& v) {
    while (safeBack(v).has_value() && safeBack(v).value() % 2 == 1) {
        v.pop_back();
    }
}

std::optional<size_t> findIndex(const std::vector<int>& v, int target) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] == target) return i;
    }
    return std::nullopt;
}

int main6() {
    std::vector<int> empty;
    auto last = safeBack(empty);
    std::cout << "  safeBack({}) has value? " << std::boolalpha << last.has_value() << '\n';

    std::vector<int> v = {5, 7, 9};
    removeOddsFromEndOptional(v);
    std::cout << "  {5, 7, 9} -> size " << v.size() << '\n';

    std::vector<int> nums = {2, 4, 6};
    for (int target : {4, 9}) {
        std::optional<size_t> index = findIndex(nums, target);
        if (index) {
            std::cout << "  found " << target << " at index " << *index << '\n';
        } else {
            std::cout << "  " << target << " not found\n";
        }
    }
    return 0;
}

// =====================================================================================
// 8) A C++23 aside: chaining optionals (compile with -std=c++23)
// =====================================================================================
std::optional<int> half(int x) {
    if (x % 2 == 0) return x / 2;
    return std::nullopt;
}

int main8() {
#if defined(__cpp_lib_optional) && __cpp_lib_optional >= 202110L
    std::optional<int> a = 8;
    auto result = a.and_then(half).and_then(half).and_then(half);  // 8 -> 4 -> 2 -> 1
    if (result) std::cout << "  8 halved three times: " << *result << '\n';

    std::optional<int> b = 7;
    auto result2 = b.and_then(half);  // 7 is odd: nullopt
    std::cout << "  7 halved: " << (result2 ? std::to_string(*result2) : "nullopt") << '\n';

    std::optional<int> x = 8;
    auto squared = x.transform([](int n) { return n * n; });  // optional holding 64
    std::cout << "  8 squared: " << squared.value() << '\n';

    auto fallback = result2.or_else([] { return std::optional<int>{0}; });  // 0
    std::cout << "  fallback: " << fallback.value() << '\n';
#else
    std::cout << "  (needs C++23: compile with -std=c++23)\n";
#endif
    return 0;
}

// =====================================================================================
// 9) Enums
// =====================================================================================
enum class TrafficLight { Red, Yellow, Green };

enum class ErrorCode { None = 0, NotFound = 404, ServerError = 500 };

// Unscoped enum: the names Small and Large leak into the surrounding scope.
enum Size { Small, Large };

int main9() {
    // Magic number: what does 2 mean?
    int trafficLight = 2;
    if (trafficLight == 2) std::cout << "  GO (magic number)\n";

    // Meaningful names instead:
    TrafficLight light = TrafficLight::Green;
    if (light == TrafficLight::Green) std::cout << "  GO (enum class)\n";
    // light = 2;   // does not compile

    Size s = Large;
    int n = s;  // allowed for a plain enum: implicit conversion to int
    std::cout << "  Large as int: " << n << '\n';
    // int m = light;   // ERROR for an enum class

    auto code = ErrorCode::NotFound;
    std::cout << "  ErrorCode::NotFound: " << static_cast<int>(code) << '\n';  // 404
    return 0;
}

// =====================================================================================
// 10) Exceptions: throw and catch
// =====================================================================================
int main10() {
    try {
        int age = 15;
        if (age >= 18) {
            std::cout << "  Access granted\n";
        } else {
            throw age;  // throw an exception: jump to the matching catch below
        }
    } catch (int myNum) {
        std::cout << "  Access denied. Age is: " << myNum << '\n';
    }

    // The standard library throws exceptions too.
    try {
        std::optional<int> empty = std::nullopt;
        std::cout << empty.value();  // throws std::bad_optional_access
    } catch (const std::bad_optional_access& e) {
        std::cout << "  caught bad_optional_access: " << e.what() << '\n';
    }

    try {
        std::vector<int> v = {1, 2, 3};
        std::cout << v.at(10);  // throws std::out_of_range
    } catch (const std::bad_optional_access&) {
        std::cout << "  not this one\n";
    } catch (const std::out_of_range&) {
        std::cout << "  caught out_of_range\n";
    } catch (...) {
        std::cout << "  catch-all\n";
    }
    return 0;
}

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 1) Type safety ---\n";
    main1();
    std::cout << "--- 3) Sentinel values ---\n";
    main3();
    std::cout << "--- 4) std::optional ---\n";
    main4();
    std::cout << "--- 5) Checking and getting the value ---\n";
    main5();
    std::cout << "--- 6) Returning an optional from a function ---\n";
    main6();
    std::cout << "--- 8) Chaining optionals (C++23) ---\n";
    main8();
    std::cout << "--- 9) Enums ---\n";
    main9();
    std::cout << "--- 10) Exceptions ---\n";
    main10();
    return 0;
}
