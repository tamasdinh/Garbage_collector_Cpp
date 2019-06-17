// This class defines an element that is stored in the garbage collection information list.
#include <iostream>

template <class T>
class PtrDetails {

    public: // all members are public as they have to be used by other classes

        unsigned refcount;  // current reference count
        T *memPtr;          // pointer to allocated memory

        bool isArray;       // isArray is true if memPtr points to an allocated array.
        unsigned arraySize; // If memPtr is pointing to an allocated array, then arraySize contains its size

    PtrDetails(T* _ptr, unsigned _arraySize = 0): refcount(1), memPtr(_ptr), arraySize(_arraySize) {
        isArray = _arraySize > 0;
    }
};

// Overloading operator == allows two class objects to be compared. This is needed by the STL list class.
template <class T>
bool operator == (const PtrDetails<T> &ob1, const PtrDetails<T> &ob2) {
    return (ob1.memPtr == ob2.memPtr);
}