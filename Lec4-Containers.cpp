/*
Recap:
A stringstream is an istream and also ostream.

Space-Time ----> Task : Fetch a wrench in a disorganized storage ---> we need to search it through
everything. Task : Fetch a wrench in a organized storage     ----> it is easier to find it. How much
stuff? Space helps us to see and navigate. and we know that space is time. Disorganized ----> space
efficnet but slow to search such as vector. Organized ----> Space inefficnet but faster to search
such as map.


STL ---> Standard Template Library ---> How this different from standartd library std?
C++ standard library ---> streams, strings, math, ... and also  contains STL including (Containers,
Iterators, Functors, Algorithms) made by Alexander Stepanov

What does template means ---> class IntList{...}, class StringList{...}, class DoubleList{...} --->
can be template <typename T> class Vector<T>{....}

This lecture ---> Containers:

1) Sequential Containers ---> impliment data structures which can be accessed sequentilally. (They
contains sequences) such as vector, Deque, Array, List.


1.1) Vector ---> A resizable contigous array.

Zero-overhead principle --> 1) you do not pay for what you do not use 2) What you do use is just as
efficient as what you could reasonably write by hand.

Q: how elements stay inside the memory ---> we have size and capacity the capacity is 2**i and size
is the number of elements we push_back to vector.


(create empty vector) std::vector<int> vec; ---> (create with n copy of zero) std::vector<int>
vec(n); (create with n copy of value k) std::vector<int> vec(n,k). (add k into the end of a vector)
--> std::vector<int> v; ---> v.push_back(k); (clear vector) v.clear(); (check if v is empty)
if(v.empty()); Get the element at index i ---> int k = v.at(i); or int k = v[i]; (replace the
element at index i) ---> v.at(i) = k; v[i] = k;

Trace insert() --> consider we have a vector [10,20,30,40,50,60] --> size = 6 cap  = 8
we want to insert at index 0 value 90 ---> for insert all the values should shift one to right and
then put 90 in the index 0;


1.2) Deque ---> Double ended queue. (probounce: deck)
Like a vector with push_back and pop_back and also push_front and pop_front.

inside deque ---> we have a control block (pointer array)---> when we push_back value 10 to deque
---> we make a pointer in control block to a block and put 10 to the first location.--->
push_back(11) after that at the same pointer second posyion insert 11 ---> then if we push 12 with
push_front(12) --> it make another pointer in the contorl block that point to another block that the
last element is 12. push_back(13) ---> go to thr first pointer block and add 13 ---> push_back(14)
the same but after that push_back(15) ---> make a new pointer block becuase the size of each block
is 4 and all now completes.

How Deque evolve:
1) There is MAP (control block) ---> deque is dynamic array of pointer ---> these pointers do not
hold the data. they hold memory addrrsses of data block. this allows the deque to be non contigous.

2) O(1) head/tail growth ---> when we push_front or push_back ---> the deque checks that the current
edge block has room or not. if not it allocates a new fixed-size block and add its pointer to the
next available slot in the MAP. becuase we only add a pointer the other existing elements do not
move in the memory.

3) Re-centering growth: when we push to one side we eventually hit the egde of the contol block and
instead of shifting everything the deque allocates larger pointer array and copies pointer into the
middle. so we always have room to grow in both directions.

4) Random Access math : to find deque[i] the cpu does not iterate. it calculates block = (i +
offset) / size to find a pointer in the MAP, then cell = (i + offset) % size to find the item in
data block. it is two jump instead of a vectors one.

2) Associative Containers: Impliment sorted data structures that can be quickly search.

2.1) MAP ---> contains key-value pairs with unique keys. (in python we called them dictionary)--->
it sorts the pairs with the key.

A map is  acollection of pairs. we can loop over each pair. the pairs are sorted by key.

A std::map<k,v> is a collection of std::pair<const, k, v>
for (const auto& pair: mymap){
    auto key  = pair.first;
    auto value = pair.value;
}
    or
for (const auto& [key,value]: myMap){
}

Map storage ----> Red-Black Tree ---> Stores pairs in a binary search tree (BST) ---> specifically
it uses a Red-Black Tree gaurunteeing maximum  depth of 2log(n) this makes search O(log(n))

Map visualizer ---> we want to insert key=16 and value p ---> make a black node with 16('p') then
insert (18,'v') ---> it make a red node on the right with 18('r') ---> then insert (5,'e') add a red
node in the left of (16,'p') with (5,'e'). Then insert (19,'s')  make a new node on the right side
of (18,'r') with (19,'s')
Then if we insert (20,'t') then sorted happened ---> (19,'s') go to root of and (18,'r') got to the
left and (20,;t;) to right. left to right everything sorted.

Map auto-insertion ---> we can do
std::map>










*/

#include <iostream>
#include <sstream>
#include <string>

int main_recap() {
    std::stringstream ss;
    ss << 3.14f << ' ' << "Hello";  // use as ostream
    float pi;
    std::string hi;
    ss >> pi >> hi;  // use as istream.
    std::cout << pi << '\n' << hi << std::endl;
}

#include <vector>

int main_vec() {
    std::vector<int> vec{1, 2, 3, 4};
    vec.push_back(5);
    vec.push_back(6);
    vec[1] = 20;
    for (size_t i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << " ";  // be carefull with indices ---> [] operation does not check
                                     // but .at() check the index.
    }
}

int min_val_vec(std::vector<int>& vec) {
    int min_val = vec.at(0);
    for (size_t i = 1; i < vec.size(); i++) {
        if (vec.at(i) < min_val) {
            min_val = vec.at(i);
        }
        return min_val;
    }
}

// maintainig a list of the 10000 prices.

#include <deque>

void recivePrice(std::deque<double>& prices, double price) {
    prices.push_back(price);
    if (prices.size() > 1000) {
        prices.pop_back();
    }
}

#include <map>

void main_map() {
    std::map<int, std::string> adjs;
    adjs[106] = "awsome";
    adjs[103] = "mathy";
    adjs[107] = "deep";
    std::cout << adjs[106];  // How it is efficient? It sorts the pairs with the key.
}

int main_map1() {
    std::map<int, char> preston{{16, 'p'}, {18, 'r'}, {5, 'e'}, {19, 's'},
                                {20, 't'}, {15, 'o'}, {14, 'n'}};
    for (const auto& pair : preston) {
        std::cout << pair.first << ' ' << pair.second << std::endl;
    }
    return 0;
}