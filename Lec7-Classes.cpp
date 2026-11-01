/*
=====================================================================================
Lecture 7: Classes, Class Templates and Const Correctness
=====================================================================================

0) Recap: iterator categories
   - Random access ---> Bidirectional ---> Forward ---> Input / Output
       Random access : can SKIP elements (it += n)
       Bidirectional : can move in BOTH directions (--it)
       Forward       : MULTI-pass
       Input / Output: SINGLE pass
   - Q1: What can you do with a bidirectional iterator that you cannot do with an input
         iterator?
     A : Move backward (--it), and pass over the elements more than once.
   - Q2: What can you do with a random access iterator that you cannot do with a
         bidirectional iterator?
     A : Jump n steps at once (it += n, it[n]) and compare positions (it1 < it2).
   - Q3: What type of iterator is std::set<int>::iterator?
     A : Bidirectional (a set is a tree, so jumping 5 steps would be slow).
   - Plan for today: Classes, Inheritance, Virtuality, Class Templates, Const correctness.

1) Why classes?
   - C++ was originally called "C with classes".
   - std::deque: inside it is scary underlying data (a control block: an array of pointers
     to memory blocks). Users never see that, they only see a clean interface:
         scary underlying data
         -------- Abstraction boundary (ENCAPSULATION) --------
         clean interface: push_back(), erase(), ...
   - Object Oriented Programming (OOP): data is grouped into OBJECTS, and objects contain
     MEMBERS: fields (data) and functions. Example:
         Button
           - xsize, ysize, xposition, yposition, label_text, interested_listeners
           + draw(), press(), register_callback(), unregister_callback()
   - A class is a USER DEFINED TYPE:
         class Cookie { ... };     // the type
         Cookie c1;  Cookie c2;    // INSTANCES (objects) of the type

2) Struct vs class
   - We have seen this before: aren't structs user defined types? Yes, but a struct has no
     access control: everything is public. A class has ACCESS CONTROL: we can define
     public and private sections.
   - A struct is like an elevator with glass walls: you can see (and touch) everything
     inside. A class is a normal elevator: you only see the buttons (the public interface).
         class CoolClass {
         private:
             // only the class itself can use these
         public:
             // everyone can use these
         };
   - (Technically the ONLY difference: struct members are public by default, class
     members are private by default.)

3) One class, two files
                    Header file (.h)                     Source file (.cpp)
     Purpose    :   defines the INTERFACE                IMPLEMENTS the class functions
     Contains   :   function prototypes, class           function implementations,
                    declarations, type definitions,      executable code
                    macros, constants
     Access     :   shared across source files           compiled into an object file
                    (via #include)
     Example    :   void someFunction();                 void someFunction() { ... }

4) One class, two kinds of members
   1) Data members     (sometimes called "fields" or "member variables")
   2) Member functions (sometimes called "methods")
        - Special ones: constructor, destructor, etc.
   - Private fields often get "getters" (read them) and "setters" (change them).
   - Outside the class, a member function is named ClassName::functionName.
     The ClassName:: part says "this function belongs to that class".
   - this is a pointer to the current object. this->capacity means "the FIELD capacity",
     which we need when a parameter has the same name.
   - Example: see Elevator in section 4 of the code below.

5) Constructor and destructor
   - Constructor: runs when an object is CREATED, sets up the fields.
   - Destructor (~ClassName): runs when an object is DESTROYED (e.g. goes out of scope).
     If the constructor allocated memory with new, the destructor must free it with delete.
         Elevator::Elevator(int numFloors) { this->floors = new int[numFloors]; }
         Elevator::~Elevator()             { delete[] this->floors; }
   - We can also initialize the fields with an INITIALIZER LIST (before the body runs):
         Elevator::Elevator(int numFloors) : floors{new int[numFloors]} {}
   - Example: see FloorElevator in section 5 of the code below.

6) Inheritance
   - Elevator (capacity, floors) ---> IkeaElevator, ScaryElevator, WaterElevator
   - Vehicle (+ goTo(destination))
       ---> Elevator : - capacity, - floors
       ---> Car      : - seats,    - mileage
              ---> InNOutMobile : + serveFries()
   - DYNAMIC POLYMORPHISM: the type is determined at RUNTIME.
   - EXTENSIBILITY: we can add more to a class without changing the original.
   - Example: Square inherits from Rectangle
         // shapes.h
         class Rectangle {
         private:
             int width;
             int height;
         public:
             Rectangle(int w, int h);
             int getArea();
         };
         class Square : public Rectangle {
         public:
             Square(int len);
         };

         // shapes.cpp
         #include "shapes.h"
         Square::Square(int len) : Rectangle(len, len) {}
   - The constructor of Square calls the constructor of Rectangle in its initializer list:
     a square is a rectangle with width == height == len.
   - Why Square::Square and not just Square?
       Inside the class body, the compiler already knows we are in Square, so the
       definition can be written directly:
           class Square : public Rectangle {
           public:
               Square(int len) : Rectangle(len, len) {}   // defined INSIDE the class
           };
       In shapes.cpp we are OUTSIDE the class, so we must say which class the function
       belongs to: Square::Square(int len) (class name :: function name). Without Square::
       the compiler does not know this is a constructor of Square and gives an error.
   - Why "class Square : public Rectangle" and not "class Square : Rectangle"?
       For a class the default inheritance is PRIVATE, so getArea() would be hidden from
       users of Square. Also the members of a class are private by default, so
       Square(int len) must be under public:, otherwise nobody can create a Square.
   - Example: see Rectangle and Square in section 6 of the code below.

7) Inheritance access specifiers
   - The word after the colon controls what the child can show of its parent:
         class Square : public Rectangle { ... };
   - private (the DEFAULT for a class) ---> all members of the parent become private in
                                          the child (users of Square cannot call getArea()).
   - public                            ---> public members of the parent stay public.
   - Access inside a class has one more level:
       public    ---> everyone can see and modify.
       protected ---> the class and its SUBCLASSES can see and modify.
       private   ---> only the class itself.

8) Virtuality: virtual functions and abstract classes
   - A VIRTUAL function can be replaced (OVERRIDDEN) by a subclass. When we call it through
     a pointer or reference to the parent, the CHILD's version runs (dynamic polymorphism).
   - A PURE virtual function (= 0) has NO implementation in the parent.
   - A class with a pure virtual function is ABSTRACT: it cannot be instantiated.
         Vehicle v{10};      // ERROR: Vehicle is abstract
         Bus b{10};          // OK: Bus implements getCapacity()
         // vehicles.h
         class Vehicle {
         protected:
             int seats;                          // Bus can use it
         public:
             Vehicle(int seats);
             virtual int getCapacity() = 0;      // pure virtual: no implementation
         };
         class Bus : public Vehicle {
         public:
             Bus(int seats);
             int getCapacity() override;         // must match a virtual function of the parent
         };

         // vehicles.cpp
         #include "vehicles.h"
         Bus::Bus(int seats) : Vehicle{seats} {}
         int Bus::getCapacity() { return seats * 2; }  // NO "override" outside the class
   - override tells the compiler: "this MUST match a virtual function of a parent class".
     If the name or the parameters are wrong, we get a compile error instead of a silent bug.
     (Like virtual, it is written only in the class body, not in the .cpp definition.)
   - Example: see Vehicle and Bus in section 8 of the code below.

9) The diamond problem
         // persons.h
         struct Person   { std::string name; };
         struct Student  : Person {};
         struct Employee : Person {};
         struct TA       : Student, Employee {};      // MULTIPLE inheritance

                     Person
                    /      \
               Student    Employee
                    \      /
                       TA
   - A TA contains TWO Persons (one through Student, one through Employee):
         TA me;
         me.name = "Preston";   // ERROR: ambiguous, which name?
   - Solution #1: say which Person you mean.
         me.Student::name  = "Preston";
         me.Employee::name = "Mr. Preston";
   - Solution #2: inherit Person VIRTUALLY, so there is only ONE Person inside a TA.
         struct Student  : virtual Person {};
         struct Employee : virtual Person {};
         struct TA       : Student, Employee {};
         TA me;
         me.name = "Preston";   // OK
   - Example: see section 9 of the code below.

10) Exercise: find the nearest elevator that still has capacity
   - Example: see findBestElevator in section 10 of the code below.

11) What are templates?
   - Q: I need a way to store a list of integers!      A: I am on it.
         class IntVector {
             // Code to store a list of integers...
         };
   - Q: Now I need a way to store a list of doubles!   A: Right on it.
         class DoubleVector {
             // Code to store a list of doubles...
         };
   - Q: Could you give me a list of strings?           A: This is getting old...
   - The logic is the same every time, only the type changes.
     What if we could keep the logic, but change the type?
         class IntVector {};
         class DoubleVector {}; ---> template <typename T>
         class StringVector {};     class Vector { ... };

         Vector<int> v1;   Vector<double> v2;   Vector<std::string> v3;
   - So std::vector<T> is a template. Q: How does this T work?

12) Template classes
   - Key idea: templates AUTOMATE CODE GENERATION.
   - A template is like a FACTORY: you give it a type, it generates a new class.
         int         ---> [ template Vector ] ---> Vector<int>
         std::string ---> [ template Vector ] ---> Vector<std::string>
   - Templates vs types:
         template <typename T>
         class Vector {};           // this is a TEMPLATE, it is not a type

         Vector<std::string>        // this is a TYPE (a template INSTANTIATION)
   - Input of the factory: a type (T). The factory: the template. Output: a new type.
   - TEMPLATE INSTANTIATION: code for a specific type is generated on demand, when you
     use it.
   - Vector<double> and Vector<int> are two instantiations of the same template, but they
     are two completely DIFFERENT types.
   - Example: see Vector and main8 in section 12 of the code below.

13) A few template quirks (strange behaviours)
   13.1) Must copy the template<...> syntax in the .cpp file
         When implementing a template, you might try something like this:
             // Vector.h
             template <typename T>
             class Vector {
             public:
                 T& at(size_t index);
             };

             // Vector.cpp
             T& Vector::at(size_t index) { ... }             // ERROR: what is T?
         Compiler: "I do not know what T is!" So we add the template line:
             template <typename T>
             T& Vector::at(size_t index) { ... }             // ERROR: still wrong
         Q: Does anyone still see a problem with this?
         A: Vector is not a type, but Vector<T> is:
             template <typename T>
             T& Vector<T>::at(size_t index) { ... }          // OK
   13.2) The .h file must include the .cpp file at the bottom
         For NON-template classes, the .cpp file includes the .h file:
             // StrVector.h
             class StrVector {
             public:
                 std::string& at(size_t index);
             };

             // StrVector.cpp
             #include "StrVector.h"
             std::string& StrVector::at(size_t index) { ... }
         For TEMPLATE classes, the .h file includes the .cpp file:
             // Vector.h
             template <typename T>
             class Vector {
             public:
                 T& at(size_t index);
             };
             #include "Vector.cpp"

             // Vector.cpp
             template <typename T>
             T& Vector<T>::at(size_t index) { ... }
         That is pretty weird. Why? It is due to the way template code generation is
         implemented in the compiler (and linker): to generate Vector<int>, the compiler
         must see the whole template, not only the declarations. Do not worry too much
         about the why.
   13.3) typename is the same as class
             template <typename T>
             class Vector {};
             template <class T>                              // exactly the same
             class Vector {};

             template <typename T, typename V> struct Pair;  // same as:
             template <class T, class V> struct Pair;
         (template <class T> also works for non-class types like int.)

14) Const correctness: the problem
   - Here is our Vector, without any const:
         template <class T>
         class Vector {
         public:
             size_t size();
             bool empty();
             T& operator[](size_t index);
             T& at(size_t index);
             void push_back(const T& elem);
         };
   - Let's use it:
         void printVec(const Vector<int>& v) {
             for (size_t i = 0; i < v.size(); i++) {   // ERROR
                 std::cout << v.at(i) << " ";          // ERROR
             }
             std::cout << '\n';
         }
   - Compiler: ERROR, "passing const Vector<int> as this argument discards qualifiers".
     Huh? There is a method called size. What is the problem?
   - By passing v as const, we promise NOT to modify v.
   - The compiler cannot be sure that methods like size and at do not modify v.
     Remember, member functions can access member variables.

15) Const methods
   - A CONST METHOD says: "Dear compiler, I promise not to modify this object inside of
     this method. Please hold me accountable."
         template <class T>
         class Vector {
         public:
             size_t size() const;
             bool empty() const;
             ...
         };
   - Make sure to also add const to the implementation in the .cpp file:
         template <class T>
         size_t Vector<T>::size() const { return logical_size; }
   - Example: see size and empty in section 12 of the code below.

16) Const overloading
   - First try:
         T& at(size_t index) const;
     Problem #1: CONST users can modify the element. We return a non-const reference, so
     v.at(0) = 42 compiles, but v is const so this should not be allowed.
     Solution? Return a const reference:
         const T& at(size_t index) const;
     Problem #2: NON-CONST users cannot modify the element anymore. v.at(0) = 42 is now
     an ERROR for every Vector, so we cannot update elements.
   - Solution: CONST OVERLOADING. Define two versions of the method: one that gets called
     for const objects, another that gets called for non-const objects.
         // Vector.h
         const T& at(size_t index) const;
         T& at(size_t index);

         // Vector.cpp
         template <class T>
         const T& Vector<T>::at(size_t index) const { return elems[index]; }

         template <class T>
         T& Vector<T>::at(size_t index) { return elems[index]; }
   - Example: see at and main9 in section 16 of the code below.

17) What is this?
   - In a normal method, this is a pointer to the object:
         void Point::setX(int x) { this->x = x; }             // Point* this
   - In a const method, this is a pointer to a CONST object, so we cannot change it:
         int Point::getX() const { return this->x; }          // const Point* this
   - So in a const method of Vector<T>, this has type const Vector<T>*.
   - Example: see Point in section 17 of the code below.

18) The const interface
   - Objects marked as const can only make use of the CONST INTERFACE.
   - The const interface = the functions of the object that are const.
         Vector<T>                                const Vector<T>
           public:                                  public:
             size_t size() const;                     size_t size() const;
             bool empty() const;                      bool empty() const;
             const T& at(size_t) const;               const T& at(size_t) const;
             T& at(size_t);
             void push_back(const T& elem);
           private:                                 private:
             size_t logical_size;                     const size_t logical_size;
             T* elems;                                T* const elems;
   - Careful: elems becomes T* const (the POINTER is const), not const T*. The elements
     themselves are not const. This is why the compiler did not stop Problem #1 in
     section 16: it is our job to return const T&.

19) A slight aside: const_cast
   - CASTING: the process of converting one type to another.
   - const_cast allows us to cast away the const-ness of a variable.
         const_cast<target_type>(expression)

         int value = 5;
         const int& ref = value;
         int& mutableRef = const_cast<int&>(ref);   // now mutableRef can be changed
   - When should we use const_cast? (Almost) never. If you need a mutable value, just do
     not add const. Changing an object that was really declared const is undefined behavior.
   - Example: see section 19 of the code below.

20) A C++ party trick: the mutable keyword
   - const_cast makes an ENTIRE object mutable. Is there anything more fine-grained?
     Yes: a mutable field can be changed even inside a const object.
         struct MutableStruct {
             int dontTouchThis;
             mutable double iCanChange;
         };

         const MutableStruct cm{1, 2.0};
         cm.dontTouchThis = 42;   // ERROR
         cm.iCanChange = 3.14;    // OK
   - Example: see section 20 of the code below.

21) Summary
   - We can make our own TYPES with classes.
   - We ENCAPSULATE variables with access specifiers (public, protected, private).
   - INHERITANCE lets us reuse and extend functionality.
   - VIRTUAL gives us control over inheritance (overriding, abstract classes, diamonds).
   - Templates AUTOMATE CODE GENERATION: a template is a factory, not a type.
   - Template quirks: copy template<...> and write Vector<T>:: in the .cpp file, include
     the .cpp file at the bottom of the .h file, typename is the same as class.
   - Mark methods that do not modify the object as CONST.
   - Const objects can only use the CONST INTERFACE.
   - Use CONST OVERLOADING when const and non-const users need different return types.
*/
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// =====================================================================================
// 2) Struct: no access control, everything is public
// =====================================================================================
struct StanfordID {
    // These are the fields
    std::string name;
    std::string sunet;
    int idNumber;
};

int main1() {
    StanfordID id;
    id.name = "Hamid";
    id.sunet = "Mousavi";
    id.idNumber = 100;  // anyone can change any field
    std::cout << id.name << " " << id.sunet << " " << id.idNumber << '\n';
    return 0;
}

// =====================================================================================
// 4) A class with private fields, a constructor, getters and setters
// =====================================================================================
// ---- file: elevator.h ----
class Elevator {
   private:
    // private member fields
    int floor;
    int capacity;

   public:
    // Constructor
    Elevator(int capacity);
    // Public member function
    void request(int floor);
    // Private members often have "getters"...
    int getCapacity();
    // ...and "setters"
    void setCapacity(int capacity);
};

// ---- file: elevator.cpp ----
// #include "elevator.h"
Elevator::Elevator(int capacity) {
    if (capacity < 0) {
        std::cerr << "Invalid capacity\n";
        capacity = 1;
    }
    this->capacity = capacity;  // this->capacity is the field, capacity is the parameter
    floor = 0;
}

void Elevator::request(int floor) { this->floor = floor; }

int Elevator::getCapacity() { return capacity; }

void Elevator::setCapacity(int capacity) { this->capacity = capacity; }

// ---- file: main.cpp ----
// #include "elevator.h"
int main2() {
    Elevator e{1};
    e.request(1);
    // e.capacity = 5;     // ERROR: capacity is private
    e.setCapacity(5);                                      // OK: go through the public interface
    std::cout << "Capacity: " << e.getCapacity() << '\n';  // 5
    return 0;
}

// =====================================================================================
// 5) Constructor and destructor: new in the constructor, delete in the destructor
// =====================================================================================
// ---- file: elevator.h ----
class FloorElevator {
   private:
    int* floors;  // pointer to an array allocated on the heap

   public:
    FloorElevator(int numFloors);
    ~FloorElevator();
};

// ---- file: elevator.cpp ----
FloorElevator::FloorElevator(int numFloors) : floors{new int[numFloors]} {  // initializer list
    // same as: this->floors = new int[numFloors];
    std::cout << "Constructor: allocated " << numFloors << " floors\n";
}

FloorElevator::~FloorElevator() {
    delete[] this->floors;
    std::cout << "Destructor: freed the floors\n";
}

int main3() {
    FloorElevator e{10};  // constructor runs here
    std::cout << "Using the elevator...\n";
    return 0;
}  // e goes out of scope ---> destructor runs here

// =====================================================================================
// 6) Inheritance: a Square is a Rectangle
// =====================================================================================
// ---- file: shapes.h ----
class Rectangle {
   private:
    int width;
    int height;

   public:
    Rectangle(int w, int h);
    int getArea();
};

class Square : public Rectangle {
   public:
    Square(int len);
};

// ---- file: shapes.cpp ----
// #include "shapes.h"
Rectangle::Rectangle(int w, int h) : width{w}, height{h} {}

int Rectangle::getArea() { return width * height; }

// Outside the class we need Square:: to say which class this constructor belongs to.
Square::Square(int len) : Rectangle(len, len) {}

int main4() {
    Rectangle r{3, 4};
    Square s{5};
    std::cout << "Rectangle area: " << r.getArea() << '\n';  // 12
    std::cout << "Square area: " << s.getArea() << '\n';     // 25, getArea() is inherited
    return 0;
}

// =====================================================================================
// 8) Virtuality: an abstract Vehicle and a Bus that overrides getCapacity()
// =====================================================================================
// ---- file: vehicles.h ----
class Vehicle {
   protected:
    int seats;  // protected: Bus can use it, users cannot

   public:
    Vehicle(int seats);
    virtual ~Vehicle() = default;
    virtual int getCapacity() = 0;  // pure virtual ---> Vehicle is abstract
};

class Bus : public Vehicle {
   public:
    Bus(int seats);
    int getCapacity() override;  // must match a virtual function of the parent
};

// ---- file: vehicles.cpp ----
// #include "vehicles.h"
Vehicle::Vehicle(int seats) : seats{seats} {}

Bus::Bus(int seats) : Vehicle{seats} {}

int Bus::getCapacity() { return seats * 2; }  // no "override" outside the class

int main5() {
    // Vehicle v{10};   // ERROR: cannot instantiate an abstract class
    Bus b{10};
    Vehicle& v = b;  // a reference to the PARENT type...
    std::cout << "Capacity: " << v.getCapacity() << '\n';  // ...still calls Bus::getCapacity: 20
    return 0;
}

// =====================================================================================
// 9) The diamond problem
// =====================================================================================
// ---- file: persons.h ----
struct Person {
    std::string name;
};

// Solution #1: normal inheritance ---> a TA contains TWO Persons
struct Student : Person {};
struct Employee : Person {};
struct TA : Student, Employee {};

// Solution #2: virtual inheritance ---> a VirtualTA contains ONE Person
struct VirtualStudent : virtual Person {};
struct VirtualEmployee : virtual Person {};
struct VirtualTA : VirtualStudent, VirtualEmployee {};

int main6() {
    TA me;
    // me.name = "Preston";          // ERROR: ambiguous, which Person's name?
    me.Student::name = "Preston";    // Solution #1: say which one
    me.Employee::name = "Mr. Preston";
    std::cout << me.Student::name << " / " << me.Employee::name << '\n';

    VirtualTA you;
    you.name = "Preston";  // Solution #2: only one name, no ambiguity
    std::cout << you.name << '\n';
    return 0;
}

// =====================================================================================
// 10) Exercise: find the nearest elevator that has capacity
// =====================================================================================
// A namespace keeps these names apart from the Elevator class in section 4.
namespace dispatch {

// ---- file: vehicle.h ----
class Vehicle {
   protected:
    int passengers;
    int capacity;

   public:
    Vehicle(int passengers, int capacity);
    // True if at least one more person fits.
    bool hasCapacity();
};

// ---- file: elevator.h ----
class Elevator : public Vehicle {
   private:
    int currentFloor;

   public:
    Elevator(int currentFloor, int passengers, int capacity);
    // How many floors away `floor` is.
    int distanceTo(int floor);
    int getFloor();
};

// ---- file: vehicle.cpp / elevator.cpp ----
Vehicle::Vehicle(int passengers, int capacity) : passengers{passengers}, capacity{capacity} {}

bool Vehicle::hasCapacity() { return passengers < capacity; }

Elevator::Elevator(int currentFloor, int passengers, int capacity)
    : Vehicle(passengers, capacity), currentFloor{currentFloor} {}

int Elevator::distanceTo(int floor) { return std::abs(currentFloor - floor); }

int Elevator::getFloor() { return currentFloor; }

// ---- file: dispatch.cpp ----
// #include "dispatch.h"
Elevator findBestElevator(std::vector<Elevator>& elevators, int floor) {
    Elevator best = elevators[0];
    bool found = false;  // has best been set to an elevator with capacity yet?
    for (Elevator elevator : elevators) {
        // Skip it if it has no capacity.
        if (!elevator.hasCapacity()) {
            continue;
        }

        // Keep it if nothing is found yet, or if it is closer to floor than best is.
        if (!found || elevator.distanceTo(floor) < best.distanceTo(floor)) {
            best = elevator;
            found = true;
        }
    }
    return best;
}

}  // namespace dispatch

int main7() {
    std::vector<dispatch::Elevator> elevators{
        {2, 8, 8},   // floor 2, FULL
        {9, 3, 8},   // floor 9, has room
        {4, 1, 8},   // floor 4, has room
    };
    dispatch::Elevator best = dispatch::findBestElevator(elevators, 3);
    // Floor 2 is closest to 3 but full, so we get the one on floor 4.
    std::cout << "Best elevator is on floor " << best.getFloor() << '\n';  // 4
    return 0;
}

// =====================================================================================
// 12) A class template: Vector, with the const correctness of sections 15 and 16
// =====================================================================================
// ---- file: Vector.h ----
template <typename T>  // template declaration
class Vector {         // Vector is a template that takes in the name of a type T
   public:
    Vector();
    ~Vector();

    // Copying a class that owns memory needs more work, so we turn it off for now.
    Vector(const Vector& other) = delete;
    Vector& operator=(const Vector& other) = delete;

    // Section 15: const methods, they do not modify the object
    size_t size() const;
    bool empty() const;

    // Section 16: const overloading, one version for const and one for non-const objects
    const T& at(size_t index) const;
    T& at(size_t index);
    const T& operator[](size_t index) const;
    T& operator[](size_t index);

    void push_back(const T& elem);

   private:
    size_t logical_size;
    size_t capacity;
    T* elems;  // pointer to an array allocated on the heap
};  // T gets replaced when Vector is instantiated

// The compiler will produce something like:
// class Vector<int> {
//     int& at(size_t index);  // More methods...
// };

// #include "Vector.cpp"       // section 13.2: at the BOTTOM of the .h file

// ---- file: Vector.cpp ----
// Section 13.1: every definition needs template<...> and Vector<T>::
template <typename T>
Vector<T>::Vector() : logical_size{0}, capacity{4}, elems{new T[capacity]} {}

template <typename T>
Vector<T>::~Vector() {
    delete[] elems;
}

template <typename T>
size_t Vector<T>::size() const {
    return logical_size;
}

template <typename T>
bool Vector<T>::empty() const {
    return logical_size == 0;
}

template <typename T>
const T& Vector<T>::at(size_t index) const {
    return elems[index];  // (the real std::vector::at also checks the index)
}

template <typename T>
T& Vector<T>::at(size_t index) {
    return elems[index];
}

template <typename T>
const T& Vector<T>::operator[](size_t index) const {
    return elems[index];
}

template <typename T>
T& Vector<T>::operator[](size_t index) {
    return elems[index];
}

template <typename T>
void Vector<T>::push_back(const T& elem) {
    if (logical_size == capacity) {  // full ---> move to a bigger array
        capacity *= 2;
        T* bigger = new T[capacity];
        for (size_t i = 0; i < logical_size; i++) {
            bigger[i] = elems[i];
        }
        delete[] elems;
        elems = bigger;
    }
    elems[logical_size] = elem;
    logical_size++;
}

struct MyCustomStruct {
    int value;
};

// Section 13.3: typename and class mean the same thing here
template <typename T, class V>
struct Pair {
    T first;
    V second;
};

int main8() {
    // Template instantiation: the code for each type is generated here, on demand.
    Vector<int> intVec;
    Vector<double> doubleVec;
    Vector<std::string> strVec;
    Vector<Vector<int>> vecVec;
    Vector<MyCustomStruct> structVec;

    intVec.push_back(1);
    doubleVec.push_back(2.5);
    strVec.push_back("hello");
    structVec.push_back(MyCustomStruct{7});

    std::cout << intVec.at(0) << " " << doubleVec.at(0) << " " << strVec.at(0) << " "
              << structVec.at(0).value << '\n';                  // 1 2.5 hello 7
    std::cout << "vecVec is empty: " << vecVec.empty() << '\n';  // 1 (true)

    Pair<int, std::string> pair{1, "one"};
    std::cout << pair.first << " " << pair.second << '\n';  // 1 one
    return 0;
}

// =====================================================================================
// 16) Const correctness: const methods and const overloading in action
// =====================================================================================
// v is const, so this function can only use the const interface of Vector (section 18).
void printVec(const Vector<int>& v) {
    for (size_t i = 0; i < v.size(); i++) {  // size() const
        std::cout << v.at(i) << " ";         // at() const ---> const int&
    }
    std::cout << '\n';
}

int main9() {
    Vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    v.at(0) = 42;  // v is non-const ---> calls the non-const at(), returns int&
    printVec(v);   // 42 2 3

    const Vector<int>& cv = v;
    std::cout << cv.at(0) << '\n';  // cv is const ---> calls the const at(): 42
    // cv.at(0) = 7;        // ERROR: const at() returns const int&
    // cv.push_back(4);     // ERROR: push_back is not part of the const interface
    return 0;
}

// =====================================================================================
// 17) What is this?
// =====================================================================================
// ---- file: point.h ----
class Point {
   private:
    int x;

   public:
    Point(int x);
    int getX() const;
    void setX(int x);
};

// ---- file: point.cpp ----
// #include "point.h"
Point::Point(int x) : x{x} {}

int Point::getX() const {  // this has type const Point*
    // this->x = 5;         // ERROR: we promised not to modify the object
    return this->x;
}

void Point::setX(int x) { this->x = x; }  // this has type Point*

int main10() {
    Point p{1};
    p.setX(2);
    std::cout << "p.getX(): " << p.getX() << '\n';  // 2

    const Point cp{3};
    // cp.setX(4);          // ERROR: setX is not a const method
    std::cout << "cp.getX(): " << cp.getX() << '\n';  // 3
    return 0;
}

// =====================================================================================
// 19) const_cast: casting away const
// =====================================================================================
int main11() {
    int value = 5;
    const int& ref = value;
    // ref = 6;             // ERROR: ref is a const reference
    int& mutableRef = const_cast<int&>(ref);
    mutableRef = 6;  // OK here, because value itself is not const
    std::cout << "value: " << value << '\n';  // 6
    return 0;
}

// =====================================================================================
// 20) The mutable keyword
// =====================================================================================
struct MutableStruct {
    int dontTouchThis;
    mutable double iCanChange;
};

int main12() {
    const MutableStruct cm{1, 2.0};
    // cm.dontTouchThis = 42;   // ERROR: cm is const
    cm.iCanChange = 3.14;  // OK: iCanChange is mutable
    std::cout << cm.dontTouchThis << " " << cm.iCanChange << '\n';  // 1 3.14
    return 0;
}

// =====================================================================================
// main: runs every example in order
// =====================================================================================
int main() {
    std::cout << "--- 2) Struct ---\n";
    main1();
    std::cout << "--- 4) Class: Elevator ---\n";
    main2();
    std::cout << "--- 5) Constructor and destructor ---\n";
    main3();
    std::cout << "--- 6) Inheritance: Square is a Rectangle ---\n";
    main4();
    std::cout << "--- 8) Virtuality: abstract Vehicle, Bus overrides ---\n";
    main5();
    std::cout << "--- 9) Diamond problem ---\n";
    main6();
    std::cout << "--- 10) Exercise: findBestElevator ---\n";
    main7();
    std::cout << "--- 12) Class templates ---\n";
    main8();
    std::cout << "--- 16) Const methods and const overloading ---\n";
    main9();
    std::cout << "--- 17) What is this? ---\n";
    main10();
    std::cout << "--- 19) const_cast ---\n";
    main11();
    std::cout << "--- 20) mutable ---\n";
    main12();
    return 0;
}
