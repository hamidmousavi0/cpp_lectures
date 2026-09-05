#include <stdio.h> 
#include <iostream> 
























// --> Operator ( there is in c++ Iceberg)

void operator_iceberg(){
	int x = 10; 
	while(x --> 0){ // c goes to zero
		printf("%d ", x);
	}	
	
}

// else if is a lie. 
void elseif_lie(){
	int i = 10; 
	if (false){
		std::cout<<"hello" << std::endl;
	}
	else while (i>0){ // after else we can use if , while. so "else if" isn't special syntax
		std::cout<< i << std::endl; 
		--i; 
	}
}

//iostream is bad




// main function

int main(){
	operator_iceberg();
	elseif_lie();
	
		
}