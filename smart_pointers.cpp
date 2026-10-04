#include<iostream> 
#include<string> 
#include<fstream>
#include<memory>
/*
Why do we need smart pointers?

std::string returnNameCheckPawsome(int petId){
	Pet* p = new Pet(petId);
	
	if (p.type() == "Dog"){
		std::cout<< p.firstName() << std::endl; 
	}
	std::string returnStr = p.firstName()
	// If an exception is thrown before the object is deleted, we have a memory leak.
	delete p;
	return returnStr;
	
*/

// Is this just related to pointers? No 
//1 - Heap Memory ---> new , delete
//2 - Files ---> open , close 
//3 - Locks ---> try_lock, unlock
//4 - Sockets ---> socket, close 

// Resources need to be released after they are acquired.

// Q: How can we ensure that we release resources in the case that we have an exception?
// A: RAII ---> Rule1: All resources acquired by a class should be acquired in the constructor.
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

// Is this RAII compliant? No, the ifstream is opened and closed in code, not in the constructor and destructor
// Q: How can we fix this? RAII for memory (smart pointers).
// Avoid calling new and delete explicitly.
// new returns a pointer ---> if the pointer is assigned to a plain pointer ---> memory leak
// RAII for locks ---> lock_guard ---> creates a new object that acquires resources in the constructor and releases them in the destructor.
// Smart Pointers ---> do the same for memory --> Smart Pointer class --> Dynamically Acquired Resources.

// We have three types:
// 1- std::unique_ptr ---> uniquely owns its resources, cannot be copied.
// 2- std::shared_ptr ---> can make copies; the underlying memory is freed when the last shared_ptr goes out of scope.
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
	std::unique_ptr<Node> copy = n; // error, if the original's destructor is called, the copy points to deallocated resources


	// shared_ptr solves this problem by deallocating the memory when all the shared pointers go out of scope.
	// shared_ptr ---> pointer to T ---> Data in T object.
	//            ---> pointer to Control block ----> has some information (reference count, weak count, ...)
}
	
	/*
	How to initialize: 
	1- std::unique_ptr<T> uniquePtr {new T};
	2- std::shared_ptr<T> sharedPtr {new T}; Q: we are still calling new????? No No
	3- std::weak_ptr<T> wp = sharedPtr;
	*/
// We need to write: 
// 1 - std::unique_ptr<T> uniquePtr = std::make_unique<T>(); 
// 2 - std::shared_ptr<T> sharedPtr = std::make_shared<T>();


// std::weak_ptr is a pointer that can look at an object owned by a shared_ptr without claiming ownership.
// Q: Does it affect the reference count? No

// std::weak_ptr can resolve the circular dependency:
/*
class B; 

class A{
	public: 
		std::shared_ptr<B> ptr_to_b; 
		~A(){
			std::cout << "All A resources deallocated";
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