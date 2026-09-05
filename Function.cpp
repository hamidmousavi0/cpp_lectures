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

 








