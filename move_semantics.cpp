// Copying data is expensive
// Why does it matter? How can we avoid needlessly copying data?

// Recap:
// Special Member Functions: handle class lifecycle
// 1- Copy Constructor ---> Type::Type(const Type& other); Type a = b ;
// 2- Copy assignment operator ---> Type& Type::operator=(const Type& other); a = b; 
// 3- Destructor ---> Type::~Type(); 
// Compiler creates these for us. 

class Photo{
	public:
		Photo(int width, int height);
		Photo(const Photo& other);
		Photo& operator=(const Photo& other);
		~Photo();
	private:
		int width;
		int height; 
		int* data; 
};

Photo::Photo(int width, int height): width(width),height(height), data(new int [width * height]){};

// Photo photo(500,500) creates a new photo and allocates memory.

Photo::Photo(const Photo& other): width(other.width), height(other.height), data(new int [width * height])
{
	std::copy(other.data, other.data + width * height, data);
}
// Photo p = photo; creates a new photo from an existing one and creates a copy of its data


Photo& Photo::operator=(const Photo& other){
	if (this==&other) return *this; // self assignment p = p; 
	delete[] data; // clean up old pixels
	width = other.width; 
	height = other.height; 
	data = new int [width * height]; 
	std::copy(other.data, other.data + width * height, data);
	return *this; 
}
// p = photo; replaces a photo's content with the content of another.


Photo::~Photo(){
	delete[] data; 
}


Photo takePhoto(); 
int main(){
	Photo selfie = takePhoto(); // the value returned from a function is destroyed before the next line
	// copy-constructor and destructor. 
	Photo retake(0,0);
	retake = takePhoto(); // Assignment and destructor
}


// What is the problem? takePhoto() ----> Photo selfie = takePhoto(); 
// takePhoto() makes a Photo object with width= 3840, height=2160, and data=0x1024c3bd.
// Photo selfie = takePhoto() ---> copy constructor ---> copies the data, then the destructor is called and removes the temporary object from takePhoto()
// What if we could reuse the memory instead?
// That is, use the same memory address we have for the temporary object from takePhoto()
// Why can't we? 1- We can use the same address for data (instead of copying, steal the data)
// Then it is a move constructor, not a copy constructor.
// What is the problem? When takePhoto() finishes, the destructor is called and removes the stolen data!!!!
// If we set the data pointer of the temporary object to nullptr, we can keep the data.
// Now we create a new object without copying the data.
// Q: Is it always safe to do it?
// Move vs Copy Semantics

Photo takePhoto(); 
void foo(Photo whoAmI){
	Photo selfie = whoAmI; // What if we move here?
	whoAmI.get_pixel(21,24); // ??? whoAmI is nullptr
}
// Building a new computer ---> 1- Copy Semantics : means I still want to use my old computer, 
// 								2- Move Semantics : I do not need my old computer. 


Photo selfie = pic; // make copies of a persistent object, it might get used in the future.
Photo selfie = takePhoto(); // move a temporary object since we no longer need it.

// How does the compiler know whether to move or copy?

// lvalues (have a definite address) & rvalues (no address): generalize the idea of temporariness.
// lvalue lifetime is until the end of scope.
// rvalue lifetime is until the next line.
// lvalue is persistent but rvalue is temporary.

// If we have an lvalue, how can we avoid copying its memory?

void uploadToInsta(Photo pic);
int main(){
	Photo selfie = takePhoto();
	uploadToInsta(selfie); // unnecessary copy is made here
}
// we can pass by reference (lvalue reference)
void uploadToInsta(Photo& pic);
int main(){
	Photo selfie = takePhoto();
	uploadToInsta(selfie); // No copy is made here
}
// Q
void uploadToInsta(Photo& pic);
int main(){
	uploadToInsta(takePhoto());
	// Does it work? The function expects an lvalue as the argument.
}

// How can we fix it? (rvalue reference)
void uploadToInsta(Photo&& pic);// we can do anything that we want with pic. It is temporary.
int main(){
	uploadToInsta(takePhoto());
	
}

// lvalue reference ---> syntax: Type&, persistent: must keep the object in a valid state after the function
// rvalue reference ----> syntax: Type&&, temporary: we can steal (move) its resources. It might end up in an invalid state, but that is OK.
// Overloading & and && parameters distinguishes lvalue and rvalue references
// The compiler decides which version of upload to call depending on
// whether the argument is an lvalue or an rvalue!

// Let's overload the special member functions

// Copy constructor
Photo::Photo(const Photo& other): width(other.width), height(other.height), data(new int [width * height])
{
	std::copy(other.data, other.data + width * height, data);
}
// Move constructor 
Photo::Photo(const Photo&& other): width(other.width), height(other.height)
{
	std::copy(other.data, other.data + width * height, data);
}
 
// We have two new special member functions: 1- Move Constructor Type::Type(Type&& other)
//											 2- Move Assignment Type& Type::operator=(Type&& other)


// std::move
// Forcing move semantics
// Usually we let the compiler decide between & and &&
// Is this the most efficient way? What if we know we will not use an lvalue again?
void PhotoCollection::insert(const Photo& pic, int pos){
	for (int i=size();  i > pos; i--){
		elems[i] = elems[i-1]; // copy elements to new spot.
	}
	elems[i] = pic; 
}
// We can write std::move(elems[i-1]) to use move semantics.
// std::move just casts an lvalue to an rvalue.

// We have 5 special member functions. Do we need to define all of them?

// Rule of Zero ---> If a class does not manage memory (or other external resources), the
// compiler generates the SMFs and they are sufficient.

// Rule of Three ---> If a class manages external resources ---> we must define the copy assignment/constructor
// If we don’t, compiler-generated SMF won’t copy underlying resource
// This will lead to bugs, e.g. two Photo’s referring to the same underlying data

// Rule of Three: If you need any one of these, you need them all: Destructor, Copy Assignment, Copy Constructor


// Rule of Five ---> If we defined the copy constructor/assignment and destructor, we should also define the move constructor/assignment
// This is not required, but our code will be slower as it involves unnecessary copying
// Rule of Five: If you need any of these, you probably want them all: Destructor, Copy Assignment, Copy Constructor, Move Assignment (optional), Move Constructor (optional)