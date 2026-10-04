// Copy data is so expensive 
// Why does it matter? How can we aviod needlessly copying data?

//Recap: 
// Special Member Functions: handle class lifecycle 2
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

// Photo photo(500,500) create new photo and allocates memory. 

Photo::Photo(const Photo& other): width(other.width), height(other.height), data(new int [width * height])
{
	std::copy(other.data, other.data + width * height, data);
}
// Photo p = photo; creates a new photo from an existing one and creat a copy of its data


Photo& Photo::operator=(const Photo& other){
	if (this==&other) return *this; // self assignment p = p; 
	delete[] data; // old pixel clean 
	width = other.width; 
	height = other.height; 
	data = new int [width * height]; 
	std::copy(other.data, other.data + width * height, data);
	return *this; 
}
// p = photo; replace a photo's content with  content of other. 


Photo::~Photo(){
	delete[] data; 
}


Photo takePhoto(); 
int main(){
	Photo selfie = takePhoto(); // the value return from a function destroy before the next line
	// copy-constructor and destructor. 
	Photo retake(0,0);
	retake = takePhoto(); // Assignment and destructor
}


// What is the problem? takePhoto() ----> Photo selfie = takePhoto(); 
// takePhoto() makes a Photo object with width= 3840, height=2160, and data=0x1024c3bd.
// Photo selfie = takePhoto() ---> copy constructor ---> copy data and destructor call and remove the temporary object from takePhoto()
// What if we can reuse the memory instead? 
// It means that use the same memory address we have for temporary object from takePhoto()
// Why we can not? 1- we can use the same address for data (instead of copy steal the data)
// Then it is move constructor not copy constructor. 
// What is the problem? when takePhoto() finish then destructor called and remove the stolen data!!!!
// if we change the data pointer to nullptr for temporary object from function we can have the data. 
// now we create a new data without copy data. 
// Q: Is it always safe to do it?
// Move vs Copy Semantics

Photo takePhoto(); 
void foo(Photo whoAmI){
	Photo selfie = whoAmI; // What is we move here? 
	whoAmI.get_pixel(21,24); // ??? whoAmI is nullptr
}
// Building a new computer ---> 1- Copy Semantics : means I still want to use my old computer, 
// 								2- Move Semantics : I do not need my old computer. 


Photo selfie = pic; // make copies of persistant object, it might get used in future. 
Photo selfie = takePhoto(); //move temporary object since we do not longer need. 

// How does compiler know whether to move or copy? 

// lvalues (have a definete address) & rvalues (no address): generalize the idea of temproriness. 
// lvalue lifetime is until the end of scope. 
// rvalue life time is until the next line. 
// lvalue is persistant but rvalue is temporary.

// IF we have an lvalue how can we aviod copying its memory? 

void uploadToInsta(Photo pic);
int main(){
	Photo selfie = takePhoto();
	uploadToInsta(selfie); // unnecessary copy is made here
}
// we can pass by refrence (lvalue refrence)
void uploadToInsta(Photo& pic);
int main(){
	Photo selfie = takePhoto();
	uploadToInsta(selfie); // No copy is made here
}
// Q
void uploadToInsta(Photo& pic);
int main(){
	uploadToInsta(takePhoto());
	// Does it work? the function expect lvalue as the argument. 
}

// How can we fix it? (rvalue refrence)
void uploadToInsta(Photo&& pic);// we can do anything that we want with pic. it is temporary. 
int main(){
	uploadToInsta(takePhoto());
	
}

// lvalue refrence---> syntax: Type&, persistant: must keep object in valid state after the fucntion
// rvalue refrence----> syntax: Type&& , Temporary we can steal (move) its resources. it might end up with invalid state but that is ok. 
// overloading & and && parameters distinguidh lvalue and rvalue refrences
// Compiler decides which version of upload to call depending on
// whether argument is lvalue or rvalue! 

// lets overload the special member functions

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
 
// We have two new special member functions: 1- Move Constructor Type::(Type&& other) 
//											 2- Type& Type::operator=(Type&& other)


// std::move 
// Forcing move semantic 
// usally we let the compiler decide between & and &&
// Is this the most efficient way? why if we know we do not use an lvalue again? 
void PhotoCollection::insert(const Photo& pic, int pos){
	for (int i=siz();  i > pos; i--){
		elems[i] = elems[i-1]; // copy elements to new spot.
	}
	elems[i] = pic; 
}
// we can write std::move(elems[i-1]) to use move semnatic. 
// std::move just type-cast and lvalue to an rvalue. 

// we have 5 Special Member functions. Do we need to define all of them? 

// Rule of Zero ---> IF class does not manage the memory (or another external resources) the 
// compiler generates the SMFs and they are sufficent. 

// Rule of Three ---> If class manage external resources ---> we must define Copy assignment/constructor
// If we don’t, compiler-generated SMF won’t copy underlying resource
// This will lead to bugs, e.g. two Photo’s referring to the same underlying data

// Rule of Three: If you need any one of these, you need them all:• Destructor• Copy Assignment• Copy Constructor


// Rule of Five ---> • If we defined copy constructor/assignment and destructor, we should also define move constructor/assignment
// This is not required, but our code will be slower as it involves unnecessary copying	
// Rule of Five: If you need any of these, you probably want them all:• Destructor• Copy Assignment• Copy Constructor• Move Assignment (Optional)• Move Constructor (Optional)