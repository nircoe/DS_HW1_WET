#ifndef AVL_EXCEPT_H
#define AVL_EXCEPT_H
#include <iostream>
class AVL_exception : public std::exception
{
};
class SUCCESS : public AVL_exception
{
};
class FAILURE : public AVL_exception
{
};
class INVALID_INPUT : public AVL_exception
{
};
class ALLOCATION_ERROR : public AVL_exception
{
};

#endif