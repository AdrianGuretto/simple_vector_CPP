# Simple C++ Vector
Simple implementation of a STL vector

## Contents
* *array_ptr.h*  -  file containing ```ArrayPtr``` template class, which is my own array smart pointer implementation.
  * __Type* Get()__ - returns raw pointer to the array
  * __[[nodiscard]] Type* Release()__ - returns the raw pointer and sets its private pointer variable to ```nullptr```
  * __swap(ArrayPtr& other_array)__ - swaps contents with another ```ArrayPtr``` object
  * ___swap(Type* other_raw_ptr_)__ - swaps private raw_pointer with the provided one

* *simple_vector.h* - file containing ```SimpleVector``` template class. It supports basic STL vector functions: *begin()*, *cbegin()*, *end()*, *cend()*, *GetSize()*, *Insert()*, *Clear()*, *GetCapacity()*, *Reserve()*, *Resize()*, *PopBack()*, *PushBack()*, *Erase()*, *At()*, *IsEmpty()*

* *main.cpp* - file containing tests for ```SimpleVector``` and ```ArrayPtr``` clases.

