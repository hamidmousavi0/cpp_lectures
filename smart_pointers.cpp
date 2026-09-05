#include<iostream> 
#include<string> 
#include<ifstream>
#include<memory>
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
//3 - Locks ---> try_lock, unlock
//4 - Sockets ---> socket, close 

// The resources need to release after Acquiring. 

// Q: How can we ensure that we release resources in the case that we have an exception. 
// A: RAII ---> Rule1: All resources acquire with a class should be acquired in the constructor. 
//         ---> Rule2: All resources used by a class should be released in the destructor. 


// Example: 
void printFile(){
	ifstream input;
	input.open("hamlet.txt"); 
	string line; 
	while(getline(input,line)){
		std::cout << line << std::endl; 
	}
	input.close(); 
}

// Is this RAII compliant?  No , ifstream open and closed in code not constructor and destructor
// Q: how can we fix this? RAII for memory (Smart pointer). 
// Avoid calling new and delete explicily. 
// new return a pointer ---> if the pointer assigned to a plain pointer ---> object leckage
// RAII for locks ---> lock_guard ---> create a new object that acquire resources in constructor and release in destructor. 
// Smart Pointers ---> do the same in memory --> Smart Pointer class --> Dynamically Acquired Resources.

// we have Three types: 
// 1- std::unique_ptr ---> uniquely own it's resources, can not be copied.
// 2- std::shared_ptr ---> can make copies, destructed when the underlaying memory goes out of scope. 
// 3- std::weak_ptr ---> a class of pointers designed to mitigate circular dependency. 

// BAD
void rawPtrFn(){
		Node* n = new Node; 
		// do sth with n 
		delete n 
}

// GOOD

void rawPtrFN(){
	std::unique_ptr<Node> n {new Node}; 
	std::unique_ptr<Node> copy = n; // error, if the original destructor is called after copy then the copy point to deallocated resources
	
	
	// shared pointer solve this problem and deallocating the memory when all the share pointer go out of scope. 
	// shared_ptr ---> pointer to T ---> Data in T object. 
	//            ---> pointer to Control block ----> have some information (refrences count, weak count, ...)
}
	
	/*
	How to initialize: 
	1- std::unique_ptr<T> uniquePtr {new T};
	2- std::shared_ptr<T> sharedPtr {new T}; Q: we are still call new????? No No 
	3- std::weak_ptr<T> wp = sharedPtr;
	*/
// We need to write: 
// 1 - std::unique_ptr<T> uniquePtr = std::make_unique<T>(); 
// 2 - std::shared_ptr<T> sharedPtr = std::make_shared<T>();


// std::weak_ptr is a pointer that can  look into the object own by shared_ptr without claiming of the ownership. 
// Q: does it effect the refrences count? No

// std::weak_ptr can resolve the cirular dependency:
/*
class B; 

class A{
	public: 
		std::shared_ptr<B> ptr_to_b; 
		~A(){
			std::cout << "All A resources deallocate"; 
		}
};

class B{
	public:
		std::weak_ptr<A> ptr_to_a; 
		~B(){
			std::cout<<"All B resources deallocated";
		}
};

int main(){
	std::shared_ptr<A> shared_ptr_to_a = std::make_shared<A> (); 
	std::shared_ptr<A> shared_ptr_to_b = std::make_shared<B> (); 
	
	a-> ptr_to_b = shared_ptr_to_b;
	b-> ptr_to_a = shared_ptr_to_a;
}
*/