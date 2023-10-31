#include <cstring>
#include "string327.h"
#include <cstdio>
#include <cstdlib> //malloc and free

string327::string327(){
    str = strdup("");
}

string327::string327(const char *s){
    str = strdup(s);
}

string327::string327(const string327 &s){
    str = strdup(s.str);
}


string327::~string327(){
    free(str);
}

int string327::length(){
    return strlen(str);
}

bool string327::operator<(const string327 &rhs){
    return strcmp(str, rhs.str) < 0;
}

bool string327::operator>(const string327 &rhs){
    return strcmp(str, rhs.str) > 0;
}

bool string327::operator==(const string327 &rhs){
    return strcmp(str, rhs.str) == 0;
}

bool string327::operator!=(const string327 &rhs){
    return strcmp(str, rhs.str) != 0;
}

bool string327::operator<=(const string327 &rhs){
    return strcmp(str, rhs.str) <= 0;
}

bool string327::operator>=(const string327 &rhs){
    return strcmp(str, rhs.str) >= 0;
}

string327 string327::operator+(const string327 &rhs){
    char *temp = (char *)malloc(strlen(str) + strlen(rhs.str) + 1);
    strcpy(temp, str);
    strcat(temp, rhs.str);
    string327 s(temp);
    free(temp);
    return s;
}

