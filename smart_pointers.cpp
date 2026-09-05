#include<iostream> 
#include<string> 

/*
Why we need smart pointers: 

std::string returnNameCheckPawsome(int petId){
	Pet* p = new Pet(petId);
	
	if (p.type() == "Dog"){
		std::cout<< p.firstName() << std::endl; 
	}
	std::string retrunstr = p.firstName()
	// If a throw happen before delete the object then we have memory leckage. 
	delete p; 
	return retrunstr; 
	
*/

// Is this just related to pointers? No 
//1 - Heap Memory ---> new , delete
//2 - Files ---> open , close 
//3 - 