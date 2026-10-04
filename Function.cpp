// Function 
<Return Type> Function_Name (Arguments){

	// Function Body

}

// Example: 

int min(int a, int b) {
	return a < b ? a:b ;
}


// To have a function for different types we can use template. 

// Template Function: 

template <typename T> 
T min(T a, T b){
	return a < b ? a:b; 
} 



// To avoid copy we can use refrences

template <typename T> 
T min(const T& a, const T& b){
	return a<b ? a:b; 

// call template function
min<int> (106,107); // or 
min(106,107) // compiler can find the type automatically like auto.

// Example: 
 

template <typename Iterator, typename TElm>
Iterator find(Iterator begin, Iterator end, TElm value){
	Iterator it = begin; 
	while(it != end){
		if(*it==value) breal; 
		++it; 
	}
	return it; 
}
std::vector<int> v{106,111,42,112}; 
auto it = find(v.begin(),v.end(),42);
*it = 107;
	


// Concept: Putting Contraints on the Templates 

template <typename T> 
concept Comparable = requires(T a , T b){ // Given a and b the following requires to hold
	{a < b}-> std::convirtable_to<bool>; // {a < b} is a constraint and must compile without error 
	// -> means the result must be boolean. 
	
}; 

template <typename T> requires Comparable<T> 
T min(const T& a, const T& b){
	return a < b ? a : b; 
}
// or 
template <Comparable T> 
T min(const T& a, const T& b){
	return a < b ? a : b; 
}


// Variadic Templates: variable numbers of parameters
// This is based on the recuresion
// Base case
template <Comparable T> 
T min (const T& a , const T& b);

template <Comparable T, Comparable... Args>
T min(const T& v, const Args&... args){
	auto m = min(args...);
	return v < m ? v : m; 
}

// Template Metaprogramming: how can we do work at compile time. 
template <>
struct Factorial<0> {
	enum{value =1 }; // enum is a way to store compile time constant
};
template <size_t N>
struct Factorial{
	enum{value=N*Factorial<N-1>::value>};
};
std::cout<<Factorial<7>::value<<std::endl;
// TMP baking results into an executable at compile time. 
// we can have Compile-time execution and Readable code using constexr and consteval.

constexpr size_t factorial(size_t n){
	if (n==0) return 1;
	return n * factorial(n-1);
} // Dear compiler try to run me at compile time.

consteval size_t factorail(size_t n){
	if(n==0) return 1;
	return n * factorail(n-1)
};
// Dear compiler you must run me at compile time. 

// Functions and Lambdas: how can we represent a function as a variable
// Predicate: is a boolean-value function. 

bool isVowel(char c){
	c = toupper(c);
	return c=='A' || c=='E' || c=='I'; 
}
bool isDivisable(int n, int d){
	return n % d  == 0; 
}
// How can we use isVowel to find first vowel in the string? 
// we need to pass a predicate to a function. 

 






























// Special Member functions

// Recap: Non-member overloading
// Non-member operator overloading
bool operator<(const StudenID& lhs, const StudenID& rhs); 
// member operator overloading 
bool StudenID::operator<(const StudenID& rhs) const {...}; 
// Advantage of non-member overloading over the member overloading? It is symmetric. 
// What do you sometimes have to do with non-member
// overloading that you don’t with member overloading? If you need to access private/protected fields, you
// need to specify that it is a friend.
// .cpp file
#include StanfordID.h 
std::string StanfordID::getIdNumber(){
	return idNumber; 
}
bool StanfordID::operator<(const StanfordID& other) const{
	return idNumber < other.getIdNumber(); 
}
// what happen if we have 

bool operator<(const StanfordID& lhs, const StanfordID& rhs) const{
	return lhs.idNumber < rhs.idNumber; // Error idNumber is private. 
}
// We must write 
bool operator<(const StanfordID& lhs, const StanfordID& rhs) const{
	return lhs.getIdNumber() < rhs.getIdNumber(); 
}


// The friend keyword allows non-member functions or classes to access private information in another class. 
// How do we use the friend? in the header of the target class we declare the operator overload function as a friend. 
// Notice: If StanfordID didn’t have agetIdNumber() method, you’d have to add friendto access idNumber directly



// in classes we have ---> Constructor, Destructor ---> these are special member functions. 
// A constructor is called when a new instance of class is created. and the destructor called when it goes out of scope.
// Default Constructor --> T()
// Destructor ---> ~T()
// Copy Constructor ---> T(const& T&)
// Copy Assignment operator ---> T& operator=(const T&)
// Move Constructor ---> T(T&&)
// Move assignment operator T& operator=(T&&) 

class Widget{
	public: 
		Widget(); // default constructor
		Widget(const Widget& w); // copy constructor  ---> member-wise copy of another
		Widget& operator = (const Widget& w); // copy assignment operator --> assign already exist object to another
		~Widget(); // default destructor
		Widget(const Widget&& rhs); // Move constructor
		Widget& operator = (Widget&& rhs); // move assignment operator
}
// these are have default versions that generated automatically. 
// when we create a constructor we need to initialize the member variables. 
template <typename T> 
Vector<T>::Vector()
{
	_size = 0; // init and reassign is inefficient
	_capacity = 4; 
	_data = new T[_capacity];
}
// How can we do it efficiently? initializer lists

template <typename T> 
Vector<T>::Vector():_size (0), _capacity(4), _data(new T[_capacity]){}; // init and assign at once. 

// What if the variables are not assignable? 
template <typename T> 
class  Myclass{
	const int _constant; 
	int& _refrence; 
	public: 
		Myclass(int value, int& ref): _constant(value), _refrence(ref){}
};
// The only way we can init is init lists becuase we can not declare and assign value to const member. 

// Why should we override the SMFs? 
// By default the copy constructor make a  copy of each member variable. 
// Member-wise copying. Q: Is it always good enough? 

// consider pointers: if var is a pointer---> a member-wise copy will point to the same allocated data. 

template<typename T> 
Vector<T>::Vector<T>(const Vector::Vector<T>& other): _size(other._size), _capacity(other._capacity), _data(other._data){}; 
// pointers point at the same array. 

//Many times, you will want to create a copy that does more than just copies the member 	variables.
// Deep copy: an object that is a complete, independent copy of the original
// In these cases, you’d want to override the default special member functions with your own implementation!
// Declare them in the header and write their implementation in the .cpp, like any function!
template<typename T> 
Vector<T>::Vector<T>(const Vector::Vector<T>& other): _size(other._size), _capacity(other._capacity),
 _data(new T[other._capacity]){
	 for(size_t i = 0; i < _size ; ++i){
		 _data[i] = other._data[i];
	 }
 };
// Now we have deepcopy. 


// How do you prevent copy: 
class PasswordManager{
	public:
		PasswordManager(); 
		~PasswordManager(); 
		PasswordManager(const PasswordManager& rhs); 
		PasswordManager& operator=(const PasswordManager& rhd);
	private:
		// other members
}
// by using delete we can remove smfs. 
class PasswordManager{
	public:
		PasswordManager(); 
		~PasswordManager(); 
		PasswordManager(const PasswordManager& rhs)=delete; 
		PasswordManager& operator=(const PasswordManager& rhd)=delete;
	private:
		// other members
}
// now copying is not possible. 

// This is how calsses such as std::unique_ptr works. 

// Rule of Zero ---> If the default SMFs work, don’t define your own!
// This usually happens when we work with dynamically allocated memory, like pointersto things on the heap!

// Rule of Three ---> IF you need a custom destructor, then you also probably need to define a copy
// constructor and a copy assignment operator for your class


// Is Copying enough? The copy constructor will copy every value in thevalues map one by one!
// Very slowly! ---> solution is Move semantics. 