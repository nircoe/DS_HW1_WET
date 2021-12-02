#ifndef AVL_EXCEPT_H
#define AVL_EXCEPT_H
#include <iostream>

class AVL_exception : public std::exception {};
class SUCCESS_exception : public AVL_exception {};
class FAILURE_exception : public AVL_exception {};
class INVALID_INPUT_exception : public AVL_exception {};
class ALLOCATION_ERROR_exception : public AVL_exception {};

#endif