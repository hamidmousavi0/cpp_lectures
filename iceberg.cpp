#include <stdio.h> 
#include <iostream> 
























// --> operator (from the C++ iceberg)

void operator_iceberg(){
	int x = 10; 
	while(x --> 0){ // x goes to zero (really: (x--) > 0)
		printf("%d ", x);
	}	
	
}

// "else if" is a lie.
void elseif_lie(){
	int i = 10; 
	if (false){
		std::cout<<"hello" << std::endl;
	}
	else while (i>0){ // after else we can use any statement (if, while, ...), so "else if" isn't special syntax
		std::cout<< i << std::endl; 
		--i; 
	}
}

// iostream is bad




// main function

int main(){
	operator_iceberg();
	elseif_lie();
	
		
}