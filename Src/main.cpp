#include "Ui/Menu.hpp"
#include "Tests/Tests.hpp"
#include "Tests/PerformanceTests.hpp"

#include <iostream>
#include <string>

static int ReadArgument(const char* Text) {
    std::string value(Text);
    std::size_t position = 0;
    int result = std::stoi(value, &position);
    if (position != value.size()) {throw std::invalid_argument("Неверный числовой параметр");}
    return result;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {return RunMenu();}
    if (argc == 2 && std::string(argv[1]) == "--test") {return RunTests() == 0 ? 0 : 1;}
    if (argc >= 2 && std::string(argv[1]) == "--performance") {
        PerformanceOptions options;
        try {
            for (int index = 2; index < argc; index += 2) {
                if (index + 1 == argc) {throw std::invalid_argument("Не задано значение параметра");}
                std::string name(argv[index]);
                if (name == "--max") {options.MaxCount = ReadArgument(argv[index + 1]);}
                else if (name == "--ordered-limit") {options.OrderedLimit = ReadArgument(argv[index + 1]);}
                else if (name == "--repeats") {options.Repeats = ReadArgument(argv[index + 1]);}
                else if (name == "--seed") {
                    int seed = ReadArgument(argv[index + 1]);
                    if (seed < 0) {throw std::invalid_argument("Seed не может быть отрицательным");}
                    options.Seed = static_cast<unsigned int>(seed);
                }
                else if (name == "--csv") {options.CsvPath = argv[index + 1];}
                else {throw std::invalid_argument("Неизвестный параметр измерений");}
            }
            if (options.OrderedLimit > options.MaxCount) {options.OrderedLimit = options.MaxCount;}
            return run_performance_tests(options);
        } catch (const std::exception& Error) {
            std::cerr << "Ошибка: " << Error.what() << "\n";
            return 1;
        }
    }

    std::cerr << "Использование: " << argv[0] << " [--test | --performance [--max N] [--ordered-limit N] [--repeats N] [--seed N] [--csv путь]]\n";
    return 1;
}
