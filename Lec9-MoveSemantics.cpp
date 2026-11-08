/*
=====================================================================================
Lecture 9: Move Semantics
=====================================================================================

0) Recap: special member functions (SMFs)
   - SMFs handle the lifecycle of a class:
         Type::Type(const Type& other);              // copy constructor:  Type a = b;
         Type& Type::operator=(const Type& other);   // copy assignment:   a = b;
         Type::~Type();                              // destructor
   - The compiler creates these for us, unless we want to manage our own memory.
   - Copying data is expensive. Why does it matter? How can we avoid needlessly copying data?
   - Plan for today: lvalues and rvalues, rvalue references, the move constructor and move
     assignment, std::move, and the Rule of Zero / Three / Five.

1) Motivation: copying costs energy
   - What drains your phone battery?
         - connecting to wifi
         - powering the display
         - copying data
   - Yes: moving bytes around in memory is one of the most expensive things a chip does.
     Memory access costs far more energy than math.
   - Our example: the captain's camera. One 1080 x 1080 photo has 1,166,400 pixels.
     Copying it reads and writes every pixel: over 2 million memory accesses.

2) The Photo class
         class Photo {
         public:
             Photo(int width, int height);            // constructor
             Photo(const Photo& other);               // copy constructor
             Photo& operator=(const Photo& other);    // copy assignment
             ~Photo();                                // destructor
         private:
             int width;
             int height;
             int* data;                               // the pixels
         };
   - Photo p(500, 500);   creates a new photo and allocates its pixels.
   - Photo q = p;         copy constructor: a new photo with a COPY of p's pixels.
   - q = p;               copy assignment: frees q's old pixels and copies p's pixels.
   - Key idea: ONE TREASURE, ONE OWNER. A photo owns its pixels, and only its destructor may
     delete[] them. Two photos must never own the same pixels, otherwise both destructors
     would delete[] them!
   - Example: see Photo in section 2 of the code below.

3) Which SMFs run?
         Photo takePhoto();
         int main() {
             Photo selfie = takePhoto();     // (A)
             Photo retake(0, 0);
             retake = takePhoto();           // (B)
         }
   - (A): nothing extra! The compiler builds selfie in place (COPY ELISION).
   - (B): copy assignment, then the destructor of the temporary.
   - Example: see section 3 of the code below.

4) The problem: a wasted copy
   - retake = takePhoto();
     takePhoto() makes a temporary, copy assignment duplicates every pixel, then the temporary
     is destroyed. The temporary is about to be destroyed and nobody will look at it again.
     Instead of copying its pixels, retake could PLUNDER them: just take the pointer.
   - Step 1: take the pointer. No pixels are copied, retake points at the same treasure.
     But now two photos point to one treasure. Who owns it? When the temporary is destroyed,
     its destructor deletes the pixels that retake is still using!
   - Step 2: leave the old owner empty handed: set other.data = nullptr.
     (delete[] nullptr does nothing, so the temporary's destructor is now harmless.)
   - One treasure, one owner, and zero pixels copied. That is a MOVE.

5) Is it always safe to move?
         void post(Photo whoAmI) {
             Photo selfie = whoAmI;        // plunder whoAmI here?
             whoAmI.getPixel(21, 24);      // whoAmI is used again! (its data would be nullptr)
         }
   - Only plunder objects that nobody will use again:
         COPY persistent things, MOVE temporaries.
   - Think of buying a new computer:
         copy semantics: I still want to use my old computer.
         move semantics: I do not need my old computer any more.
   - Example: see section 5 of the code below.

6) lvalues and rvalues
   - How does the compiler know whether to move or copy? lvalues and rvalues generalize the
     idea of "temporariness".
   - LVALUE: has a name and an address. It lives until the end of its scope (persistent).
         Photo selfie = ...;       // selfie is an lvalue
   - RVALUE: a temporary value without a name. It lives until the end of the line.
     It is PLUNDERABLE.
         takePhoto();              // the returned Photo is an rvalue
         x + 1;  42;               // rvalues too

7) lvalue references and rvalue references
   - Passing by value makes an unnecessary copy:
         void upload(Photo pic);
         upload(selfie);           // copy!
   - An lvalue reference avoids the copy:
         void upload(Photo& pic);
         upload(selfie);           // no copy
         upload(takePhoto());      // ERROR: a temporary cannot bind to Photo&
   - An rvalue reference (&&) binds to temporaries:
         void upload(Photo&& pic); // pic is temporary, we can do anything we want with it
         upload(takePhoto());      // OK
   - lvalue reference Type& : persistent, the object must stay in a valid state after the
                              function.
     rvalue reference Type&&: temporary, we can steal (move) its resources. It may end up
                              empty, and that is OK.
   - Overload on & and && and the compiler sorts the loot for you:
         void upload(Photo& pic);    // lvalues land here
         void upload(Photo&& pic);   // rvalues land here
         upload(selfie);             // calls upload(Photo&)
         upload(takePhoto());        // calls upload(Photo&&)
   - Example: see section 7 of the code below.

8) Move constructor and move assignment
   - Two new special member functions:
         Type::Type(Type&& other);                   // move constructor
         Type& Type::operator=(Type&& other);        // move assignment
   - Copy constructor vs move constructor:
         Photo::Photo(const Photo& other)
             : width(other.width), height(other.height), data(new int[width * height]) {
             std::copy(other.data, other.data + width * height, data);
         }
         // new treasure for a new owner, and copy every pixel.

         Photo::Photo(Photo&& other)
             : width(other.width), height(other.height), data(other.data) {
             other.data = nullptr;
         }
         // take the treasure and leave other empty handed.
   - Move assignment: free our own pixels, take other's, leave other empty handed.
         Photo& Photo::operator=(Photo&& other) {
             if (this == &other) return *this;
             delete[] data;
             width = other.width;
             height = other.height;
             data = other.data;
             other.data = nullptr;
             return *this;
         }
   - Warning: Photo(const Photo&& other) is a mistake. A const object cannot be changed, so we
     could not set other.data = nullptr: we would be back to copying.
   - Mark the move operations noexcept: std::vector only moves its elements when it grows if
     moving cannot throw. Otherwise it copies them to be safe.
   - Example: see section 8 of the code below.

9) std::move: forcing move semantics
   - Usually we let the compiler decide between & and &&. What if we know that an lvalue will
     not be used again?
         void insert(std::vector<Photo>& album, const Photo& pic, int pos) {
             album.push_back(Photo(1, 1));
             for (int i = album.size() - 1; i > pos; i--) {
                 album[i] = album[i - 1];                 // copies every photo!
             }
             album[pos] = pic;
         }
   - album[i - 1] is an lvalue, but we will overwrite it right away. Plunder instead:
             album[i] = std::move(album[i - 1]);
   - std::move says "I am done with this one": it treats it as an rvalue.
   - std::move does NOT move anything! It is just a cast from an lvalue to an rvalue.
     The move constructor or move assignment does the plundering.
   - After a move the old owner is empty handed: only ASSIGN to it or DESTROY it.
   - Use std::move only when you are sure the object will not be used again.
   - Example: see section 9 of the code below.

10) Rule of Zero, Rule of Three and Rule of Five
   - RULE OF ZERO: if a class does not manage memory (or another external resource), write
     none of the SMFs. The ones the compiler generates are sufficient, and they move too.
   - RULE OF THREE: if a class manages an external resource, define the destructor, copy
     constructor and copy assignment. If you need one of them, you need all three.
     Otherwise the generated SMFs copy only the pointer, not the resource: two Photos would
     refer to the same pixels.
   - RULE OF FIVE: if you wrote those three, add both move operations too. They are not
     required, but without them every "move" silently becomes a slow copy.
         Destructor, Copy Constructor, Copy Assignment, Move Constructor, Move Assignment.
   - Most classes should follow the Rule of Zero: let members like std::vector and
     std::string own the resources.
   - Example: see section 10 of the code below.

11) Summary
   - Copying is expensive: memory access costs energy and time.
   - Every resource should have ONE OWNER. Copying makes a new one, moving hands it over.
   - lvalues have a name and persist, rvalues are temporaries that can be plundered.
   - && lets functions and SMFs spot temporaries and plunder them.
   - The move constructor and move assignment steal the resource and leave the source empty.
   - std::move casts an lvalue to an rvalue. It moves nothing by itself.
   - After a move, only assign to or destroy the moved-from object.
   - Rule of Zero for most classes, Rule of Five when you manage a resource yourself.
*/
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// =====================================================================================
// 2) The Photo class (with all five SMFs, see section 8)
// =====================================================================================
// Every SMF prints its name and updates a counter, so we can see which one runs.
// The photos are small here so the examples run fast; the idea is the same for 1080 x 1080.
class Photo {
   public:
    Photo(int width, int height);                // constructor
    Photo(const Photo& other);                   // copy constructor
    Photo& operator=(const Photo& other);        // copy assignment
    Photo(Photo&& other) noexcept;               // move constructor
    Photo& operator=(Photo&& other) noexcept;    // move assignment
    ~Photo();                                    // destructor

    int getPixel(int x, int y) const { return data[y * width + x]; }
    void setPixel(int x, int y, int value) { data[y * width + x] = value; }
    bool isEmpty() const { return data == nullptr; }
    int pixelCount() const { return width * height; }

    static bool verbose;   // print every SMF call?
    static int copies;     // number of copy constructions/assignments
    static int moves;      // number of move constructions/assignments
    static void resetCounters() { copies = moves = 0; }

   private:
    int width;
    int height;
    int* data;  // the pixels: the treasure this photo owns
};

bool Photo::verbose = true;
int Photo::copies = 0;
int Photo::moves = 0;

Photo::Photo(int width, int height)
    : width(width), height(height), data(new int[width * height]{}) {
    if (verbose) std::cout << "  constructor (" << width << "x" << height << ")\n";
}

// New treasure for a new owner, and copy every pixel.
Photo::Photo(const Photo& other)
    : width(other.width), height(other.height), data(new int[width * height]) {
    std::copy(other.data, other.data + width * height, data);
    ++copies;
    if (verbose) std::cout << "  copy constructor (" << width * height << " pixels copied)\n";
}

Photo& Photo::operator=(const Photo& other) {
    if (this == &other) return *this;  // self assignment: p = p;
    delete[] data;                     // clean up old pixels
    width = other.width;
    height = other.height;
    data = new int[width * height];
    std::copy(other.data, other.data + width * height, data);
    ++copies;
    if (verbose) std::cout << "  copy assignment (" << width * height << " pixels copied)\n";
    return *this;
}

// Take the treasure and leave other empty handed.
Photo::Photo(Photo&& other) noexcept
    : width(other.width), height(other.height), data(other.data) {
    other.data = nullptr;
    other.width = other.height = 0;
    ++moves;
    if (verbose) std::cout << "  move constructor (0 pixels copied)\n";
}

Photo& Photo::operator=(Photo&& other) noexcept {
    if (this == &other) return *this;
    delete[] data;  // free our own old pixels
    width = other.width;
    height = other.height;
    data = other.data;      // step 1: take the pointer
    other.data = nullptr;   // step 2: leave the old owner empty handed
    other.width = other.height = 0;
    ++moves;
    if (verbose) std::cout << "  move assignment (0 pixels copied)\n";
    return *this;
}

// Free the pixels when the photo is destroyed. delete[] nullptr does nothing.
Photo::~Photo() {
    if (verbose) std::cout << "  destructor" << (data ? "" : " (empty handed)") << '\n';
    delete[] data;
}

// Returns a temporary (an rvalue).
Photo takePhoto() { return Photo(4, 3); }

int main2() {
    Photo p(5, 5);   // constructor
    Photo q = p;     // copy constructor
    q = p;           // copy assignment
    return 0;
}                    // destructor, destructor

// =====================================================================================
// 3) Which SMFs run?
// =====================================================================================
int main3() {
    std::cout << " (A) Photo selfie = takePhoto();\n";
    Photo selfie = takePhoto();  // copy elision: built in place, no copy and no move
    std::cout << " Photo retake(0, 0);\n";
    Photo retake(0, 0);
    std::cout << " (B) retake = takePhoto();\n";
    retake = takePhoto();  // without a move assignment: copy assignment + destructor
                           // with our move assignment: move assignment + destructor
    std::cout << " end of scope\n";
    return 0;
}

// =====================================================================================
// 5) Is it always safe to move?
// =====================================================================================
void post(Photo whoAmI) {
    Photo selfie = whoAmI;  // whoAmI is an lvalue: COPY, because we use it again below
    // Photo selfie = std::move(whoAmI);   // plunder? then whoAmI would be empty handed...
    std::cout << "  pixel: " << whoAmI.getPixel(2, 1) << '\n';  // ...and this would crash
}

int main5() {
    Photo pic(4, 3);
    pic.setPixel(2, 1, 7);
    post(pic);  // pic is copied into whoAmI, then whoAmI is copied into selfie
    return 0;
}

// =====================================================================================
// 7) lvalue references and rvalue references
// =====================================================================================
void upload(Photo& /*pic*/) { std::cout << "  upload(Photo&): lvalue, must leave it valid\n"; }
void upload(Photo&& /*pic*/) { std::cout << "  upload(Photo&&): rvalue, can plunder it\n"; }

int main7() {
    Photo selfie = takePhoto();
    upload(selfie);       // lvalue: calls upload(Photo&)
    upload(takePhoto());  // rvalue: calls upload(Photo&&)
    return 0;
}

// =====================================================================================
// 8) Move constructor and move assignment
// =====================================================================================
int main8() {
    Photo a(4, 3);
    a.setPixel(0, 0, 42);

    Photo b = std::move(a);  // move constructor: b takes a's pixels
    std::cout << "  b pixel: " << b.getPixel(0, 0) << ", a empty? " << std::boolalpha
              << a.isEmpty() << '\n';

    Photo c(1, 1);
    c = std::move(b);        // move assignment: c frees its own pixels and takes b's
    std::cout << "  c pixel: " << c.getPixel(0, 0) << ", b empty? " << b.isEmpty() << '\n';
    return 0;
}

// =====================================================================================
// 9) std::move: forcing move semantics
// =====================================================================================
void insertByCopy(std::vector<Photo>& album, const Photo& pic, int pos) {
    album.push_back(Photo(1, 1));
    for (int i = static_cast<int>(album.size()) - 1; i > pos; i--) {
        album[i] = album[i - 1];  // copy every photo
    }
    album[pos] = pic;
}

void insertByMove(std::vector<Photo>& album, const Photo& pic, int pos) {
    album.push_back(Photo(1, 1));
    for (int i = static_cast<int>(album.size()) - 1; i > pos; i--) {
        album[i] = std::move(album[i - 1]);  // plunder instead: album[i - 1] is reassigned next
    }
    album[pos] = pic;  // pic is const and belongs to the caller: copy it
}

int main9() {
    Photo::verbose = false;
    Photo pic(100, 100);

    std::vector<Photo> album1;
    album1.reserve(20);  // no reallocation, so we only count the shifting
    for (int i = 0; i < 10; ++i) album1.emplace_back(100, 100);
    std::vector<Photo> album2;
    album2.reserve(20);
    for (int i = 0; i < 10; ++i) album2.emplace_back(100, 100);

    Photo::resetCounters();
    insertByCopy(album1, pic, 0);
    std::cout << "  insertByCopy: " << Photo::copies << " copies, " << Photo::moves
              << " moves\n";

    Photo::resetCounters();
    insertByMove(album2, pic, 0);
    std::cout << "  insertByMove: " << Photo::copies << " copies, " << Photo::moves
              << " moves\n";

    // std::move moves nothing by itself: it is only a cast.
    Photo keep(2, 2);
    Photo&& ref = std::move(keep);  // no SMF runs here
    std::cout << "  after std::move(keep), keep empty? " << std::boolalpha << keep.isEmpty()
              << '\n';
    (void)ref;

    return 0;
}

// =====================================================================================
// 10) Rule of Zero, Rule of Three and Rule of Five
// =====================================================================================
// Rule of Zero: no treasure of its own to guard, so we write none of the SMFs.
// std::string and std::vector manage their memory, and the generated SMFs copy AND move.
struct Album {
    std::string name;
    std::vector<Photo> photos;
};

// Rule of Three only: copy SMFs and destructor, but no move operations.
// Declaring a copy constructor stops the compiler from generating a move constructor,
// so std::move falls back to the copy constructor: every move becomes a slow copy.
class OldPhoto {
   public:
    explicit OldPhoto(int size) : size(size), data(new int[size]{}) {}
    OldPhoto(const OldPhoto& other) : size(other.size), data(new int[other.size]) {
        std::copy(other.data, other.data + size, data);
        std::cout << "  OldPhoto copy constructor (" << size << " pixels copied)\n";
    }
    OldPhoto& operator=(const OldPhoto& other) {
        if (this == &other) return *this;
        delete[] data;
        size = other.size;
        data = new int[size];
        std::copy(other.data, other.data + size, data);
        return *this;
    }
    ~OldPhoto() { delete[] data; }

   private:
    int size;
    int* data;
};

int main10() {
    Photo::verbose = false;
    Album trip{"Summer", {}};
    trip.photos.emplace_back(4, 3);
    trip.photos.emplace_back(4, 3);

    Photo::resetCounters();
    Album copy = trip;              // generated copy constructor copies every Photo
    std::cout << "  Album copy: " << Photo::copies << " photo copies\n";

    Photo::resetCounters();
    Album moved = std::move(trip);  // generated move constructor just takes the vector
    std::cout << "  Album move: " << Photo::copies << " photo copies, " << Photo::moves
              << " photo moves (the vector itself was moved)\n";

    OldPhoto old(1000);
    OldPhoto other = std::move(old);  // no move constructor: this copies!
    return 0;
}

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 2) The Photo class ---\n";
    main2();
    std::cout << "--- 3) Which SMFs run? ---\n";
    main3();
    std::cout << "--- 5) Is it always safe to move? ---\n";
    main5();
    std::cout << "--- 7) lvalue and rvalue references ---\n";
    main7();
    std::cout << "--- 8) Move constructor and move assignment ---\n";
    main8();
    std::cout << "--- 9) std::move ---\n";
    main9();
    std::cout << "--- 10) Rule of Zero, Three and Five ---\n";
    main10();
    return 0;
}
