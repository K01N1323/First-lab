#include "Menu.hpp"

#include <iostream>
#include <limits>
#include <exception>
#include <string>
#include <cmath>
#include <type_traits>

#include "Include/BinarySearchTree.hpp"
#include "Tests/Tests.hpp"
#include "Tests/PerformanceTests.hpp"

// Чтение одного целого числа.
// false означает конец ввода или ошибку потока.
static bool ReadInt(const char* Prompt, int& Value) {
    while (true) {
        std::cout << Prompt << std::flush;

        int NewValue = 0;

        if (!(std::cin >> NewValue)) {
            if (std::cin.eof() || std::cin.bad()) {return false;}

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Ошибка: введите одно целое число в диапазоне int.\n";
            continue;
        }

        bool ExtraCharacters = false;
        char Symbol = '\0';

        while (std::cin.get(Symbol)) {
            if (Symbol == '\n') {break;}

            if (Symbol != ' ' && Symbol != '\t' && Symbol != '\r') {
                ExtraCharacters = true;
            }
        }

        if (std::cin.bad()) {return false;}

        if (ExtraCharacters) {
            std::cout << "Ошибка: в строке должно быть только одно целое число.\n";

            if (std::cin.eof()) {return false;}

            continue;
        }

        Value = NewValue;
        return true;
    }
}

static bool ReadChoice(const char* Prompt, int MinValue, int MaxValue, int& Choice) {
    while (ReadInt(Prompt, Choice)) {
        if (Choice >= MinValue && Choice <= MaxValue) {return true;}

        std::cout << "Выберите число от " << MinValue << " до " << MaxValue << ".\n";
    }

    return false;
}

static bool ReadValue(const char* Prompt, int& Value) {return ReadInt(Prompt, Value);}

static bool ReadValue(const char* Prompt, double& Value) {
    while (true) {
        std::cout << Prompt << std::flush;
        double NewValue = 0.0;
        if (!(std::cin >> NewValue)) {
            if (std::cin.eof() || std::cin.bad()) {return false;}
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка: введите одно вещественное число.\n";
            continue;
        }

        bool ExtraCharacters = false;
        char Symbol = '\0';
        while (std::cin.get(Symbol)) {
            if (Symbol == '\n') {break;}
            if (Symbol != ' ' && Symbol != '\t' && Symbol != '\r') {ExtraCharacters = true;}
        }
        if (std::cin.bad()) {return false;}
        if (ExtraCharacters) {
            std::cout << "Ошибка: в строке должно быть одно вещественное число.\n";
            if (std::cin.eof()) {return false;}
            continue;
        }
        Value = NewValue;
        return true;
    }
}

static bool ReadValue(const char* Prompt, std::string& Value) {
    std::cout << Prompt << std::flush;
    return static_cast<bool>(std::getline(std::cin, Value));
}

template <typename T>
static bool CompareValues(const T& first, const T& second) {return first < second;}

template <typename T>
static void ShowTree(const BinarySearchTree<T>& tree) {
    std::cout << "Элементов в дереве: " << tree.GetCount() << "\n";
    tree.Print(std::cout);
}

template <typename T>
static bool ReadTree(BinarySearchTree<T>& tree) {
    int Count = 0;
    if (!ReadChoice("Количество элементов: ", 0, std::numeric_limits<int>::max(), Count)) {return false;}
    BinarySearchTree<T> result(CompareValues<T>);
    for (int Number = 0; Number < Count;) {
        T Value{};
        if (!ReadValue("Элемент: ", Value)) {return false;}
        try {
            result.Insert(Value);
            Number++;
        } catch (const std::invalid_argument& Error) {
            std::cout << "Ошибка: " << Error.what() << ". Введите другой элемент.\n";
        }
    }
    tree = std::move(result);
    std::cout << "Дерево заполнено.\n";
    return true;
}

template <typename T>
static bool GenerateTree(BinarySearchTree<T>& tree) {
    int Mode = 0;
    int Count = 0;
    int Seed = 0;
    if (!ReadChoice("Порядок вставки: случайный (1), упорядоченный (2): ", 1, 2, Mode)) {return false;}
    int MaxCount = 1000000;
    std::cout << "Предельное количество в этом режиме: " << MaxCount << "\n";
    if (!ReadChoice("Количество элементов: ", 0, MaxCount, Count)) {return false;}
    if (!ReadChoice("Seed (неотрицательный): ", 0, std::numeric_limits<int>::max(), Seed)) {return false;}

    T MinValue{};
    T MaxValue{};
    std::string Prefix;
    if constexpr (std::is_same<T, std::string>::value) {
        if (!ReadValue("Префикс строк: ", Prefix)) {return false;}
    } else {
        if (!ReadValue("Минимальное значение: ", MinValue)) {return false;}
        if (!ReadValue("Максимальное значение: ", MaxValue)) {return false;}
        if (MinValue > MaxValue) {throw std::invalid_argument("Минимум больше максимума");}
        if constexpr (std::is_same<T, int>::value) {
            if (static_cast<long long>(MaxValue) - MinValue + 1 < Count) {
                throw std::invalid_argument("В диапазоне недостаточно различных целых чисел");
            }
        }
    }

    DynamicArray<int> items(Count);
    for (int index = 0; index < Count; index++) {items.Set(index, index);}
    if (Mode == 1) {ShufflePerformanceItems(items, static_cast<unsigned int>(Seed));}
    BinarySearchTree<T> result(CompareValues<T>);
    int Width = static_cast<int>(std::to_string(Count).size());
    for (int index = 0; index < Count; index++) {
        int Number = items.Get(index);
        T Value{};
        if constexpr (std::is_same<T, std::string>::value) {
            std::ostringstream stream;
            stream << Prefix << std::setw(Width) << std::setfill('0') << Number;
            Value = stream.str();
        } else if constexpr (std::is_same<T, int>::value) {
            long long Span = static_cast<long long>(MaxValue) - MinValue;
            Value = static_cast<int>(MinValue + (Count <= 1 ? 0 : Span * Number / (Count - 1)));
        } else {
            double Ratio = Count <= 1 ? 0.0 : static_cast<double>(Number) / (Count - 1);
            Value = MinValue * (1.0 - Ratio) + MaxValue * Ratio;
        }
        result.Insert(Value);
    }
    tree = std::move(result);
    std::cout << "Дерево сгенерировано; значения равномерно распределены по диапазону.\n";
    return true;
}

template <typename T>
static T NegateValue(const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {
        return value + "!";
    } else {
        if constexpr (std::is_same<T, int>::value) {
            if (value == std::numeric_limits<int>::min()) {throw std::overflow_error("Переполнение при смене знака");}
        }
        return -value;
    }
}

template <typename T>
static T DoubleTreeValue(const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {
        return "слово: " + value;
    } else {
        if constexpr (std::is_same<T, int>::value) {
            if (value > std::numeric_limits<int>::max() / 2 || value < std::numeric_limits<int>::min() / 2) {
                throw std::overflow_error("Переполнение при удвоении");
            }
        }
        T result = value * 2;
        return result;
    }
}

template <typename T>
static T SquareTreeValue(const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {
        return value + value;
    } else if constexpr (std::is_same<T, int>::value) {
        long long result = static_cast<long long>(value) * value;
        if (result > std::numeric_limits<int>::max()) {throw std::overflow_error("Переполнение при возведении в квадрат");}
        return static_cast<int>(result);
    } else {
        return value * value;
    }
}

template <typename T>
static bool PositiveValue(const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {return !value.empty();}
    else {return value > 0;}
}

template <typename T>
static bool SelectedValue(const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {return value.size() >= 5;}
    else if constexpr (std::is_same<T, int>::value) {return value % 2 == 0;}
    else {return value == std::trunc(value);}
}

template <typename T>
static bool NegativeValue(const T& value) {
    if constexpr (std::is_same<T, std::string>::value) {return value.find(' ') != std::string::npos;}
    else {return value < 0;}
}

template <typename T>
static double SumValues(const double& result, const T& value) {
    double NewValue = result;
    if constexpr (std::is_same<T, std::string>::value) {NewValue += static_cast<double>(value.size());}
    else {NewValue += value;}
    return NewValue;
}

template <typename T>
static std::string JoinValues(const std::string& result, const T& value) {
    std::ostringstream stream;
    stream << result;
    if (!result.empty()) {stream << " | ";}
    stream << value;
    return stream.str();
}

template <typename T>
static long long CountValues(const long long& result, const T&) {return result + 1;}

static void ShowTestResult(int Failed) {
    if (Failed == 0) {
        std::cout << "\nВсе выполненные проверки прошли.\n";
    } else {
        std::cout << "\nЗарегистрировано ошибок: " << Failed << ". Подробности выше.\n";
    }
}

static bool RunPerformanceDemo(bool& HadFailures) {
    PerformanceOptions options;
    if (!ReadChoice("Максимальный размер (1–100000000): ", 1, 100000000, options.MaxCount)) {return false;}
    if (!ReadChoice("Предел упорядоченной вставки (0 — пропустить): ", 0, options.MaxCount, options.OrderedLimit)) {return false;}
    if (!ReadChoice("Повторов (1–20): ", 1, 20, options.Repeats)) {return false;}
    std::string Path;
    if (!ReadValue("Путь CSV (пустая строка — docs/performance-avl.csv): ", Path)) {return false;}
    if (!Path.empty()) {options.CsvPath = Path;}
    if (run_performance_tests(options) != 0) {HadFailures = true;}
    std::cout << "Для графиков: python3 Tools/PlotPerformance.py путь_к_CSV docs/graphs\n";
    return true;
}

template <typename T>
static bool RunTreeDemo(bool& HadFailures) {
    BinarySearchTree<T> tree(CompareValues<T>);
    BinarySearchTree<T> second(CompareValues<T>);
    std::cout << "Балансировка AVL выполняется после каждой вставки и удаления.\n";

    while (true) {
        std::cout << "\n--- Состояние дерева ---\n";
        ShowTree(tree);
        std::cout << "\n1. Заполнить вручную\n"
                  << "2. Сгенерировать\n"
                  << "3. Показать дерево\n"
                  << "4. Вставить элемент\n"
                  << "5. Найти элемент\n"
                  << "6. Удалить элемент\n"
                  << "7. Очистить дерево\n"
                  << "8. Map\n"
                  << "9. Where\n"
                  << "10. Reduce (ЛКП)\n"
                  << "11. Извлечь поддерево\n"
                  << "12. Задать второе дерево\n"
                  << "13. Проверить вхождение второго дерева\n"
                  << "14. Сохранить в строку\n"
                  << "15. Прочитать строку {К}(Л)[П]\n"
                  << "16. Сравнить и объединить два дерева\n"
                  << "17. Все тесты корректности\n"
                  << "18. Измерения производительности\n"
                  << "0. Вернуться к выбору типа\n";
        int Choice = 0;
        if (!ReadChoice("Ваш выбор: ", 0, 18, Choice)) {return false;}
        if (Choice == 0) {return true;}
        if (Choice == 3) {continue;}
        try {
            if (Choice == 1) {
                if (!ReadTree(tree)) {return false;}
            } else if (Choice == 2) {
                if (!GenerateTree(tree)) {return false;}
            } else if (Choice >= 4 && Choice <= 6) {
                T Value{};
                if (!ReadValue("Элемент: ", Value)) {return false;}
                if (Choice == 4) {tree.Insert(Value); std::cout << "Элемент вставлен.\n";}
                else if (Choice == 5) {
                    std::cout << (tree.Contains(Value) ? "Элемент найден.\n" : "Элемент не найден.\n");
                } else {tree.Remove(Value); std::cout << "Элемент удалён.\n";}
            } else if (Choice == 7) {
                tree.Clear();
                std::cout << "Дерево очищено.\n";
            } else if (Choice == 8) {
                if constexpr (std::is_same<T, std::string>::value) {
                    std::cout << "1. Добавить суффикс «!»\n2. Добавить префикс «слово: »\n3. Повторить строку дважды\n";
                } else {
                    std::cout << "1. Сменить знак\n2. Удвоить\n3. Возвести в квадрат\n";
                }
                int Function = 0;
                if (!ReadChoice("Функция: ", 1, 3, Function)) {return false;}
                T (*mapper)(const T&) = Function == 1 ? NegateValue<T> : Function == 2 ? DoubleTreeValue<T> : SquareTreeValue<T>;
                BinarySearchTree<T> result = tree.Map(mapper);
                std::cout << "Результат Map:\n";
                ShowTree(result);
            } else if (Choice == 9) {
                if constexpr (std::is_same<T, std::string>::value) {
                    std::cout << "1. Непустые\n2. Не менее 5 байтов\n3. С пробелом\n";
                } else if constexpr (std::is_same<T, int>::value) {
                    std::cout << "1. Положительные\n2. Чётные\n3. Отрицательные\n";
                } else {
                    std::cout << "1. Положительные\n2. Целые по значению\n3. Отрицательные\n";
                }
                int Function = 0;
                if (!ReadChoice("Предикат: ", 1, 3, Function)) {return false;}
                bool (*where)(const T&) = Function == 1 ? PositiveValue<T> : Function == 2 ? SelectedValue<T> : NegativeValue<T>;
                BinarySearchTree<T> result = tree.Where(where);
                std::cout << "Результат Where:\n";
                ShowTree(result);
            } else if (Choice == 10) {
                std::cout << "1. Сумма чисел (для строк — число байтов)\n2. Соединить значения\n3. Посчитать элементы\n";
                int Function = 0;
                if (!ReadChoice("Свёртка: ", 1, 3, Function)) {return false;}
                if (Function == 1) {
                    double StartValue = 0.0;
                    if (!ReadValue("Начальное число: ", StartValue)) {return false;}
                    std::cout << "Результат Reduce: " << tree.Reduce(SumValues<T>, StartValue) << "\n";
                } else if (Function == 2) {
                    std::string StartValue;
                    if (!ReadValue("Начальная строка: ", StartValue)) {return false;}
                    std::string result = tree.Reduce(JoinValues<T>, StartValue);
                    std::cout << "Результат Reduce: " << result.substr(0, 4000);
                    if (result.size() > 4000) {std::cout << "... ограничение вывода";}
                    std::cout << "\n";
                } else {
                    std::cout << "Результат Reduce: " << tree.Reduce(CountValues<T>, 0LL) << "\n";
                }
            } else if (Choice == 11) {
                T Value{};
                if (!ReadValue("Корень поддерева: ", Value)) {return false;}
                BinarySearchTree<T> result = tree.GetSubtree(Value);
                std::cout << "Извлечённое поддерево:\n";
                ShowTree(result);
            } else if (Choice == 12) {
                int Mode = 0;
                if (!ReadChoice("Второе дерево: вручную (1), генерация (2), фиксированная строка (3): ", 1, 3, Mode)) {return false;}
                if (Mode == 1) {if (!ReadTree(second)) {return false;}}
                else if (Mode == 2) {if (!GenerateTree(second)) {return false;}}
                else {
                    std::string Text;
                    if (!ReadValue("Строка {К}(Л)[П]: ", Text)) {return false;}
                    second.FromString(Text);
                }
                std::cout << "Второе дерево:\n";
                ShowTree(second);
            } else if (Choice == 13 || Choice == 16) {
                std::cout << "Второе дерево:\n";
                ShowTree(second);
                if (Choice == 13) {
                    std::cout << (tree.ContainsSubtree(second) ? "Второе дерево входит как полное поддерево.\n" : "Второе дерево не входит как полное поддерево.\n");
                } else {
                    std::cout << "Равенство (==): " << (tree == second ? "да" : "нет")
                              << "; неравенство (!=): " << (tree != second ? "да" : "нет") << "\n";
                    BinarySearchTree<T> result = tree + second;
                    std::cout << "Объединение (+):\n";
                    ShowTree(result);
                }
            } else if (Choice == 14) {
                std::string Format;
                if (!ReadValue("Формат (пустая строка — {К}(Л)[П]): ", Format)) {return false;}
                if (Format.empty()) {Format = "{К}(Л)[П]";}
                std::string Text = tree.ToString(Format);
                std::cout << "Запись дерева: " << Text.substr(0, 4000);
                if (Text.size() > 4000) {std::cout << "... ограничение вывода; полная запись доступна в файле";}
                std::cout << "\n";
                std::string Path;
                if (!ReadValue("Сохранить в файл (пустая строка — пропустить): ", Path)) {return false;}
                if (!Path.empty()) {
                    std::ofstream file(Path, std::ios::binary);
                    file << Text;
                    if (!file) {throw std::runtime_error("Не удалось сохранить файл");}
                    std::cout << "Запись сохранена: " << Path << "\n";
                }
            } else if (Choice == 15) {
                std::string Text;
                if (!ReadValue("Строка дерева: ", Text)) {return false;}
                tree.FromString(Text);
                std::cout << "Дерево прочитано.\n";
            } else if (Choice == 17) {
                int Failed = RunTests();
                ShowTestResult(Failed);
                if (Failed != 0) {HadFailures = true;}
            } else if (Choice == 18) {
                if (!RunPerformanceDemo(HadFailures)) {return false;}
            }
        } catch (const std::exception& Error) {
            std::cout << "\nОперация прервана: " << Error.what() << "\n";
        }
    }
}

int RunMenu() {
    bool HadFailures = false;
    bool ContinueInput = true;
    while (ContinueInput) {
        std::cout << "\n БИНАРНОЕ ДЕРЕВО ПОИСКА \n"
                  << "1. Дерево int\n"
                  << "2. Дерево double\n"
                  << "3. Дерево std::string\n"
                  << "4. Все тесты корректности\n"
                  << "5. Измерения производительности\n"
                  << "0. Выход\n";
        int Choice = 0;
        if (!ReadChoice("Ваш выбор: ", 0, 5, Choice)) {break;}
        if (Choice == 0) {break;}
        try {
            if (Choice == 1) {ContinueInput = RunTreeDemo<int>(HadFailures);}
            else if (Choice == 2) {ContinueInput = RunTreeDemo<double>(HadFailures);}
            else if (Choice == 3) {ContinueInput = RunTreeDemo<std::string>(HadFailures);}
            else if (Choice == 4) {
                int Failed = RunTests();
                ShowTestResult(Failed);
                if (Failed != 0) {HadFailures = true;}
            } else {ContinueInput = RunPerformanceDemo(HadFailures);}
        } catch (const std::exception& Error) {
            std::cout << "\nОперация прервана: " << Error.what() << "\n";
            HadFailures = true;
        } catch (...) {
            std::cout << "\nОперация прервана неизвестным исключением.\n";
            HadFailures = true;
        }
    }
    std::cout << "\nПрограмма завершена.\n";
    return HadFailures ? 1 : 0;
}
