#include "gc_pointer.h"
#include "LeakTester.h"

int main() {

    // UNIT TESTS

    std::cout << "\n";
    std::cout << "---Testing *t constructor, no array, new element---" << std::endl;
    Pointer<int> p = new int(19);
    Pointer<int>::showlist();

    std::cout << "---Testing *t constructor, array, new element---" << std::endl;
    Pointer<int, 5> p2 = new int[5];
    Pointer<int>::showlist();
    Pointer<int, 5>::showlist();

    std::cout << "---Testing operator = override, no array---" << std::endl;
    p = new int(28);
    Pointer<int>::showlist();
    Pointer<int, 5>::showlist();

    std::cout << "---Testing operator = override, array---" << std::endl;
    p2 = new int[5];
    Pointer<int>::showlist();
    Pointer<int, 5>::showlist();

    std::cout << "---Testing &ob constructor, no array---" << std::endl;
    Pointer<int> p3 = p;
    Pointer<int>::showlist();
    Pointer<int, 5>::showlist();

    std::cout << "---Testing &ob constructor, array---" << std::endl;
    Pointer<int, 5> p4 = p2;
    Pointer<int>::showlist();
    Pointer<int, 5>::showlist();

    return 0;
}