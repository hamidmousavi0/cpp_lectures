//Type Safety: How much a language prevents from typing error
//           : How much a language guarantee the behaviour of the program 
//	         : Which a function signiture guarantee the bahavior of the function 
			 
			 

#include<vector>
void removeOddsFromEnd(std::vector<int>& vec){
	while(vec.back() % 2  == 1 ) {
		vec.pop_back();
	}
}
// What if vec is {}/ an empty vector? 
//Undefined behavior: Function could crash, could give us
// garbage, could accidentally give us some actual value
// Solution:
void removeOddsFromEnd(std::vector<int>& vec){
	while(!vec.empty()&& vec.back() %2==1){
		vec.pop_back();
	}
}

// look at the vec.back()
/*
value_type& vector<value_type>::back(){
	return *(begin() + size() - 1);
}

*/

// one solution

std::pair<bool, valueType&> vector<valueType>::back(){
	if(empty()){
		return {false,valueType()}; // valueType is the default constructor
	}
	return {true, *(begin() + size() -1)};
}
// What happen if valueType does not have a default construcor
// even it does calling constructor is so expensive. 


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

// What is std::optional<T>? is a template class which will either contain a value of type T or contain nothing (nullopt)
// nullopt is different wtih nullptr
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
	while(vec.back() % 2 ==1){ // we can not do arithmetic with an optional we need to get value of it if exist
		vec.pop_back();
	}
}
	
// std::optional types have a: .value() method that return contained value or throw bad_optional_access. 

// it has also .value_or(valueType val) method that return value or default val.

// it has .has_value() return true if contained value and false otherwise. 

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

// .and_then(function f) ---> return the result of calling f(value) if contained value exist, otherwise nullopt
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
// we have also .transform(function f) 
// we have also .or_else(function f) --> return value if exist otherwise return the result of calling f. 




