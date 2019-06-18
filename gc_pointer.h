#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"

/*
    Pointer implements a pointer type that uses garbage collection to release unused memory.
    A Pointer must only be used to point to memory that was dynamically allocated using new.
    When used to refer to an allocated array, specify the array size.
*/

template <class T, int size = 0>
class Pointer {

    private:
        static std::list<PtrDetails<T>> refContainer;      // maintains the garbage collection list.

        T *addr;    // points to the allocated memory to which this Pointer pointer currently points.

        bool isArray;   // true if this Pointer points to an allocated array.

        unsigned arraySize; // If this Pointer is pointing to an allocated array, then arraySize contains its size.

        static bool first; // true when first Pointer is created

        typename std::list<PtrDetails<T>>::iterator findPtrInfo(T *ptr); // Return an iterator to pointer details in refContainer.


    public:
        typedef Iter<T> GCiterator;     // Define an iterator type for Pointer<T>.

// NOTE: templates aren't able to have prototypes with default arguments this is why constructor is designed like this:
        Pointer() {
            Pointer(NULL);
        }

        Pointer(T*);    // Copy constructor

        Pointer(const Pointer &);

        ~Pointer();

        static bool collect();      // Collect garbage. Returns true if at least one object was freed.

        T *operator=(T *t);         // Overload assignment of pointer to Pointer.

        Pointer &operator=(Pointer &rv);    // Overload assignment of pointer to Pointer.

        T &operator*() {            // Return a reference to the object pointed to by this Pointer.
            return *addr;
        }

        T *operator->() { return addr; }    // Return the address being pointed to.

        T &operator[](int i){ return addr[i];}  // Return a reference to the object at the index specified by i.

        operator T *() { return addr; }         // Conversion function to T *.

        Iter<T> begin() {                       // Return an Iter to the start of the allocated memory.
            int _size;
            if (isArray)
                _size = arraySize;
            else
                _size = 1;
            return Iter<T>(addr, addr, addr + _size);
        }


        Iter<T> end(){                          // Return an Iter to one past the end of an allocated array.
            int _size;
            if (isArray)
                _size = arraySize;
            else
                _size = 1;
            return Iter<T>(addr + _size, addr, addr + _size);
        }

        static int refContainerSize() { return refContainer.size(); } // Return the size of refContainer for Pointer.

        static void showlist();     // A utility function that displays refContainer.

        static void shutdown();     // Clear refContainer when program exits.
};


// STATIC INITIALIZATION

template <class T, int size>
std::list<PtrDetails<T>> Pointer<T, size>::refContainer;       // Creates storage for the static variables

template <class T, int size>
bool Pointer<T, size>::first = true;

template<class T,int size>
Pointer<T,size>::Pointer(T *t) {   // Constructor for both initialized and uninitialized objects. -> see class interface
    if (first)
        atexit(shutdown);         // Register shutdown() as an exit function.
    first = false;

    typename std::list<PtrDetails<T>>::iterator p;    // initialize iterator p for list of pointers with type T

    p = findPtrInfo(addr);
    if (p->refcount)
        p->refcount--;

    p = findPtrInfo(t);
    if (p->refcount) {
        p->refcount++;
    } else {
        refContainer.push_back(PtrDetails<T>(t, size));
        std::cout << "*t Constructor - Pointer added to refContainer..." << std::endl;
    }
    addr = t;
    arraySize = size;
    isArray = arraySize > 0;
}

template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob){        // Copy constructor.

    typename std::list<PtrDetails<T>>::iterator p;    // initialize iterator p for list of pointers with type T

    p = findPtrInfo(addr);
    if (p->refcount)
        p->refcount--;

    p = findPtrInfo(ob.addr);
    if (p != refContainer.end()) {
        p->refcount++;
        std::cout << "&ob Constructor - Pointer refcount incremented..." << std::endl;
        addr = ob.addr;
        arraySize = ob.arraySize;
        isArray = arraySize > 0;
    }
}

template <class T, int size>
Pointer<T, size>::~Pointer(){           // Destructor for Pointer.

    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    if (p->refcount)
        p->refcount--;
    collect();
}

template <class T, int size>
T *Pointer<T, size>::operator=(T *t){       // Overload assignment of pointer to Pointer.

    typename std::list<PtrDetails<T>>::iterator p;

    p = findPtrInfo(addr);
    if (p->refcount)
        p->refcount--;

    addr = t;
    p = findPtrInfo(addr);
    if (p != refContainer.end()) {
        p->refcount++;
        std::cout << "op= override - Pointer found in refContainer; refcount incremented..." << std::endl;
    } else {
        refContainer.push_back(PtrDetails<T>(addr, size));
        std::cout << "op= override - Pointer added to refContainer..." << std::endl;
    }

    return t;

}

template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){         // Overload assignment of Pointer to Pointer.

    typename std::list<PtrDetails<T>>::iterator p;    // initialize iterator p for list of pointers with type T
    p = findPtrInfo(addr);
    p->refcount--;
    p = findPtrInfo(rv.addr);
    p->refcount++;
    addr = rv.addr;

    return rv;
}

template <class T, int size>
bool Pointer<T, size>::collect(){       // Collect garbage. Returns true if at least one object was freed.
    // Note: collect() will be called in the destructor

    bool memFreed = false;
    typename std::list<PtrDetails<T>>::iterator p;

    do {
        for (p = refContainer.begin(); p!= refContainer.end(); p++) {
            if (p->refcount > 0) continue;
            memFreed = true;
            refContainer.remove(*p);

            if (p->memPtr) {
                if (p->isArray) {
                    delete[] p->memPtr;
                } else {
                    delete p->memPtr;
                }
            }
            break;
        }
    } while (p!= refContainer.end());

    return memFreed;
}

template <class T, int size>
void Pointer<T, size>::showlist() {              // A utility function that displays refContainer.
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end()) {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++) {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

template <class T, int size>
typename std::list<PtrDetails<T>>::iterator            // Find a pointer in refContainer.
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)        // Find ptr in refContainer.
        if (p->memPtr == ptr)
            return p;
    return p;
}

template <class T, int size>
void Pointer<T, size>::shutdown(){                      // Clear refContainer when program exits.
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++) {
        p->refcount = 0;            // Set all reference counts to zero
    }
    collect();
}