#ifndef TESTS_HPP
#define TESTS_HPP

#include "BinarySearchTreeTests.hpp"
#include "TypeTests.hpp"

inline int RunTests() {
    int Failed = 0;
    Failed += run_binary_search_tree_tests();
    Failed += run_type_tests();
    std::cout << "\nВсего проваленных проверок: " << Failed << "\n";
    return Failed;
}

#endif // TESTS_HPP
