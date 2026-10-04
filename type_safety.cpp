//Type Safety: How much a language prevents type errors
//           : How much a language guarantees the behavior of the program
//	         : How much a function signature guarantees the behavior of the function
			 
			 

#include<vector>
void removeOddsFromEnd(std::vector<int>& vec){
	while(vec.back() % 2  == 1 ) {
		vec.pop_back();
	}
}
// What if vec is {} (an empty vector)?
//Undefined behavior: Function could crash, could give us
// garbage, could accidentally give us some actual value
// Solution:
void removeOddsFromEnd(std::vector<int>& vec){
	while(!vec.empty()&& vec.back() %2==1){
		vec.pop_back();
	}
}

// Look at vec.back()
/*
value_type& vector<value_type>::back(){
	return *(begin() + size() - 1);
}

*/

// One solution

std::pair<bool, valueType&> vector<valueType>::back(){
	if(empty()){
		return {false,valueType()}; // valueType() calls the default constructor
	}
	return {true, *(begin() + size() -1)};
}
// What happens if valueType does not have a default constructor?
// Even if it does, calling the constructor can be expensive.


// Q: What should back return ?
/*

??? vector<valueType>::back(){
	if(empty()){
	return ??;
	}
	return *(begin()+size()-1);
}
*/

// std::optional

// What is std::optional<T>? It is a template class which will either contain a value of type T or contain nothing (nullopt)
// nullopt is different from nullptr
// nullptr: is an object that can convert to a value of any pointer type.
// nullopt: an object that can be converted to any optional type. 

// Example: 
/*
int* p = nullptr; 
if(p==nullptr){
	std::cout<< "p is a null pointer";
}
std::optional<int> x = nullptr; // ERROR.
*/
/*
std::optional<int> x = std::nullopt;
if(!x){
	std::cout<< "x is empty optional";
}
int* p = std::nullopt; //ERROR
*/


void main(){
	std::optional<int> num1 = {}; // num1 does not have any value. 
	num1 = 1 ; // it does
	num1 = std::nullopt; // now it does not
}

std::optional<valueType> vector<valueType>::back(){
	if(empty()){
		return {};
	}
	return *(begin() + size() -1);
}

void removeOddsFromEnd(vector<int>& vec){
	while(vec.back() % 2 ==1){ // we cannot do arithmetic with an optional; we need to get its value if it exists
		vec.pop_back();
	}
}
	
// std::optional types have a .value() method that returns the contained value or throws bad_optional_access.

// It also has a .value_or(valueType val) method that returns the value, or val if it is empty.

// It has .has_value(), which returns true if it contains a value and false otherwise.

#include<iostream>
#include<optional> 

int main(){
	std::optional<int> a  = 5; 
	std::optional<int> b = std::nullopt;
	std::cout<<"a.has_value(): " << a.has_value() << "\n" ;
	std::cout<<"b.has_value(): " << b.has_value() << "\n" ;
	if (a.has_value()){
		std::cout<<"a.value()" << a.value() << "\n"; 
	}
	
}

// Revisiting back()

void removeOddsFromEnd(vector<int>& vec){
	while(vec.back().has_value() && vec.back().value() % 2 ==1){
		vec.pop_back();
	}
}

// .and_then(function f) ---> returns the result of calling f(value) if a contained value exists, otherwise nullopt
// (f must return std::optional)

#include<iostream> 
#include<optional>
std::optional<int> half(int x){
	if (x % 2 ==0) return x/2; 
	return std::nullopt;
}
int main(){
	std::optional<int> a = 5; 
	auto result = a.and_then(half).and_then(half).and_then(half);
	if (result) std::cout<<result; 
	std::optional<int> b = 7; 
	auto result2 = b.and_then(half);
}
// We also have .transform(function f)
// We also have .or_else(function f) --> returns the value if it exists, otherwise returns the result of calling f.




