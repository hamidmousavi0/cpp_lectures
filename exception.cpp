// Exceptions: throw and catch: handle exceptions so the code runs without crashing.
/*
try{
     // code that we check for exception
}
catch ([exception type] e1){
    // behavior when we encounter an error
}
catch ([exception type] e2){
    // else if
}
catch (...){
    // else: catch-all
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
            throw(age); // throw an exception ---> the catch below handles it
        }
    }
    catch (int myNum)
    {
        std::cout << "access denied" << std::endl;
        std::cout << "Age is: " << myNum;
    }
}
