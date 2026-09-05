// Exception: Thrown but catch: handle exceptions to run code without error.
/*
try{
     // code that we check for exception
}
catch ([exception type] e1){
    // behavior when we enconter an error
}
catch ([exception type] e2){
    // else if
}
catch {
    // else catch_all
}
*/
#include <iostream>

int main()
{
    try
    {
        int age = 15;
        if (age >= 18)
        {
            std::cout << "Access Granted" << std::endl;
        }
        else
        {
            throw(age); // exception throw ---> we have a catch that manage this
        }
    }
    catch (int myNum)
    {
        std::cout << "access denied" << std::endl;
        std::cout << "Age is: " << myNum;
    }
}
