/*
=====================================================================================
Lecture 8: Function Templates, Lambdas, Operators and Special Member Functions
=====================================================================================

0) Recap: class templates and const correctness
   - Class templates AUTOMATE CODE GENERATION: a template is a factory, not a type.
   - Const objects can only use the CONST INTERFACE of a class.
   - Plan for today:
       Function templates, Concepts, Variadic templates, Template metaprogramming,
       Lambdas, Functors, Algorithms, Ranges and Views,
       Operator overloading, static,
       Special member functions.

1) Function templates
   - I want to take the minimum of two values:
         int min(int a, int b);
         double min(double a, double b);    // the same logic again...
   - A function template keeps the logic and changes the type:
         template <typename T>
         T min(T a, T b) { return a < b ? a : b; }   // this is a TEMPLATE, not a function

         min<std::string>                            // this is a FUNCTION
   - To avoid copying we can add type qualifiers:
         template <typename T>
         T min(const T& a, const T& b) { return a < b ? a : b; }
   - Example: see minByValue and minByRef in section 1 of the code below.

2) Function template instantiation
   - EXPLICIT instantiation: we say what type we want in the angle brackets.
         min<int>(10, 6);     // the compiler generates the code we requested
   - IMPLICIT instantiation: we let the compiler figure out the type.
         min(10, 6);
   - Warning: implicit vs explicit
         min("preston", "emily");   // with T min(T a, T b).  Q: What is the min here?
     A: We do not know! The compiler assumes T = const char*, so it compares two ADDRESSES,
        not two strings.
     Solution: explicit instantiation.
         min<std::string>("preston", "emily");      // "emily"
   - (std::min takes const T&, so there T would be char[8] and char[6], two different types:
     std::min("preston", "emily") does not even compile.)
   - Example: see section 2 of the code below.

3) Function templates with auto
   - T min(T a, T b) does not support a long and an int together. Two type parameters do,
     but what should we return? We would be duplicating code again:
         template <class T, class U>
         T min(T a, U b) { return a < b ? a : b; }     // long min(long, int)

         template <class T, class U>
         U min(T a, U b) { return a < b ? a : b; }     // long min(int, long)
   - Let the compiler work out the return type with auto:
         template <class T, class U>
         auto min(T a, U b) { return a < b ? a : b; }
   - Example: see minAuto in section 3 of the code below.

4) Concepts: our requirements are unclear
         template <typename T>
         long minimum(T a, T b) { return a < b ? a : b; }
   - Nothing here says that T must support a < b. It works for int, but with std::complex
     (which has no <) the error points inside the template.
   - Let's look at the std example:
         template <typename It, typename Value>
         It find(It begin, It end, Value val) {
             It current = begin;
             while (current != end) {
                 if (*current == val) break;
                 ++current;
             }
             return current;
         }

         find(1, 3, 6);    // It becomes int, Value becomes int
   - Q: When does the checking happen?
     A: Only when the compiler generates the code. With It = int, *current does not work,
        so we get an error inside find, far away from our mistake.
   - CONCEPTS make the requirements clear: a concept is a named set of CONSTRAINTS on a type
     that are checked at compile time.

5) Making a Comparable concept
   - We must be able to say T < T:
         template <typename T>
         concept Comparable = requires(T a, T b) {    // for all a and b of type T...
             { a < b } -> std::convertible_to<bool>;  // a < b must compile and the result
         };                                           // must be convertible to bool
   - Then we can use the concept:
         template <typename T>
             requires Comparable<T>
         T min(const T& a, const T& b);

         // or, shorter:
         template <Comparable T>
         T min(const T& a, const T& b);
   - Proper concepts = nice errors. The standard library has concepts too:
         template <std::input_iterator It, typename T>
         It find(It begin, It end, const T& value);

         find(1, 3, 6);    // Q: Why does this not work?
     A: int is not an input iterator. Now the error is at OUR call, and it says exactly that.
   - Example: see Comparable and findChecked in section 5 of the code below.

6) Example: the Printable concept
   - printTwo accepts any type, even one std::cout cannot print. A concept states up front
     what a type must support: a type T is Printable if a value of it can be inserted into
     std::cout.
         template <typename T>
         concept Printable = requires(T value) {
             std::cout << value;      // this must compile, it is only checked and never runs
         };

         template <Printable T, Printable U>
         void printTwo(T first, U second) {
             std::cout << first << ", " << second << std::endl;
         }

         printTwo(1, "hi");       // 1, hi
         printTwo(Grape{}, 1);    // ERROR at this call: Grape does not satisfy Printable
   - Example: see section 6 of the code below.

7) Variadic templates
   - Goal: a min that supports N arguments.
         min(5);
         min(6, 7);
         min(6, 7, 8, 9, 10);
   - Bad solution: one function per number of arguments.
         template <Comparable T>
         T min(T a, T b) { return a < b ? a : b; }

         template <Comparable T>
         T min(T a, T b, T c) { return a < b ? (a < c ? a : c) : (c < b ? c : b); }
   - Improved bad solution: reuse the smaller one.
         template <Comparable T>
         T min3(T a, T b, T c) { return min2(a, min2(b, c)); }
   - That is recursion! So we write a BASE CASE and a RECURSIVE CASE:
         template <Comparable T>
         T min(const T& v) { return v; }

         template <Comparable T, Comparable... Args>
         T min(const T& v, const Args&... args) {
             auto m = min(args...);
             return v < m ? v : m;
         }
   - VARIADIC TEMPLATE: matches 0 or more types     (Comparable... Args)
   - PARAMETER PACK   : takes 0 or more parameters  (const Args&... args)
   - PACK EXPANSION   : passes all of the args      (args...)
   - Example: see minOf in section 7 of the code below.

8) Example: commaSeparate
   - Prints any number of Printable values, separated by ", ". A pack cannot be printed in
     one go (<< rest << does not compile), so we use recursion again.
         template <Printable T>
         void commaSeparate(T last) {                  // base case
             std::cout << last << std::endl;
         }

         template <Printable T, Printable... Rest>
         void commaSeparate(T first, Rest... rest) {   // recursive case
             std::cout << first << ", ";
             commaSeparate(rest...);
         }

         commaSeparate(1, "hi", 2.5);    // 1, hi, 2.5
   - The base case must be declared first, so the recursive case can see it.
   - Example: see section 8 of the code below.

9) Template metaprogramming (TMP)
   - How can we do work at COMPILE TIME?
         template <int N>
         struct Factorial {
             enum { value = N * Factorial<N - 1>::value };
         };

         template <>
         struct Factorial<0> {           // the base case is a template SPECIALIZATION
             enum { value = 1 };         // an enum stores a compile time constant
         };

         std::cout << Factorial<5>::value << std::endl;   // all of this happens at compile time
   - TMP bakes results into the executable at compile time. It is used for:
       - optimizing matrices, trees and other mathematical structures,
       - policy based design: passing around behaviour through the templates,
       - the Boost MPL library.

10) Neat compile time execution: constexpr and consteval
   - The same idea, as readable code:
         constexpr size_t factorial(size_t n) {   // Dear compiler, TRY to run me at compile time
             if (n == 0) return 1;
             return n * factorial(n - 1);
         }

         consteval size_t factorial(size_t n) {   // Dear compiler, you MUST run me at compile time
             if (n == 0) return 1;
             return n * factorial(n - 1);
         }
   - Example: see Factorial and factorial in sections 9 and 10 of the code below.

11) When to use templates
   - I want the compiler to automate a repetitive coding task
       ---> template functions and variadic templates.
   - I want better error messages
       ---> concepts.
   - I do not want to wait until run time
       ---> template metaprogramming and constexpr / consteval.

12) Functions and lambdas: how can we represent functions as variables?
   - Recall our templated find function:
         template <typename It, typename T>
         It find(It first, It last, const T& value) {
             for (auto it = first; it != last; ++it) {
                 if (*it == value) return it;
             }
             return last;
         }
   - It works for vectors, and even for other containers. We defined it in a general way.
         std::vector<std::string> v{"run", "forrest"};
         auto it1 = find(v.begin(), v.end(), "run");

         std::set<std::string> s{"run", "forrest"};
         auto it2 = find(s.begin(), s.end(), "run");
   - How can we make find even more general? Instead of searching for a value, what if we
     could ask arbitrary questions?
       - A vowel in a string?
       - A prime number in a vector<int>?
       - A number divisible by 5 in a set<int>?
   - A PREDICATE is a boolean-valued function.
         bool isVowel(char c);
         bool isDivisible(int n, int d) { return n % d == 0; }
   - Key idea: we need to pass a predicate to a function.
   - Example: see findValue in section 12 of the code below.

13) Passing a predicate: find_if
   - *it == value works only for finding a specific value. We replace this critical part of
     the code with a call of our predicate:
         template <typename It, typename ????>
         It find(It first, It last, ???? pred) {
             for (auto it = first; it != last; ++it) {
                 if (pred(*it)) return it;     // run pred on each value, stop at the first match
             }
             return last;
         }
   - The question is: what is the TYPE of the predicate? We make it a template parameter, and
     the compiler figures it out with implicit instantiation:
         template <typename It, typename Pred>
         It find_if(It first, It last, Pred pred);

         std::string flower = "rose";
         auto it = find_if(flower.begin(), flower.end(), isVowel);
         // what is the type of isVowel? The compiler says: do not worry about it.
   - Passing functions allows us to GENERALIZE an algorithm with USER DEFINED behaviour.
   - Example: see myFindIf in section 13 of the code below.

14) Functions generalize poorly
   - Consider that we want to find a number less than N in a vector:
         bool lessThan5(int x) { return x < 5; }
         bool lessThan6(int x) { return x < 6; }
         bool lessThan7(int x) { return x < 7; }

         find_if(begin, end, lessThan5);
   - What if we do not know N until run time?
         int n;
         std::cin >> n;
         find_if(begin, end, ???);
   - Q: Why would this not work?
         bool isLessThan(int elem, int n) { return elem < n; }
     A: We cannot add another parameter: find_if only passes ONE parameter to pred.
         if (pred(*it)) return it;
   - We want to give our function extra STATE, without introducing a new parameter.

15) Lambda functions
   - LAMBDA functions are functions that can capture state from an enclosing scope.
         int n;
         std::cin >> n;
         auto lessThanN = [n](int x) { return x < n; };

         find_if(begin, end, lessThanN);
       auto    ---> I do not know the type, but the compiler does.
       [n]     ---> capture clause: lets us use outside variables.
       (int x) ---> parameters, exactly like a normal function.
       { ... } ---> function body, exactly like a normal function, except that only parameters
                    and captures are in scope.
   - A lambda PARAMETER is supplied when the lambda is CALLED, but a CAPTURE comes from the
     enclosing scope when the lambda is CREATED.
   - Example: see section 15 of the code below.

16) A note on captures
         auto lambda = [capture-values](arguments) { return expression; };

         [x](arguments)     ---> capture x by value (makes a copy)
         [&x](arguments)    ---> capture x by reference
         [x, y](arguments)  ---> capture x and y by value
         [&](arguments)     ---> capture everything by reference
         [&, x](arguments)  ---> capture everything by reference, except x by value
         [=](arguments)     ---> capture everything by value
   - Example: see section 16 of the code below.

17) How do lambdas work? Functors
   - A FUNCTOR is any object that defines an operator().
     In English: an object that acts like a function.
         template <typename T>
         struct greater {                     // std::greater
             bool operator()(const T& a, const T& b) const { return a > b; }
         };

         std::greater<int> g;
         g(1, 2);                             // seems like a function
   - Since a functor is an object, it can have STATE:
         struct MyFunctor {
             int operator()(int a) const { return a * value; }
             int value;
         };

         MyFunctor f;
         f.value = 5;
         f(10);                               // 50
   - When we use a lambda, a functor type is generated:
         int n = 10;
         auto lessThanN = [n](int x) { return x < n; };
     is equivalent to:
         class __lambda_6_8 {                 // a random name only the compiler will see
         public:
             bool operator()(int x) const { return x < n; }   // functor call operator
             __lambda_6_8(int& _n) : n{_n} {}                 // class constructor
         private:
             int n;                                           // captures become fields
         };

         int n = 10;
         auto lessThanN = __lambda_6_8{n};    // the capture is passed to the constructor
   - Example: see section 17 of the code below.

18) Where do we use functions and lambdas? Algorithms
   - <algorithm> is a collection of template functions that operate on ITERATORS.
   - It lets us inspect and transform data.
   - Example: see section 18 of the code below.

19) Ranges
   - Ranges are a new version of the STL ("the STL v2").
   - A RANGE is anything with a begin and an end.
       e.g. std::vector<T>, std::map<K, V>, your own custom type with a begin and end
   - Range algorithms operate on ranges. std::ranges provides new versions of the <algorithm>
     functions, for most of the STL algorithms. They are very new.
         std::vector<char> v = {'a', 'b', 'c'};
         auto it = std::ranges::find(v, 'c');   // we can pass v because it is a range
   - We can still work with iterators if we need to:
         auto first = v.begin() + 1;
         auto last = v.end() - 1;
         auto it = std::ranges::find(first, last, 'c');
   - Example: see section 19 of the code below.

20) Views: a way to compose algorithms
   - A VIEW is a range that LAZILY adapts another range.
   - Filter and transform in the old STL:
         std::vector<char> v = {'a', 'b', 'c', 'd', 'e'};
         std::vector<char> f;
         std::copy_if(v.begin(), v.end(), std::back_inserter(f), isVowel);

         std::vector<char> t;
         std::transform(f.begin(), f.end(), std::back_inserter(t), toUpper);
   - Filter and transform with views:
         std::vector<char> letters = {'a', 'b', 'c', 'd', 'e'};
         auto f = std::ranges::views::filter(letters, isVowel);
         auto t = std::ranges::views::transform(f, toUpper);
         auto vowelUpper = std::ranges::to<std::vector<char>>(t);
   - Views are COMPOSABLE. We can chain them together with operator |:
         std::vector<char> upperVowel = letters
                                        | std::ranges::views::filter(isVowel)
                                        | std::ranges::views::transform(toUpper)
                                        | std::ranges::to<std::vector<char>>();
   - Views compose operations WITHOUT immediately constructing intermediate containers.
     std::ranges::to materializes the output into a vector.
   - (std::ranges::to needs C++23. We compile with C++20, so the code below uses a loop.)
   - Pro tip: views are like Python generators.
   - Example: see section 20 of the code below.

21) Operator overloading: why?
   - Q: What does the compiler turn this lambda into?
         auto isStrong = [](int power) { return power > 80; };
     A: A functor, an object that overloads operator():
         class __lambda {
         public:
             bool operator()(int power) const { return power > 80; }
         };
     Wait, we can overload operators???
   - I want the min of two heroes:
         Hero captain{"Captain Const", 70};
         Hero lass{"Lambda Lass", 90};
         Hero weaker = std::min(captain, lass);
         // compiler: what does < even mean for a Hero??

         template <typename T>
         const T& min(const T& a, const T& b) { return (b < a) ? b : a; }
   - Same problem with std::sort, std::set<Hero> and std::map<Hero, V>.
   - Operators allow us to convey meaning about types that functions do not:
         Hero team = captain.teamUpWith(lass);
         if (captain.isWeakerThan(lass)) { ... }   // feels like a random function call

         Hero team = captain + lass;
         if (captain < lass) { ... }               // instantly readable: heroes can be
                                                   // added and compared
   - What can we overload? Almost all operators, even new, new[], delete, delete[], [] and ().
     (() is how functors work.)
   - What can we NOT overload? :: (scope resolution), ?: (ternary), . (member access),
     .* (pointer to member access), sizeof, typeid, and casts like static_cast.
     These work on names and types, not values. Changing them would break the language.

22) Operator overloading syntax
         return_type operator<symbol>(parameter_list);

         class Hero {
         private:
             std::string name;
             int power;
         public:
             Hero(std::string name, int power);
             std::string describe() const;
             bool operator<(const Hero& other) const;   // declare it just like other members
         };

23) Member vs non-member
   - Member:
         bool Hero::operator<(const Hero& rhs) const {
             return power < rhs.power;     // inside the class, the left side is this
         }
   - Non-member (no const at the end: there is no object, so no this):
         bool operator<(const Hero& lhs, const Hero& rhs) {
             return lhs.power < rhs.power; // outside any class, both sides are parameters
         }
   - Why do we need non-member? It is preferred by the STL.
       #1: The left side can be a type that is not your class:
             Hero buffed = 2 * captain;    // 2 is an int: no member
       #2: We can overload with classes we do not own:
             std::cout << captain;         // we cannot add members to std::ostream
       #3: It is SYMMETRIC: both sides are treated the same way.
   - But what about the secret identities? A non-member cannot read private members:
         return lhs.power < rhs.power;     // ERROR: power is private
     With public getters it is fine, nothing else is needed:
         return lhs.getPower() < rhs.getPower();
   - Do not write both a member and a non-member <. The call becomes ambiguous.

24) Hello friend
   - friend lets a non-member function (or class) access the private members of a class.
         class Hero {
         private:
             std::string name;
             int power;
         public:
             Hero(std::string name, int power);
             friend Hero operator+(const Hero& lhs, const Hero& rhs);
         };
   - We declare it inside the class: the CLASS grants friendship. It is still a non-member.
   - With public getters, friend is not needed at all.
   - With great power comes great responsibility.

25) Principle of Least Astonishment (PoLA)
   - Operators should mean what everyone expects them to mean.
   - Do not make operator+ do set subtraction.
   - If the meaning is not obvious, write a named function instead.

26) The classic friend: operator<<
         // hero.h
         friend std::ostream& operator<<(std::ostream& out, const Hero& hero);

         // hero.cpp
         std::ostream& operator<<(std::ostream& out, const Hero& hero) {
             out << hero.name << " (power " << hero.power << ")";
             return out;
         }
   - Example: see Hero in section 26 of the code below.

27) The static keyword: static member variables
   - How many heroes are there? Goal: count every hero that has ever been created.
         class Hero {
             int heroCount = 0;
         };
         Hero::Hero() { heroCount++; }
     Every hero gets their own heroCount, so each one is just 1.
   - We want one variable shared by the whole class: a STATIC member variable.
         class Hero {
             std::string name;
             int power;
             static int heroCount;
         };

         int Hero::heroCount = 0;
         Hero::Hero() { heroCount++; }
       - It belongs to the CLASS, not to one object.
       - Define it once, outside the class, in hero.cpp.
       - C++17: inline static lets us define it right in the header.

28) Static member functions
         class Hero {
         public:
             static int getHeroCount();    // write static only on the declaration
         };

         int Hero::getHeroCount() { return heroCount; }

         std::cout << Hero::getHeroCount();
   - Called on the class, Hero::getHeroCount(), with no object.
   - There is no this, so it can only touch static members.
   - Q: Will this compile? Why or why not?
         int Hero::getHeroCount() { return power; }
     A: No. Whose power? A static function is not called on any hero, so there is no this.
   - Regular member functions can still use static members.

29) Static wears many capes
   - In a class      : shared by every object. static int heroCount;   // one copy for all
   - In a function   : keeps its value between calls.
         int rescueCount() {
             static int count = 0;    // initialized once
             return ++count;
         }
   - At file scope   : only visible inside that .cpp file.
   - Example: see section 29 of the code below.

30) Special member functions (SMFs)
   - Recap:
         bool operator<(const StudentId& lhs, const StudentId& rhs);       // non-member
         bool StudentId::operator<(const StudentId& rhs) const { ... };    // member
   - Classes have data members and functions. Some of the functions are SPECIAL: we do not have
     to write them. They all have default versions, generated automatically when they are
     needed.
         class Widget {
         public:
             Widget();                          // 1) default constructor
             ~Widget();                         // 2) destructor
             Widget(const Widget& w);           // 3) copy constructor
             Widget& operator=(const Widget& w);// 4) copy assignment operator
             Widget(Widget&& w);                // 5) move constructor
             Widget& operator=(Widget&& rhs);   // 6) move assignment operator
         };
   - 1) Default constructor: takes no parameters and creates a new object.
   - 2) Destructor: called when an object goes out of scope.
   - 3) Copy constructor: creates a NEW object as a member-wise copy of another.
         Widget widgetOne;
         Widget widgetTwo = widgetOne;   // copy constructor is called
   - 4) Copy assignment operator: assigns an already EXISTING object to another.
         Widget widgetOne;
         Widget widgetTwo;
         widgetOne = widgetTwo;          // both objects were constructed before the =
   - 5) and 6) Move constructor and move assignment operator: we have a full lecture on this.
   - Example: see Widget in section 30 of the code below.

31) Member initializer lists
   - When we create a constructor, we need to initialize all the member variables:
         template <typename T>
         Vector<T>::Vector() {
             _size = 0;
             _capacity = 4;
             _data = new T[_capacity];
         }
   - There are two steps here: the members may be default initialized first, and then they
     are assigned. That doubles the work.
   - A MEMBER INITIALIZER LIST directly constructs the members with the intended values:
         template <typename T>
         Vector<T>::Vector() : _size(0), _capacity(4), _data(new T[_capacity]) {}
   - What if the variable is a non-assignable type?
         class MyClass {
             const int _constant;
             int& _reference;
         public:
             MyClass(int value, int& ref) : _constant(value), _reference(ref) {}
         };
     Q: This code only works with an initializer list. Why?
     A: We cannot assign a value to a const or a reference member after it is created.
   - Initializer lists can be used for any constructor, even non-default ones with parameters.
   - Example: see Vector and MyClass in section 31 of the code below.

32) Why should we override SMFs? Shallow vs deep copy
   - The compiler gives them to us for free, and by default the copy constructor copies each
     member variable: MEMBER-WISE copying. Is this always good enough?
   - Consider pointers: a member-wise copy points to the same allocated data, not a fresh copy.
         template <typename T>
         Vector<T>::Vector(const Vector<T>& other)
             : _size(other._size), _capacity(other._capacity), _data(other._data) {}
     Both pointers point at the same array, so anything done to one affects the other.
     (And both destructors will delete the same array!)
   - DEEP COPY: an object that is a complete, independent copy of the original. In these cases
     we override the SMFs with our own implementation. Declare them in the header and write
     their implementation in the .cpp file, like any function.
         template <typename T>
         Vector<T>::Vector(const Vector<T>& other)
             : _size(other._size), _capacity(other._capacity), _data(new T[other._capacity]) {
             for (size_t i = 0; i < _size; ++i) {
                 _data[i] = other._data[i];
             }
         }
   - Example: see the copy constructor of Vector in section 32 of the code below.

33) How do you prevent copies?
   - Let's say you have a class that handles all of your passwords. Nobody should copy it.
   - We can DELETE special member functions: setting an SMF to delete removes it.
         class PasswordManager {
         public:
             PasswordManager();
             ~PasswordManager();
             PasswordManager(const PasswordManager& rhs) = delete;
             PasswordManager& operator=(const PasswordManager& rhs) = delete;
         };
     Now copying is not a possible operation.
   - We can selectively allow the functionality of SMFs. What if only one instance should
     be allowed to own a resource? This is how classes like std::unique_ptr work.
   - Example: see PasswordManager in section 33 of the code below.

34) Rule of Zero and Rule of Three
   - RULE OF ZERO: if the default SMFs work, do not define your own.
     Define new ones only when the ones generated by the compiler will not work. This usually
     happens with dynamically allocated memory, like pointers to things on the heap.
   - RULE OF THREE: if you need a custom destructor, then you probably also need to define a
     copy constructor and a copy assignment operator.
     Q: Why is this the case?
     A: A custom destructor means you are manually managing memory. The copies generated by
        the compiler would only copy the pointer, not the data it points to.
   - Is copying enough? It can be wasteful: copying a big container copies every value one by
     one, which is very slow. ---> The solution is move semantics.

35) Summary
   - Function templates and variadic templates automate repetitive code.
   - CONCEPTS name the requirements of a template and give nice errors.
   - constexpr and consteval (and TMP) do work at compile time.
   - PREDICATES are functions that return a boolean.
   - LAMBDAS are anonymous functions that can capture outside variables.
   - FUNCTORS are objects that act like functions, using operator().
   - STL ALGORITHMS are built-in functions for searching, sorting and transforming data.
   - RANGES and VIEWS simplify working with containers and support lazy operations.
   - Operator overloading lets our types act like built-in types.
   - Operators should convey an obvious meaning, and only overload what you need.
   - Prefer non-member operators, and add friend when they need private members.
   - Static members belong to the class, not to any one object.
   - SMFs have default versions: follow the Rule of Zero, or else the Rule of Three.
   - Use a member initializer list, and delete the SMFs you do not want.
*/
#include <algorithm>
#include <cctype>
#include <complex>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <ranges>
#include <set>
#include <string>
#include <vector>

// Note: this file uses names like minOf and myFind instead of min and find. With std types as
// arguments, a plain min or find could clash with std::min and std::find.

// =====================================================================================
// 1) Function templates
// =====================================================================================
// Without templates: the same logic, once per type.
int minInt(int a, int b) { return a < b ? a : b; }
double minDouble(double a, double b) { return a < b ? a : b; }

// To have a function for different types we can use a template.
template <typename T>
T minByValue(T a, T b) {
    return a < b ? a : b;
}

// To avoid copying we can use references.
template <typename T>
T minByRef(const T& a, const T& b) {
    return a < b ? a : b;
}

int main1() {
    std::cout << minInt(106, 107) << " " << minDouble(1.5, 2.5) << '\n';       // 106 1.5
    std::cout << minByValue(106, 107) << " " << minByValue(1.5, 2.5) << '\n';  // 106 1.5
    return 0;
}

// =====================================================================================
// 2) Function template instantiation
// =====================================================================================
int main2() {
    std::cout << minByRef<int>(10, 6) << '\n';  // explicit: 6
    std::cout << minByRef(10, 6) << '\n';       // implicit, like auto: 6

    // minByValue("preston", "emily");    // compiles, but it compares two ADDRESSES
    std::cout << minByValue<std::string>("preston", "emily") << '\n';  // emily
    return 0;
}

// =====================================================================================
// 3) Function templates with auto
// =====================================================================================
template <class T, class U>
auto minAuto(T a, U b) {  // the compiler works out the return type
    return a < b ? a : b;
}

int main3() {
    long bigger = 5;
    int smaller = 3;
    std::cout << minAuto(bigger, smaller) << " " << minAuto(smaller, bigger) << '\n';  // 3 3
    return 0;
}

// =====================================================================================
// 4) Concepts: our requirements are unclear
// =====================================================================================
template <typename T>
long minimum(T a, T b) {  // T must support <, but nothing here says that
    return a < b ? a : b;
}

template <typename It, typename Value>
It myFind(It begin, It end, Value val) {
    It current = begin;
    while (current != end) {
        if (*current == val) break;
        ++current;
    }
    return current;
}

int main4() {
    int a = 5;
    int b = 3;
    std::cout << minimum(a, b) << '\n';  // 3
    // minimum(std::complex<double>{1, 2}, std::complex<double>{3, 4});  // ERROR: inside minimum

    std::vector<int> nums{4, 2, 6, 7};
    auto it = myFind(nums.begin(), nums.end(), 6);
    std::cout << *it << '\n';  // 6
    *it = 107;                 // we can change the element through the iterator
    // myFind(1, 3, 6);        // ERROR: inside myFind, *current does not work on an int
    return 0;
}

// =====================================================================================
// 5) Making a Comparable concept
// =====================================================================================
template <typename T>
concept Comparable = requires(T a, T b) {    // given a and b, the following must hold
    { a < b } -> std::convertible_to<bool>;  // must compile, and the result must be a boolean
};

template <typename T>
    requires Comparable<T>
T minRequires(const T& a, const T& b) {
    return a < b ? a : b;
}

// or, shorter:
template <Comparable T>
T minConcept(const T& a, const T& b) {
    return a < b ? a : b;
}

template <std::input_iterator It, typename T>
It findChecked(It first, It last, const T& value) {
    for (auto it = first; it != last; ++it) {
        if (*it == value) return it;
    }
    return last;
}

int main5() {
    std::cout << minRequires(1, 2) << " " << minConcept(1.5, 0.5) << '\n';  // 1 0.5
    // minConcept(std::complex<double>{1, 2}, std::complex<double>{3, 4});  // ERROR: not Comparable

    std::vector<int> nums{4, 2, 6, 7};
    std::cout << *findChecked(nums.begin(), nums.end(), 2) << '\n';  // 2
    // findChecked(1, 3, 6);      // ERROR at this call: int is not an input iterator
    return 0;
}

// =====================================================================================
// 6) Example: the Printable concept
// =====================================================================================
template <typename T>
concept Printable = requires(T value) {
    std::cout << value;  // this must compile, it is only checked and never runs
};

template <Printable T, Printable U>
void printTwo(T first, U second) {
    std::cout << first << ", " << second << std::endl;
}

struct Grape {};  // std::cout does not know how to print a Grape

int main6() {
    printTwo(1, "hi");  // 1, hi
    // printTwo(Grape{}, 1);      // ERROR at this call: Grape does not satisfy Printable
    return 0;
}

// =====================================================================================
// 7) Variadic templates: a min that supports N arguments
// =====================================================================================
// The improved bad solution
template <Comparable T>
T min2(T a, T b) {
    return a < b ? a : b;
}

template <Comparable T>
T min3(T a, T b, T c) {
    return min2(a, min2(b, c));
}

// Base case
template <Comparable T>
T minOf(const T& v) {
    return v;
}

// Recursive case
template <Comparable T, Comparable... Args>
T minOf(const T& v, const Args&... args) {
    auto m = minOf(args...);  // pack expansion: passes all of the args
    return v < m ? v : m;
}

int main7() {
    std::cout << min3(4, 2, 3) << '\n';          // 2
    std::cout << minOf(5) << '\n';               // 5
    std::cout << minOf(7, 6) << '\n';            // 6
    std::cout << minOf(8, 6, 7, 9, 10) << '\n';  // 6
    return 0;
}

// =====================================================================================
// 8) Example: commaSeparate
// =====================================================================================
// Base case: one value left, so no comma, just end the line.
template <Printable T>
void commaSeparate(T last) {
    std::cout << last << std::endl;
}

// Recursive case: print the first value, then handle the rest.
template <Printable T, Printable... Rest>
void commaSeparate(T first, Rest... rest) {
    std::cout << first << ", ";
    commaSeparate(rest...);
}

int main8() {
    commaSeparate(1, "hi", 2.5);  // 1, hi, 2.5
    commaSeparate(7);             // 7
    return 0;
}

// =====================================================================================
// 9) Template metaprogramming: work at compile time
// =====================================================================================
template <int N>
struct Factorial {
    enum { value = N * Factorial<N - 1>::value };
};

template <>
struct Factorial<0> {    // base case: a template specialization
    enum { value = 1 };  // enum is a way to store a compile time constant
};

int main9() {
    std::cout << Factorial<5>::value << '\n';  // 120, already computed by the compiler
    return 0;
}

// =====================================================================================
// 10) constexpr and consteval
// =====================================================================================
constexpr size_t factorial(size_t n) {  // Dear compiler, try to run me at compile time.
    if (n == 0) return 1;
    return n * factorial(n - 1);
}

consteval size_t factorialConsteval(size_t n) {  // Dear compiler, you must run me at compile time.
    if (n == 0) return 1;
    return n * factorialConsteval(n - 1);
}

int main10() {
    static_assert(factorial(5) == 120);          // static_assert is checked at compile time
    std::cout << factorialConsteval(5) << '\n';  // 120

    size_t n = 6;                       // a normal variable, not known at compile time
    std::cout << factorial(n) << '\n';  // 720, constexpr falls back to run time
    // factorialConsteval(n);     // ERROR: consteval cannot run at run time
    return 0;
}

// =====================================================================================
// 12) Recall: a templated find
// =====================================================================================
template <typename It, typename T>
It findValue(It first, It last, const T& value) {
    for (auto it = first; it != last; ++it) {
        if (*it == value) return it;
    }
    return last;
}

// Predicates: boolean-valued functions
char toUpper(char c) { return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); }

bool isVowel(char c) {
    char upper = toUpper(c);
    return upper == 'A' || upper == 'E' || upper == 'I' || upper == 'O' || upper == 'U';
}

bool isDivisible(int n, int d) { return n % d == 0; }

int main11() {
    std::vector<std::string> v{"run", "forrest"};
    auto it1 = findValue(v.begin(), v.end(), "run");
    std::cout << *it1 << '\n';  // run

    std::set<std::string> s{"run", "forrest"};  // the same function works for a set
    auto it2 = findValue(s.begin(), s.end(), "forrest");
    std::cout << *it2 << '\n';  // forrest
    return 0;
}

// =====================================================================================
// 13) Passing a predicate: find_if
// =====================================================================================
template <typename It, typename Pred>
It myFindIf(It first, It last, Pred pred) {
    for (auto it = first; it != last; ++it) {
        if (pred(*it)) return it;  // we only pass one parameter to pred
    }
    return last;
}

bool lessThan5(int x) { return x < 5; }

bool isLessThan(int elem, int n) { return elem < n; }

int main12() {
    std::string flower = "rose";
    auto it = myFindIf(flower.begin(), flower.end(), isVowel);
    std::cout << *it << '\n';  // o

    std::vector<int> nums{7, 6, 4, 2};
    std::cout << *myFindIf(nums.begin(), nums.end(), lessThan5) << '\n';  // 4
    // myFindIf(nums.begin(), nums.end(), isLessThan);  // ERROR: isLessThan needs two parameters
    return 0;
}

// =====================================================================================
// 15) Lambda functions
// =====================================================================================
int main13() {
    int n = 5;  // imagine: std::cin >> n;
    auto lessThanN = [n](int x) { return x < n; };

    std::vector<int> nums{7, 6, 4, 10};
    std::cout << *myFindIf(nums.begin(), nums.end(), lessThanN) << '\n';  // 4

    // A lambda can also use the two parameter functions: the capture is the extra state.
    int d = 5;
    auto divisibleByD = [d](int x) { return isDivisible(x, d); };
    std::cout << *myFindIf(nums.begin(), nums.end(), divisibleByD) << '\n';  // 10

    auto lessThan = [n](int x) { return isLessThan(x, n); };
    std::cout << lessThan(4) << '\n';  // 1 (true)
    return 0;
}

// =====================================================================================
// 16) Captures
// =====================================================================================
int main14() {
    int x = 1;
    int y = 2;
    auto byValue = [x]() { return x; };       // a copy of x, made here
    auto byReference = [&x]() { return x; };  // sees later changes of x
    x = 10;
    std::cout << byValue() << " " << byReference() << '\n';  // 1 10

    auto twoValues = [x, y]() { return x + y; };
    std::cout << twoValues() << '\n';  // 12

    auto everythingByReference = [&]() {
        x++;
        y++;
    };
    everythingByReference();
    std::cout << x << " " << y << '\n';  // 11 3

    auto allButX = [&, x]() { return x + y; };  // y by reference, x by value
    auto everythingByValue = [=]() { return x + y; };
    std::cout << allButX() << " " << everythingByValue() << '\n';  // 14 14
    return 0;
}

// =====================================================================================
// 17) Functors: objects that act like functions
// =====================================================================================
struct MyFunctor {
    int operator()(int a) const { return a * value; }
    int value;  // a functor is an object, so it can have state
};

// What the compiler generates for: auto lessThanN = [n](int x) { return x < n; };
class LessThanN {
   public:
    bool operator()(int x) const { return x < n; }  // functor call operator
    LessThanN(int& _n) : n{_n} {}                   // class constructor

   private:
    int n;  // our captures become fields in the class
};

int main15() {
    std::greater<int> g;
    std::cout << g(1, 2) << '\n';  // 0 (false): it looks like a function

    MyFunctor f;
    f.value = 5;
    std::cout << f(10) << '\n';  // 50

    int n = 10;
    auto lessThanN = LessThanN{n};      // capturing n by passing it to the constructor
    std::cout << lessThanN(3) << '\n';  // 1 (true)
    return 0;
}

// =====================================================================================
// 18) Algorithms
// =====================================================================================
int main16() {
    std::vector<int> nums{7, 2, 9, 4};

    int limit = 5;
    auto isBig = [limit](int x) { return x > limit; };
    std::cout << std::count_if(nums.begin(), nums.end(), isBig) << '\n';  // 2

    std::sort(nums.begin(), nums.end(), std::greater<int>{});  // sort with a functor
    std::cout << nums[0] << '\n';                              // 9
    return 0;
}

// =====================================================================================
// 19) Ranges
// =====================================================================================
int main17() {
    std::vector<char> v = {'a', 'b', 'c', 'd', 'e'};
    auto it1 = std::ranges::find(v, 'c');  // we can pass v because it is a range
    std::cout << *it1 << '\n';             // c

    auto first = v.begin() + 1;  // we can still work with iterators
    auto last = v.end() - 1;
    auto it2 = std::ranges::find(first, last, 'c');
    std::cout << *it2 << '\n';  // c
    return 0;
}

// =====================================================================================
// 20) Views
// =====================================================================================
void printChars(const std::vector<char>& v) {
    for (char c : v) {
        std::cout << c << " ";
    }
    std::cout << '\n';
}

int main18() {
    std::vector<char> letters = {'a', 'b', 'c', 'd', 'e'};

    // The old STL: one intermediate vector per step
    std::vector<char> f;
    std::copy_if(letters.begin(), letters.end(), std::back_inserter(f), isVowel);
    std::vector<char> t;
    std::transform(f.begin(), f.end(), std::back_inserter(t), toUpper);
    printChars(t);  // A E

    // Views: nothing is computed yet, the view only describes the steps
    auto upperVowels =
        letters | std::ranges::views::filter(isVowel) | std::ranges::views::transform(toUpper);

    // Lazy: the work happens here, one element at a time.
    // In C++23: auto result = upperVowels | std::ranges::to<std::vector<char>>();
    std::vector<char> result;
    for (char c : upperVowels) {
        result.push_back(c);
    }
    printChars(result);  // A E
    return 0;
}

// =====================================================================================
// 21-26) Operator overloading
// =====================================================================================
// ---- file: hero.h ----
class Hero {
   private:
    std::string name;
    int power;

   public:
    Hero(std::string name, int power);
    std::string describe() const;
    int getPower() const;

    // Section 23: member, the left side is this
    bool operator<(const Hero& other) const;

    // Sections 24 and 26: non-members that need the private fields, so the class grants friendship
    friend Hero operator+(const Hero& lhs, const Hero& rhs);
    friend Hero operator*(int factor, const Hero& hero);
    friend std::ostream& operator<<(std::ostream& out, const Hero& hero);
};

// Section 23: non-member with public getters, so no friend is needed
bool operator>(const Hero& lhs, const Hero& rhs);

// ---- file: hero.cpp ----
// #include "hero.h"
Hero::Hero(std::string name, int power) : name{name}, power{power} {}

std::string Hero::describe() const { return name + " is a hero"; }

int Hero::getPower() const { return power; }

bool Hero::operator<(const Hero& other) const { return power < other.power; }

// No Hero:: and no friend here: these are non-members.
Hero operator+(const Hero& lhs, const Hero& rhs) {
    return Hero{lhs.name + " and " + rhs.name, lhs.power + rhs.power};
}

Hero operator*(int factor, const Hero& hero) { return Hero{hero.name, factor * hero.power}; }

std::ostream& operator<<(std::ostream& out, const Hero& hero) {
    out << hero.name << " (power " << hero.power << ")";
    return out;  // returning out is what allows std::cout << a << b
}

bool operator>(const Hero& lhs, const Hero& rhs) { return lhs.getPower() > rhs.getPower(); }

int main19() {
    Hero captain{"Captain Const", 70};
    Hero lass{"Lambda Lass", 90};

    Hero weaker = std::min(captain, lass);  // std::min uses our operator<
    std::cout << weaker << '\n';            // Captain Const (power 70)
    std::cout << captain.describe() << '\n';

    std::cout << (lass > captain) << '\n';  // 1 (true)

    Hero team = captain + lass;
    std::cout << team << '\n';  // Captain Const and Lambda Lass (power 160)

    Hero buffed = 2 * captain;    // the left side is an int ---> only a non-member can do this
    std::cout << buffed << '\n';  // Captain Const (power 140)

    std::set<Hero> heroes{lass, captain};  // a set needs operator< too
    std::cout << *heroes.begin() << '\n';  // Captain Const (power 70)
    return 0;
}

// =====================================================================================
// 27-29) The static keyword
// =====================================================================================
// A namespace keeps this Hero apart from the Hero class in sections 21-26.
namespace statics {

// ---- file: hero.h ----
class Hero {
   private:
    std::string name;
    int power;
    static int heroCount;  // one copy, shared by every Hero

   public:
    Hero(std::string name, int power);
    int getPower() const;
    static int getHeroCount();  // write static only on the declaration
};

// ---- file: hero.cpp ----
int Hero::heroCount = 0;  // define it once, outside the class

Hero::Hero(std::string name, int power) : name{name}, power{power} {
    heroCount++;  // regular member functions can still use static members
}

int Hero::getPower() const { return power; }

int Hero::getHeroCount() {
    // return power;      // ERROR: whose power? a static function has no this
    return heroCount;
}

}  // namespace statics

int rescueCount() {
    static int count = 0;  // initialized once, keeps its value between calls
    return ++count;
}

static int visibleOnlyInThisFile() { return 42; }  // static at file scope

int main20() {
    std::cout << statics::Hero::getHeroCount() << '\n';  // 0, called on the class, no object
    statics::Hero captain{"Captain Const", 70};
    statics::Hero lass{"Lambda Lass", 90};
    std::cout << statics::Hero::getHeroCount() << '\n';         // 2
    std::cout << captain.getPower() + lass.getPower() << '\n';  // 160

    rescueCount();
    rescueCount();
    std::cout << rescueCount() << '\n';  // 3

    std::cout << visibleOnlyInThisFile() << '\n';  // 42
    return 0;
}

// =====================================================================================
// 30) Special member functions: when is each one called?
// =====================================================================================
// Moving comes in its own lecture, so this Widget only has the first four SMFs.
class Widget {
   public:
    Widget();                            // default constructor
    ~Widget();                           // destructor
    Widget(const Widget& w);             // copy constructor
    Widget& operator=(const Widget& w);  // copy assignment operator
};

Widget::Widget() { std::cout << "default constructor\n"; }

Widget::~Widget() { std::cout << "destructor\n"; }

Widget::Widget(const Widget&) { std::cout << "copy constructor\n"; }

Widget& Widget::operator=(const Widget&) {
    std::cout << "copy assignment operator\n";
    return *this;
}

int main21() {
    Widget widgetOne;              // default constructor
    Widget widgetTwo = widgetOne;  // copy constructor: widgetTwo is NEW
    Widget widgetThree;            // default constructor
    widgetThree = widgetOne;       // copy assignment: widgetThree already EXISTS
    return 0;
}  // three destructors run here

// =====================================================================================
// 31) and 32) Member initializer lists and deep copies: Vector
// =====================================================================================
// ---- file: Vector.h ----
template <typename T>
class Vector {
   public:
    Vector();
    ~Vector();                                     // Rule of Three: a custom destructor...
    Vector(const Vector<T>& other);                // ...needs a copy constructor...
    Vector<T>& operator=(const Vector<T>& other);  // ...and a copy assignment operator.

    size_t size() const;
    const T& at(size_t index) const;
    T& at(size_t index);
    void push_back(const T& elem);

   private:
    size_t _size;
    size_t _capacity;
    T* _data;
};

// ---- file: Vector.cpp ----
// Section 31: the members are constructed directly with the intended values.
template <typename T>
Vector<T>::Vector() : _size(0), _capacity(4), _data(new T[_capacity]) {}

template <typename T>
Vector<T>::~Vector() {
    delete[] _data;
}

// Section 32: a deep copy, with its own array.
template <typename T>
Vector<T>::Vector(const Vector<T>& other)
    : _size(other._size), _capacity(other._capacity), _data(new T[other._capacity]) {
    for (size_t i = 0; i < _size; ++i) {
        _data[i] = other._data[i];
    }
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
    if (this == &other) {  // v = v; must not delete its own data
        return *this;
    }
    delete[] _data;  // unlike the copy constructor, this object already has an array
    _size = other._size;
    _capacity = other._capacity;
    _data = new T[_capacity];
    for (size_t i = 0; i < _size; ++i) {
        _data[i] = other._data[i];
    }
    return *this;
}

template <typename T>
size_t Vector<T>::size() const {
    return _size;
}

template <typename T>
const T& Vector<T>::at(size_t index) const {
    return _data[index];
}

template <typename T>
T& Vector<T>::at(size_t index) {
    return _data[index];
}

template <typename T>
void Vector<T>::push_back(const T& elem) {
    if (_size == _capacity) {  // full ---> move to a bigger array
        _capacity *= 2;
        T* bigger = new T[_capacity];
        for (size_t i = 0; i < _size; ++i) {
            bigger[i] = _data[i];
        }
        delete[] _data;
        _data = bigger;
    }
    _data[_size] = elem;
    _size++;
}

// Section 31: const and reference members can only be set in an initializer list.
class MyClass {
    const int _constant;
    int& _reference;

   public:
    MyClass(int value, int& ref) : _constant(value), _reference(ref) {}
    int sum() const { return _constant + _reference; }
};

int main22() {
    int value = 2;
    MyClass myClass{1, value};
    std::cout << myClass.sum() << '\n';  // 3
    return 0;
}

int main23() {
    Vector<int> original;
    original.push_back(1);
    original.push_back(2);

    Vector<int> copy = original;  // copy constructor
    copy.at(0) = 99;              // a deep copy ---> original does not change

    Vector<int> assigned;
    assigned = original;  // copy assignment operator
    assigned.at(1) = 77;

    std::cout << original.at(0) << " " << original.at(1) << '\n';  // 1 2
    std::cout << copy.at(0) << " " << copy.at(1) << '\n';          // 99 2
    std::cout << assigned.at(0) << " " << assigned.at(1) << '\n';  // 1 77
    std::cout << original.size() << '\n';                          // 2
    return 0;
}

// =====================================================================================
// 33) How do you prevent copies?
// =====================================================================================
class PasswordManager {
   public:
    PasswordManager() = default;
    ~PasswordManager() = default;
    PasswordManager(const PasswordManager& rhs) = delete;             // setting an SMF to delete
    PasswordManager& operator=(const PasswordManager& rhs) = delete;  // removes it

    int passwordCount() const { return 0; }
};

int main24() {
    PasswordManager manager;
    // PasswordManager copy = manager;   // ERROR: the copy constructor is deleted
    PasswordManager other;
    // other = manager;                  // ERROR: copy assignment is deleted
    std::cout << manager.passwordCount() + other.passwordCount() << '\n';  // 0
    return 0;
}

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 1) Function templates ---\n";
    main1();
    std::cout << "--- 2) Function template instantiation ---\n";
    main2();
    std::cout << "--- 3) Function templates with auto ---\n";
    main3();
    std::cout << "--- 4) Concepts: unclear requirements ---\n";
    main4();
    std::cout << "--- 5) Comparable concept ---\n";
    main5();
    std::cout << "--- 6) Printable concept ---\n";
    main6();
    std::cout << "--- 7) Variadic templates ---\n";
    main7();
    std::cout << "--- 8) commaSeparate ---\n";
    main8();
    std::cout << "--- 9) Template metaprogramming ---\n";
    main9();
    std::cout << "--- 10) constexpr and consteval ---\n";
    main10();
    std::cout << "--- 12) A templated find ---\n";
    main11();
    std::cout << "--- 13) find_if and predicates ---\n";
    main12();
    std::cout << "--- 15) Lambda functions ---\n";
    main13();
    std::cout << "--- 16) Captures ---\n";
    main14();
    std::cout << "--- 17) Functors ---\n";
    main15();
    std::cout << "--- 18) Algorithms ---\n";
    main16();
    std::cout << "--- 19) Ranges ---\n";
    main17();
    std::cout << "--- 20) Views ---\n";
    main18();
    std::cout << "--- 21-26) Operator overloading ---\n";
    main19();
    std::cout << "--- 27-29) The static keyword ---\n";
    main20();
    std::cout << "--- 30) Special member functions ---\n";
    main21();
    std::cout << "--- 31) Member initializer lists ---\n";
    main22();
    std::cout << "--- 32) Deep copy ---\n";
    main23();
    std::cout << "--- 33) Preventing copies ---\n";
    main24();
    return 0;
}