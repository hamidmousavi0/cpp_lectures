/*
=====================================================================================
Lecture 4: Containers
=====================================================================================

0) Recap
   - A std::stringstream is BOTH an istream and an ostream: you can write into it with <<
     and read back out of it with >>.

1) Why containers? Space vs. time
   - Task: fetch a wrench from a DISORGANIZED storage room ---> you must search through
     everything. From an ORGANIZED one (labelled shelves) ---> you find it quickly.
   - Organizing costs extra space (shelves, labels), but saves search time.
       Disorganized ---> space efficient, slow to search      e.g. std::vector
       Organized    ---> uses more space, fast to search      e.g. std::map
   - Choosing a container = choosing a trade-off between space and time.

2) STL vs. the C++ standard library
   - C++ standard library: everything in namespace std ---> streams, strings, math, ...
   - STL (Standard Template Library, designed by Alexander Stepanov) is the PART of the
     standard library made of: Containers, Iterators, Functors, Algorithms.
   - "Template" = write the code once for ANY type T, instead of writing
     class IntList{...}, class StringList{...}, class DoubleList{...} separately:
         template <typename T> class vector { ... };
         ---> std::vector<int>, std::vector<std::string>, ...
   - This lecture: Containers.

3) Sequence containers: store elements in a sequence, accessed by position
   (std::vector, std::deque, std::array, std::list).

   3.1) std::vector ---> a resizable, CONTIGUOUS array (elements sit next to each other).
     - Zero-overhead principle:
         1. You do not pay for what you do not use.
         2. What you do use is as efficient as what you could reasonably write by hand.
     - size vs. capacity:
         size     = number of elements actually stored
         capacity = number of elements that fit in the memory already reserved
       When size reaches capacity, push_back allocates a BIGGER block (typically 2x in
       g++/clang, 1.5x in MSVC), moves all elements there and frees the old block.
       So push_back is fast on average ("amortized O(1)").
     - Syntax:
         std::vector<int> v;          // empty vector
         std::vector<int> v(n);       // n copies of 0
         std::vector<int> v(n, k);    // n copies of k
         v.push_back(k);              // add k at the end
         v.clear();                   // remove all elements
         if (v.empty()) ...           // is it empty?
         int k = v.at(i);  v.at(i) = k;   // get / replace element i (bounds CHECKED: throws)
         int k = v[i];     v[i] = k;      // get / replace element i (NOT checked: UB if wrong)
     - Trace insert(): v = [10,20,30,40,50,60] (size 6, capacity 8), insert 90 at index 0:
         every element must shift one step right ---> [  ,10,20,30,40,50,60]
         then 90 is written at index 0          ---> [90,10,20,30,40,50,60]
       Inserting/erasing at the front or middle of a vector is SLOW: O(n).

   3.2) std::deque ---> "double-ended queue" (pronounced "deck").
     - Like a vector, but fast at BOTH ends: push_back / pop_back AND push_front / pop_front.
     - Inside a deque:
       1. The MAP (control block): a dynamic array of POINTERS. The pointers do not hold the
          data; they point to fixed-size DATA BLOCKS. So a deque is NOT contiguous.
       2. O(1) growth at head/tail: push_front/push_back check whether the edge block has
          room. If not, a new block is allocated and its pointer is added to the map.
          Existing elements never move in memory.
       3. Re-centering: when the map itself runs out of slots on one side, a larger map is
          allocated and the POINTERS (not the elements) are copied into its middle, so there
          is room to grow in both directions again.
       4. Random access math: deque[i] does not iterate. With block size B:
              block = (i + offset) / B   ---> which pointer in the map
              cell  = (i + offset) % B   ---> which slot in that block
          Two memory jumps instead of a vector's one ---> fast, but a bit slower than vector.
     - Example (block size 4): push_back(10), push_back(11) fill block A; push_front(12)
       needs a NEW block B placed BEFORE A (12 goes in B's last slot); push_back(13),
       push_back(14) fill A; push_back(15) needs a new block C after A.

4) Associative containers: keep elements SORTED by key so they can be searched quickly
   (std::map, std::set).

   4.1) std::map<K, V> ---> key-value pairs with UNIQUE keys (Python's dictionary),
        sorted by key.
     - A map is a collection of std::pair<const K, V> (the key is const: changing it would
       break the sorting). Looping visits the pairs in KEY order:
           for (const auto& pair : myMap) { pair.first; pair.second; }   // key, value
           for (const auto& [key, value] : myMap) { ... }   // structured binding (Lecture 3)
     - Storage: a Red-Black Tree, a self-balancing binary search tree (BST). Its depth is
       at most 2*log2(n+1), so search / insert / erase are all O(log n).
     - Map visualizer (inserting into a red-black tree):
           insert (16,'p') ---> becomes the BLACK root
           insert (18,'r') ---> RED node, right of 16
           insert ( 5,'e') ---> RED node, left of 16
           insert (19,'s') ---> RED node, right of 18. Two reds in a row (18, 19) is not
                                allowed ---> recolor: 18 and 5 become BLACK
           insert (20,'t') ---> RED node, right of 19. Two reds again ---> ROTATE:
                                19 moves up, 18 becomes its left child, 20 its right child
       Reading the tree left to right is always sorted: 5 16 18 19 20.
     - Auto-insertion: m[k] on a MISSING key inserts k with a default value (0, "", ...):
           std::map<std::string, int> fav_num;
           fav_num["preston"] = 2;
           std::cout << fav_num["emily"];   // prints 0 AND silently adds "emily" to the map!
       Use .at(k) (throws if missing) or .contains(k) / .find(k) when you only want to read.
     - Syntax:
         std::map<char, int> m;               // empty map
         m.insert({k, v});                    // add key k with value v (no-op if k exists)
         m.erase(k);                          // remove key k
         if (m.count(k)) / if (m.contains(k)) // is k in the map? (contains: C++20)
         if (m.empty())                       // is it empty?
         int i = m[k];  m[k] = i;             // read / overwrite the value of key k

   4.2) std::set<K> ---> a map without values: just UNIQUE, SORTED keys in a tree.
     - Syntax:
         std::set<char> s;                    // empty set
         s.insert(k);                         // add k (no-op if already there)
         s.erase(k);                          // remove k
         if (s.count(k)) / if (s.contains(k)) // is k in the set?
         if (s.empty())                       // is it empty?

   4.3) The caveat: keys must be COMPARABLE
     - How does a map sort? Look at its definition in <map>:
           template <class Key, class T, class Compare = std::less<Key>, ...> class map;
       By default it compares keys with "<" (std::less). So the key type MUST support "<".
     - int, double, std::string ... have "<". A struct you write, or std::ifstream, does not
       ---> std::map<MyStruct, int> does not compile until you give it an operator< or a
       custom comparator.

5) Unordered associative containers: std::unordered_map, std::unordered_set
   - Usually faster drop-in replacements for map/set when you do NOT need sorted order:
         std::map<int, std::string> courses{...};
         --->  std::unordered_map<int, std::string> courses{...};
   - Defined in <unordered_map>:
         template <class Key, class T, class Hash = std::hash<Key>,
                   class KeyEqual = std::equal_to<Key>, ...> class unordered_map;
     So the key needs a HASH function and "==" (instead of "<").
   - Hash function: "scrambles" a key into a std::size_t (64 bits on most machines).
     Small changes in the input should produce large changes in the output.
   - Stored in a HASH TABLE: an array of BUCKETS. bucket = hash(key) % bucket_count.
       load factor = size / bucket_count
     Example: 4 buckets, load factor 0. Insert (10,'A') ---> 0.25, (11,'B') ---> 0.5, ...
     When the load factor would exceed max_load_factor (default 1.0) the table REHASHES:
     it allocates roughly twice as many buckets and redistributes every element.
     (g++ picks a prime bucket count, e.g. 13 ---> 29 ---> 59; see main12.)

6) Summary
       container            | i-th element | search      | insert         | erase
       ---------------------+--------------+-------------+----------------+---------------
       std::vector          | very fast    | slow        | slow (fast end)| slow (fast end)
       std::deque           | fast         | slow        | fast at ends   | fast at ends
       std::set / std::map  | slow         | fast        | fast           | fast
       std::unordered_set   | N/A          | very fast   | very fast      | very fast
       std::unordered_map   | N/A          | very fast   | very fast      | very fast
   (map/set: O(log n). unordered: O(1) on average. vector/deque search: O(n).)
   Rule of thumb: default to std::vector; switch only when you need something it is bad at.

7) Exercises
   - Supersponsors (std::map + std::set): which investors sponsor two or more F1 teams?
   - Fastest lap (std::vector): the smallest lap time in a vector.
*/
#include <cstddef>
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// =====================================================================================
// 0) Recap: a stringstream is both an ostream and an istream
// =====================================================================================
int main_recap() {
    std::stringstream ss;
    ss << 3.14f << ' ' << "Hello";  // use it as an ostream
    float pi;
    std::string hi;
    ss >> pi >> hi;  // use it as an istream
    std::cout << pi << '\n' << hi << '\n';
    return 0;
}

// =====================================================================================
// 3.1) std::vector
// =====================================================================================
int main1() {
    std::vector<int> vec{1, 2, 3, 4};
    vec.push_back(5);
    vec.push_back(6);
    vec[1] = 20;
    for (std::size_t i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << " ";  // careful with indices: [] does NOT check them,
                                     // .at() does (and throws std::out_of_range)
    }
    std::cout << '\n';
    return 0;
}

// size vs. capacity: watch the capacity jump when the reserved memory runs out.
int main2() {
    std::vector<int> vec;
    std::size_t lastCapacity = vec.capacity();
    std::cout << "size 0, capacity " << lastCapacity << '\n';
    for (int i = 0; i < 20; i++) {
        vec.push_back(i);
        if (vec.capacity() != lastCapacity) {  // a reallocation happened
            lastCapacity = vec.capacity();
            std::cout << "size " << vec.size() << ", capacity " << lastCapacity << '\n';
        }
    }
    return 0;
}

// Smallest value in a vector.
// const& ---> no copy of the vector, and the function promises not to change it (Lecture 3).
int min_val_vec(const std::vector<int>& vec) {
    int min_val = vec.at(0);  // throws std::out_of_range if vec is empty
    for (std::size_t i = 1; i < vec.size(); i++) {
        if (vec[i] < min_val) {
            min_val = vec[i];  // remember the smaller value and KEEP looking
        }
    }
    return min_val;  // only after checking EVERY element
}

int main3() {
    std::vector<int> vec{5, 3, 8, 1, 9};
    std::cout << "min of {5, 3, 8, 1, 9} is " << min_val_vec(vec) << '\n';  // 1

    // insert() at the front: every element shifts one step right ---> O(n)
    std::vector<int> v{10, 20, 30, 40, 50, 60};
    v.insert(v.begin(), 90);  // v.begin() = position of index 0 (iterators: next lecture)
    for (int x : v) {
        std::cout << x << " ";  // 90 10 20 30 40 50 60
    }
    std::cout << '\n';
    return 0;
}

// =====================================================================================
// 3.2) std::deque
// =====================================================================================
int main4() {
    std::deque<int> dq;
    dq.push_back(10);
    dq.push_back(11);
    dq.push_front(12);  // fast at the front too (a vector cannot do this cheaply)
    dq.push_back(13);
    dq.push_back(14);
    dq.push_back(15);
    for (int x : dq) {
        std::cout << x << " ";  // 12 10 11 13 14 15
    }
    std::cout << "| dq[2] = " << dq[2] << '\n';  // random access still works: 11
    return 0;
}

// Keeping only the most recent prices: new ones go in the back, the oldest leaves the front.
constexpr std::size_t kMaxPrices = 1000;

void receivePrice(std::deque<double>& prices, double price) {
    prices.push_back(price);
    if (prices.size() > kMaxPrices) {
        prices.pop_front();  // drop the OLDEST price (pop_back would drop the newest one!)
    }
}

int main5() {
    std::deque<double> prices;
    for (int i = 1; i <= 1005; i++) {
        receivePrice(prices, i * 1.5);
    }
    std::cout << "stored " << prices.size() << " prices, oldest " << prices.front() << ", newest "
              << prices.back() << '\n';  // 1000 prices, oldest 9, newest 1507.5
    return 0;
}

// =====================================================================================
// 4.1) std::map
// =====================================================================================
int main6() {
    std::map<int, std::string> adjs;  // std::string, not char*: string literals are const
    adjs[106] = "awesome";
    adjs[103] = "mathy";
    adjs[107] = "deep";
    std::cout << "adjs[106] = " << adjs[106] << '\n';  // O(log n) search in a sorted tree
    return 0;
}

int main7() {
    // Inserted in "random" order, but the map keeps the pairs sorted by key.
    std::map<int, char> preston{{16, 'p'}, {18, 'r'}, {5, 'e'}, {19, 's'},
                                {20, 't'}, {15, 'o'}, {14, 'n'}};
    for (const auto& pair : preston) {
        std::cout << pair.first << ' ' << pair.second << '\n';
    }
    // Same loop with structured binding. We inserted the letters as "preston", but they
    // come out sorted by KEY, not in insertion order.
    for (const auto& [key, value] : preston) {
        std::cout << value;  // enoprst
    }
    std::cout << '\n';
    return 0;
}

// Auto-insertion with [].
int main8() {
    std::map<std::string, int> fav_num;
    fav_num["preston"] = 2;
    std::cout << "Preston's is " << fav_num["preston"] << " and Emily's is " << fav_num["emily"]
              << '\n';  // "emily" was never set ---> prints 0
    std::cout << "map size is now " << fav_num.size() << '\n';  // 2: reading "emily" ADDED it!

    // Safer ways to read:
    std::cout << "contains \"bob\"? " << fav_num.contains("bob") << '\n';  // 0, nothing added
    // fav_num.at("bob");  // throws std::out_of_range instead of inserting
    return 0;
}

// Map syntax in action.
int main9() {
    std::map<char, int> m;
    m.insert({'a', 1});
    m.insert({'b', 2});
    m.insert({'a', 99});  // 'a' already exists ---> insert does nothing
    m['c'] = 3;           // [] inserts or overwrites
    m.erase('b');
    std::cout << "m['a'] = " << m['a'] << ", contains 'b'? " << m.contains('b')
              << ", size = " << m.size() << '\n';  // 1, 0, 2
    return 0;
}

// =====================================================================================
// 4.2) std::set
// =====================================================================================
// Find the agents that work for MORE than one department.
// departments: department name ---> set of agent names in it.
std::set<std::string> findDoubleAgents(
    const std::map<std::string, std::set<std::string>>& departments) {
    std::set<std::string> seen, doubleAgents;
    for (const auto& [department, agents] : departments) {
        for (const auto& agent : agents) {
            if (seen.contains(agent)) {
                doubleAgents.insert(agent);  // already seen in another department
            } else {
                seen.insert(agent);
            }
        }
    }
    return doubleAgents;
}

int main10() {
    std::map<std::string, std::set<std::string>> departments{
        {"CIA", {"Alice", "Bob", "Carol"}},
        {"MI6", {"Bond", "Carol"}},
        {"KGB", {"Bob", "Ivan"}},
    };
    std::cout << "Double agents:";
    for (const auto& agent : findDoubleAgents(departments)) {
        std::cout << ' ' << agent;  // Bob Carol (a set is sorted too)
    }
    std::cout << '\n';
    return 0;
}

// =====================================================================================
// 4.3) Keys must be comparable
// =====================================================================================
struct Point {
    int x;
    int y;
};

// Tell std::map how to order Points: by x, then by y.
struct ComparePoint {
    bool operator()(const Point& a, const Point& b) const {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

int main11() {
    // std::map<Point, std::string> places;  // ERROR: Point has no operator<
    std::map<Point, std::string, ComparePoint> places;  // OK: custom comparator
    places[{2, 1}] = "library";
    places[{1, 5}] = "cafe";
    places[{1, 2}] = "lab";
    for (const auto& [point, name] : places) {
        std::cout << '(' << point.x << ',' << point.y << ") " << name << '\n';
    }
    return 0;
}

// =====================================================================================
// 5) std::unordered_map
// =====================================================================================
int main12() {
    // Same interface as std::map: a drop-in replacement.
    std::unordered_map<int, std::string> courses{
        {106, "Programming Abstractions"},
        {107, "Computer Organization"},
    };
    std::cout << "106 is " << courses.at(106) << '\n';

    // Watch the hash table grow: the bucket count jumps when the load factor passes 1.0.
    std::unordered_map<int, char> table;
    std::size_t lastBuckets = table.bucket_count();
    for (int key = 0; key < 30; key++) {
        table[key] = 'A';
        if (table.bucket_count() != lastBuckets) {  // a rehash happened
            lastBuckets = table.bucket_count();
            std::cout << "size " << table.size() << " ---> rehash to " << lastBuckets
                      << " buckets, load factor " << table.load_factor() << '\n';
        }
    }
    // Looping visits the pairs in NO particular order (unlike std::map).
    return 0;
}

// =====================================================================================
// 7) Exercises
// =====================================================================================
// Exercise 1: return every investor who sponsors two or more teams.
// Same idea as findDoubleAgents: remember who we have seen in a set.
std::set<std::string> findSupersponsors(
    const std::map<std::string, std::set<std::string>>& sponsors) {
    std::set<std::string> seen;
    std::set<std::string> supersponsors;
    for (const auto& [team, investors] : sponsors) {
        for (const auto& investor : investors) {
            if (seen.contains(investor)) {
                supersponsors.insert(investor);  // already sponsors another team
            } else {
                seen.insert(investor);
            }
        }
    }
    return supersponsors;
}

int main13() {
    // A few of each team's 2026 partners.
    std::map<std::string, std::set<std::string>> sponsors{
        {"Alpine", {"BWT", "Microsoft", "Castore", "New Era", "Alpinestars", "Pirelli"}},
        {"Aston Martin", {"Aramco", "Cognizant", "Puma", "Honda", "Pirelli"}},
        {"Audi", {"Revolut", "Adidas", "Pirelli"}},
        {"Cadillac", {"TWG AI", "Alpinestars", "Tommy Hilfiger", "Pirelli"}},
        {"Ferrari", {"HP", "Shell", "Puma", "Richard Mille", "Pirelli"}},
        {"Haas",
         {"Toyota Gazoo Racing", "MoneyGram", "Castore", "New Era", "Alpinestars", "Pirelli"}},
        {"McLaren", {"Mastercard", "Puma", "Richard Mille", "Alpinestars", "Pirelli"}},
        {"Mercedes", {"Petronas", "Microsoft", "Adidas", "IWC Schaffhausen", "Pirelli"}},
        {"Racing Bulls", {"Visa", "Cash App", "Ford", "Pirelli"}},
        {"Red Bull", {"Oracle", "Visa", "Ford", "Castore", "New Era", "Sparco", "Pirelli"}},
        {"Williams", {"Atlassian", "Gulf", "New Era", "Sparco", "Claude", "Pirelli"}},
    };

    std::set<std::string> supersponsors = findSupersponsors(sponsors);
    std::cout << supersponsors.size() << " supersponsors:";
    std::string separator = " ";
    for (const std::string& investor : supersponsors) {
        std::cout << separator << investor;  // sorted, because it is a std::set
        separator = ", ";
    }
    std::cout << '\n';
    return 0;
}

// Exercise 2: return the shortest lap time in laps (same pattern as min_val_vec).
double fastestLap(const std::vector<double>& laps) {
    double shortest = laps.at(0);  // throws std::out_of_range if laps is empty
    for (double lap : laps) {
        if (lap < shortest) {
            shortest = lap;
        }
    }
    return shortest;
}

int main14() {
    std::vector<double> monza{82.347, 81.902, 81.455, 81.761, 82.010};
    std::vector<double> monaco{74.318, 73.903, 74.122, 72.954};
    std::cout << "Monza fastest lap: " << fastestLap(monza) << "s\n";    // 81.455
    std::cout << "Monaco fastest lap: " << fastestLap(monaco) << "s\n";  // 72.954
    return 0;
}

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 0) Recap ---\n";
    main_recap();
    std::cout << "--- 3.1) vector ---\n";
    main1();
    main2();
    main3();
    std::cout << "--- 3.2) deque ---\n";
    main4();
    main5();
    std::cout << "--- 4.1) map ---\n";
    main6();
    main7();
    main8();
    main9();
    std::cout << "--- 4.2) set ---\n";
    main10();
    std::cout << "--- 4.3) comparable keys ---\n";
    main11();
    std::cout << "--- 5) unordered_map ---\n";
    main12();
    std::cout << "--- 7) Exercises ---\n";
    main13();
    main14();
    return 0;
}
