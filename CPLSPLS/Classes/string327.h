#ifndef STRING327.H
#define STRING327.H
#include <iostream>

class string327
{
    private:
        char *str; // pointer to the string
    public:
        string327(); // default constructor
        string327(const char *s); // constructor
        string327(const string327 &s); // copy constructor
        ~string327(); // destructor
        int length(); // returns the length of the string
        
        //method and function are different
        //method is a function that is a member of a class
        //function is a function that is not a member of a class

        bool operator<(const string327 &rhs); // less than operator
        bool operator>(const string327 &rhs); // greater than operator
        bool operator==(const string327 &rhs); // equal to operator
        bool operator!=(const string327 &rhs); // not equal to operator
        bool operator<=(const string327 &rhs); // less than or equal to operator
        bool operator>=(const string327 &rhs); // greater than or equal to operator

        string327 operator+(const string327 &rhs); // concatenation operator
        string327 &operator+=(const string327 &rhs); // concatenation assignment operator

        char &operator[] (const int i); // index operator

        const char *c_str(); // returns a pointer to the string

        friend std::istream &operator>>(std::istream &lhs, const string327 &rhs); // output operator
};

std::ostream &operator<<(std::ostream &lhs, const string327 &rhs); // input operator

#endif