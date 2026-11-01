/*
=====================================================================================
Lecture 5: Iterators and Pointers
=====================================================================================

0) Recap
   - Containers: std::vector, std::deque (a map of pointers to blocks), std::set, std::map,
     std::unordered_set and std::unordered_map (hashing and load factor).
   - The STL has four parts:
       Containers ---> how do we store groups of things?
       Iterators  ---> how do we traverse containers?
       Functors   ---> how can we represent functions as objects?
       Algorithms ---> how do we transform and modify containers in a generic way?
   - This lecture: Iterators (and pointers).

1) Iterator basics: what even is an iterator?
   - Every loop has the same shape:
         for (init; condition; increment) {
             const auto& elem = ...;   // grab the current element
             // do something with elem
         }
     But how do we "grab the current element" for a set or a map, which have no index?
     We need something that tracks WHERE we are in a container, sort of like an index
     ---> an ITERATOR.
   - Think of a claw machine:
       The claw (iterator) can      : 1. grab a toy  2. move forward  3. check if we are done
       The machine (container) can  : 1. tell us where to start  2. tell us when to stop
     Containers and iterators work together to allow iteration.
   - Container interface:
       c.begin() ---> iterator to the FIRST element (assuming c is not empty)
       c.end()   ---> PAST-THE-END iterator: one position after the last element.
                      It never points to an element. If c is empty, c.begin() == c.end().
   - Iterator interface:
       auto it = c.begin();     // initialize (copy)
       ++it;                    // move forward
       auto& elem = *it;        // dereference: get the element (undefined if it == c.end())
       if (it == c.end()) ...   // equality: are we done?
   - Walking through a container:
       auto it = c.begin() ---> ++it ---> ++it ---> auto elem = *it ---> ++it ---> it == c.end()

2) Why ++it instead of it++?
   - Prefix  ++it : increment it and return a REFERENCE to the same object.
                    Iterator& operator++();
   - Postfix it++ : increment it and return a COPY of the old value.
                    Iterator operator++(int);
   - Some iterators are objects that are expensive to copy, and it++ makes that copy.
     ---> ++it is sometimes faster than it++, and never slower.

3) Iterator categories: iterators are organized by their properties
   - Not all iterators are made equal. ALL iterators provide these four operations:
         auto it = c.begin();   ++it;   *it;   it == c.end();
     but most provide more:
         --it;          // move backward
         *it = elem;    // modify
         it += n;       // random access (jump n steps)
         it1 < it2;     // is it1 before it2?
   - Categories, from most to least powerful (each one can do everything below it):
         Random access ---> Bidirectional ---> Forward ---> Input / Output

   3.1) Input iterators: the most basic kind. READ elements: auto elem = *it;
        If the element is a struct, access its members with ->: it->member == (*it).member
   3.2) Output iterators: WRITE elements: *it = elem;
   3.3) Forward iterators: input iterators that allow MULTIPLE PASSES.
        Multi-pass guarantee: copies of an iterator stay valid and independent:
            std::vector<int> v{1, 2, 3};
            auto a = v.begin();
            auto b = a;        // copy
            ++a;               // b is unaffected
            std::cout << *b;   // 1 (still valid)
        Q: Which data structure might NOT want a multi-pass iterator?
        A: Streams. Reading from std::cin CONSUMES the data; you cannot read it twice.
   3.4) Bidirectional iterators: move forward AND backward (--it).  e.g. std::map, std::set
            auto it = m.end();
            --it;              // now points to the LAST element
   3.5) Random access iterators: jump forward/backward quickly (it += n, it[n]).
                                                                e.g. std::vector, std::deque

   Why does it matter?
   - Some algorithms require a certain category: std::sort needs random access, so it
     works on a std::vector but NOT on a std::set or std::map.
   - Why have multiple categories? They give one uniform abstraction over all containers,
     but the way a container is implemented affects how you can move through it.
     Skipping 5 steps is easy in a sequence container (vector, deque: just compute an
     address) but slow in a tree (set, map: follow 5 links). C++ avoids giving you a slow
     operation by design ---> that is why map::iterator has no "it + 5".

4) Pointers and memory: what is a pointer? what is memory?
   - An iterator points to a CONTAINER element. A pointer points to ANY object.
   - Every variable lives somewhere in memory. All the places something could live form
     the ADDRESS SPACE of the program:
         | OS (shared) | Stack (local vars) | Heap (new / make_unique) | Globals | Text (code) |
   - Memory is BYTE-ADDRESSABLE: each byte (8 bits) has a number, from 0x0 up to 2^64 - 1
     on a 64-bit system.
   - The address of an object is the address of its LOWEST byte. An int uses 32 bits =
     4 bytes, so if int x = 106; lives at 0x10, it occupies 0x10, 0x11, 0x12, 0x13.
   - A POINTER is just a number: the address of a variable.
         int x = 106;
         int* px = &x;      // & = "address of" ---> px holds e.g. 0x50527c
         *px                // * = dereference  ---> 106
   - We can point to any kind of object:
         StanfordID id{...};          StanfordID* p = &id;          auto name = p->name;
         std::vector<int> v;          std::vector<int>* pv = &v;
   - A std::vector is CONTIGUOUS (one single chunk of memory), so a pointer to v[0] can
     move through it just like a random access iterator.
   - Iterators have an interface SIMILAR to pointers: *, ->, ++, ==. In fact a pointer IS a
     random access iterator for arrays.
*/
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// =====================================================================================
// 1) Iterator basics: a range-based for loop is really an iterator loop
// =====================================================================================
int main1() {
    // With an index: only works for containers that have one (vector, deque).
    std::vector<int> v{1, 2, 3, 4};
    for (std::size_t i = 0; i < v.size(); i++) {
        const auto& elem = v.at(i);
        std::cout << elem << ' ';
    }
    std::cout << '\n';

    // With an iterator: works for EVERY container, including std::set.
    std::set<int> s{1, 2, 3, 4};
    for (auto it = s.begin(); it != s.end(); ++it) {
        const auto& elem = *it;  // const int& (elements of a set can not be modified)
        std::cout << elem << ' ';
    }
    std::cout << '\n';

    // When we write this ...
    for (auto elem : s) {
        std::cout << elem << ' ';
    }
    std::cout << '\n';

    // ... the compiler actually generates this:
    auto b = s.begin();
    auto e = s.end();
    for (auto it = b; it != e; ++it) {
        auto elem = *it;
        std::cout << elem << ' ';
    }
    std::cout << '\n';
    return 0;
}

// What type is *it for a map? A std::pair<const Key, Value>.
int main2() {
    std::map<int, int> m1{{1, 2}, {3, 4}, {5, 6}};
    auto it1 = m1.begin();
    auto elem1 = *it1;  // {1, 2}

    // The same thing with the types written out (this is what auto saves us from):
    std::map<int, int>::iterator it2 = m1.begin();
    std::pair<const int, int> elem2 = *it2;

    std::cout << elem1.first << ' ' << elem1.second << " | " << elem2.first << ' '
              << elem2.second << '\n';  // 1 2 | 1 2
    return 0;
}

// Copies of an iterator are independent (multi-pass guarantee).
int main3() {
    std::map<int, int> m{{1, 2}, {3, 4}, {5, 6}};
    auto a = m.begin();  // a ---> {1, 2}
    ++a;                 // a ---> {3, 4}
    auto b = a;          // b ---> {3, 4}  (a copy)
    ++a;                 // a ---> {5, 6}, b is unaffected
    auto c = ++a;        // a and c ---> m.end() (do NOT dereference them!)
    std::cout << "b ---> " << b->first << ", c == m.end()? " << (c == m.end()) << '\n';
    return 0;
}

// =====================================================================================
// 3.1) Input iterators: a stream can only be read ONCE (single pass)
// =====================================================================================
// Type some ints and finish with Ctrl+D (Linux/macOS) or Ctrl+Z then Enter (Windows).
// Not called from main because it waits for keyboard input.
int main_input() {
    std::istream_iterator<int> it(std::cin);  // points at the first int
    std::istream_iterator<int> end;           // default-constructed = end of the stream
    while (it != end) {
        std::cout << *it << ' ';  // read the current value
        ++it;                     // CONSUMES it: there is no going back
    }
    std::cout << '\n';
    return 0;
}

// Operator ->: access a member of the element the iterator points to.
struct Bibble {
    int Zarf;
};

int main4() {
    std::vector<Bibble> v{{1}, {2}, {3}};
    auto it = v.begin();
    int m = (*it).Zarf;  // dereference, then access the member
    int m1 = it->Zarf;   // the same thing, shorter
    std::cout << m << ' ' << m1 << '\n';  // 1 1
    return 0;
}

// =====================================================================================
// 3.4) Bidirectional iterators: std::map and std::set can move backward
// =====================================================================================
int main5() {
    std::map<std::string, int> m{{"apple", 1}, {"banana", 2}};
    auto it = m.end();
    --it;  // step back from past-the-end ---> the LAST element
    const auto& elem = *it;
    std::cout << elem.first << ' ' << elem.second << '\n';  // banana 2
    return 0;
}

// =====================================================================================
// 3.5) Random access iterators: std::vector and std::deque can jump
// =====================================================================================
int main6() {
    std::vector<int> v{3, 1, 2, 5, 4};
    auto it = v.begin();
    auto it2 = it + 4;          // jump 4 steps forward ---> 4
    auto it3 = it2 - 2;         // jump 2 steps back    ---> 2
    auto& third = *(it + 2);    // 2
    auto& third2 = it[2];       // 2: same as *(it + 2)
    // auto bad = it + 10;      // be careful: going past end() is undefined behavior!
    std::cout << *it2 << ' ' << *it3 << ' ' << third << ' ' << third2 << '\n';

    std::sort(v.begin(), v.end());  // OK: std::sort needs random access iterators
    for (int x : v) {
        std::cout << x << ' ';  // 1 2 3 4 5
    }
    std::cout << '\n';

    std::set<int> s{1, 5, 6, 7};
    // std::sort(s.begin(), s.end());  // ERROR: set iterators are only bidirectional
    //                                 // (and a set is already sorted anyway)
    return 0;
}

// =====================================================================================
// 4) Pointers
// =====================================================================================
int main7() {
    int x = 106;
    int* px = &x;              // px is a pointer; & is the address-of operator
    std::cout << x << '\n';    // 106
    std::cout << *px << '\n';  // 106: * dereferences the pointer
    std::cout << px << '\n';   // an address, e.g. 0x50527c
    return 0;
}

// A pointer into a vector behaves like a random access iterator.
int main8() {
    std::vector<int> v{1, 2, 3, 4, 5};
    int* arr = &v[0];  // initialization: points to v[0]
    arr += 1;          // random access: v[1]
    ++arr;             // move forward:  v[2]
    arr += 2;          // random access: v[4]
    if (arr == &v[4]) {  // pointer comparison
        std::cout << "arr points to v[4] = " << *arr << '\n';  // 5
    }
    return 0;
}

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 1) Iterator basics ---\n";
    main1();
    main2();
    main3();
    std::cout << "--- 3.1) Input iterators and -> ---\n";
    main4();
    std::cout << "--- 3.4) Bidirectional ---\n";
    main5();
    std::cout << "--- 3.5) Random access ---\n";
    main6();
    std::cout << "--- 4) Pointers ---\n";
    main7();
    main8();
    return 0;
}
