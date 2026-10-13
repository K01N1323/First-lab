#ifndef TYPE_TESTS_HPP
#define TYPE_TESTS_HPP

#include "TestHelpers.hpp"

inline double TestDoubleMap(const double& value) {return -2.0 * value;}
inline bool TestDoubleWhere(const double& value) {return value > 0.0;}
inline double TestDoubleReduce(const double& sum, const double& value) {return sum + value;}
inline std::string TestStringMap(const std::string& value) {return "слово: " + value;}
inline bool TestStringWhere(const std::string& value) {return value.size() >= 3;}
inline std::string TestStringReduce(const std::string& text, const std::string& value) {return text + '[' + value + ']';}
inline int TestStringLength(const int& sum, const std::string& value) {return sum + static_cast<int>(value.size());}

inline void TestDoubleTree(int& tests_passed, int& tests_failed) {
    BinarySearchTree<double> tree(TestTreeCompareDouble);
    tree.Insert(1.5);
    tree.Insert(-2.0);
    tree.Insert(0.25);
    BinarySearchTree<double> mapped = tree.Map(TestDoubleMap);
    check_tree(mapped.Get(0) == -3.0 && mapped.Get(2) == 4.0 && tree.GetCount() == 3,
               "Double: Map меняет порядок", tests_passed, tests_failed);
    check_tree(tree.Where(TestDoubleWhere).GetCount() == 2 && tree.Reduce(TestDoubleReduce, 1.0) == 0.75,
               "Double: Where и Reduce", tests_passed, tests_failed);
    BinarySearchTree<double> copy(TestTreeCompareDouble);
    copy.FromString(tree.ToString());
    check_tree(copy.Contains(1.5) && (tree + copy).GetCount() == 3,
               "Double: ввод, балансировка и объединение", tests_passed, tests_failed);
    copy.Remove(1.5);
    check_tree(tree.Contains(1.5) && !copy.Contains(1.5), "Double: независимое удаление", tests_passed, tests_failed);
}

inline void TestStringTree(int& tests_passed, int& tests_failed) {
    BinarySearchTree<std::string> tree(TestTreeCompareString);
    tree.Insert("bbb");
    tree.Insert("a");
    tree.Insert("cc");
    BinarySearchTree<std::string> mapped = tree.Map(TestStringMap);
    check_tree(mapped.Get(0) == "слово: a" && mapped.GetCount() == 3,
               "Строки: Map", tests_passed, tests_failed);
    check_tree(tree.Where(TestStringWhere).GetCount() == 1 && tree.Reduce(TestStringReduce, std::string()) == "[a][bbb][cc]" &&
                   tree.Reduce(TestStringLength, 0) == 6,
               "Строки: Where, Reduce в строку и в число", tests_passed, tests_failed);
    BinarySearchTree<std::string> subtree = tree.GetSubtree("a");
    check_tree(tree.ContainsSubtree(subtree) && subtree.GetCount() == 1,
               "Строки: извлечение и вхождение", tests_passed, tests_failed);
    BinarySearchTree<std::string> copy(TestTreeCompareString);
    copy.FromString(mapped.ToString());
    check_tree(copy == mapped && (tree + copy).GetCount() == 6,
               "Строки: ввод, сравнение и объединение", tests_passed, tests_failed);
    std::ostringstream output;
    copy.Insert(std::string(1000, 'z'));
    copy.Print(output, 20);
    check_tree(output.str().find(std::string(1000, 'z')) != std::string::npos,
               "Вывод сохраняет полное строковое значение", tests_passed, tests_failed);
}

inline int run_type_tests() {
    int tests_passed = 0;
    int tests_failed = 0;
    std::cout << "\nТестирование нескольких типов\n";
    try {
        TestDoubleTree(tests_passed, tests_failed);
    } catch (const std::exception& Error) {
        check_tree(false, Error.what(), tests_passed, tests_failed);
    }
    try {
        TestStringTree(tests_passed, tests_failed);
    } catch (const std::exception& Error) {
        check_tree(false, Error.what(), tests_passed, tests_failed);
    }
    std::cout << "\nТипы: успешно " << tests_passed << ", провалено " << tests_failed << "\n";
    return tests_failed;
}

#endif // TYPE_TESTS_HPP
