#ifndef PERFORMANCE_TESTS_HPP
#define PERFORMANCE_TESTS_HPP

#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <stdexcept>

#include "Include/BinarySearchTree.hpp"
#include "BaseStructures/DynamicArray.hpp"

struct PerformanceOptions {
    int MaxCount;
    int OrderedLimit;
    int Repeats;
    unsigned int Seed;
    std::string CsvPath;

    PerformanceOptions(): MaxCount(100000), OrderedLimit(100000), Repeats(3), Seed(1323), CsvPath("docs/performance-avl.csv") {}
};

inline bool PerformanceCompareInt(const int& first, const int& second) {return first < second;}

inline void ShufflePerformanceItems(DynamicArray<int>& items, unsigned int Seed) {
    std::mt19937 Generator(Seed);
    for (int index = items.GetSize() - 1; index > 0; index--) {
        std::uniform_int_distribution<int> Distribution(0, index);
        int other = Distribution(Generator);
        int value = items.Get(index);
        items.Set(index, items.Get(other));
        items.Set(other, value);
    }
}

inline void WritePerformanceResult(std::ofstream& csv, int count, const char* order, int repeat,
                                   unsigned int Seed, const char* operation, int operations,
                                   const std::chrono::steady_clock::time_point& begin,
                                   const std::chrono::steady_clock::time_point& end, long long checksum) {
    double milliseconds = std::chrono::duration<double, std::milli>(end - begin).count();
    csv << count << ',' << order << ',' << repeat << ',' << Seed << ',' << operation << ',' << operations
        << ',' << std::setprecision(12) << milliseconds << ',' << checksum << '\n';
    if (!csv) {throw std::runtime_error("Не удалось записать CSV");}
    std::cout << "  " << operation << ": " << milliseconds << " мс\n";
}

inline void MeasureTree(int count, bool Ordered, int repeat, const PerformanceOptions& options, std::ofstream& csv) {
    DynamicArray<int> items(count);
    DynamicArray<int> queries(count);
    for (int index = 0; index < count; index++) {items.Set(index, index); queries.Set(index, index);}
    if (!Ordered) {ShufflePerformanceItems(items, options.Seed + static_cast<unsigned int>(repeat));}
    ShufflePerformanceItems(queries, options.Seed + static_cast<unsigned int>(repeat) + 1);

    const char* order = Ordered ? "ordered" : "random";
    BinarySearchTree<int> tree(PerformanceCompareInt);
    long long expected_sum = static_cast<long long>(count) * (count - 1) / 2;
    std::cout << "\nКоличество: " << count << ", порядок: " << order << ", повтор: " << repeat + 1 << "\n";

    auto begin = std::chrono::steady_clock::now();
    for (int index = 0; index < count; index++) {tree.Insert(items.Get(index));}
    auto end = std::chrono::steady_clock::now();
    if (tree.GetCount() != count) {throw std::runtime_error("Неверный результат вставки");}
    WritePerformanceResult(csv, count, order, repeat, options.Seed, "insert", count, begin, end, tree.GetCount());

    long long checksum = 0;
    int found = 0;
    begin = std::chrono::steady_clock::now();
    for (int index = 0; index < count; index++) {
        const int* value = tree.Find(queries.Get(index));
        if (value != nullptr) {checksum += *value; found++;}
        if (tree.Find(count + index) != nullptr) {found++;}
    }
    end = std::chrono::steady_clock::now();
    if (found != count || checksum != expected_sum) {throw std::runtime_error("Неверный результат поиска");}
    WritePerformanceResult(csv, count, order, repeat, options.Seed, "search", count * 2, begin, end, checksum);

    BinarySearchTree<int> removal(tree);
    begin = std::chrono::steady_clock::now();
    for (int index = 0; index < count; index++) {removal.Remove(queries.Get(index));}
    end = std::chrono::steady_clock::now();
    if (!removal.IsEmpty()) {throw std::runtime_error("Неверный результат удаления");}
    WritePerformanceResult(csv, count, order, repeat, options.Seed, "remove", count, begin, end, removal.GetCount());

    csv.flush();
    if (!csv) {throw std::runtime_error("Не удалось сохранить результаты");}
}

inline int run_performance_tests(const PerformanceOptions& options = PerformanceOptions()) {
    if (options.MaxCount < 1 || options.MaxCount > 100000000 || options.OrderedLimit < 0 || options.OrderedLimit > options.MaxCount ||
        options.Repeats < 1 || options.Repeats > 20) {
        std::cerr << "Ошибка: неверные пределы измерений.\n";
        return 1;
    }
    try {
        std::ofstream csv(options.CsvPath);
        if (!csv) {throw std::runtime_error("Не удалось открыть CSV; проверьте путь и папку");}
        csv << "count,order,repeat,seed,operation,operations,milliseconds,checksum\n";
        int sizes[] = {1000, 3000, 10000, 30000, 100000, 300000, 1000000, 3000000, 10000000, 30000000, 100000000};
        std::cout << "\nИзмерение AVL-дерева; seed = " << options.Seed << "\n";
        for (int count : sizes) {
            if (count > options.MaxCount) {break;}
            for (int repeat = 0; repeat < options.Repeats; repeat++) {
                MeasureTree(count, false, repeat, options, csv);
                if (count <= options.OrderedLimit) {MeasureTree(count, true, repeat, options, csv);}
            }
        }
        bool listed_max = false;
        bool listed_ordered = options.OrderedLimit == 0;
        for (int count : sizes) {
            if (count == options.MaxCount) {listed_max = true;}
            if (count == options.OrderedLimit) {listed_ordered = true;}
        }
        if (!listed_max) {
            for (int repeat = 0; repeat < options.Repeats; repeat++) {
                MeasureTree(options.MaxCount, false, repeat, options, csv);
                if (options.MaxCount <= options.OrderedLimit) {MeasureTree(options.MaxCount, true, repeat, options, csv);}
            }
        }
        if (!listed_ordered && options.OrderedLimit != options.MaxCount) {
            for (int repeat = 0; repeat < options.Repeats; repeat++) {MeasureTree(options.OrderedLimit, true, repeat, options, csv);}
        }
        std::cout << "\nРезультаты сохранены: " << options.CsvPath << "\n";
        return 0;
    } catch (const std::exception& Error) {
        std::cerr << "\nИзмерения прерваны: " << Error.what() << "\n";
        return 1;
    }
}

#endif // PERFORMANCE_TESTS_HPP
