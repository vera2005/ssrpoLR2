#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "hw/l2_ApplicationLayer.h"
#include "hw/l4_InfrastructureLayer.h"
class ConsoleOutput : public IOutput {
public:
    void Output(const std::string& s) const override { std::cout << s << std::endl; }
};
std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> args;
    std::istringstream is(line);
    std::string word;
    while (is >> word) args.push_back(word);
    return args;
}
int main() {
    ConsoleOutput out;
    RequestJournal1 journal;
    Application app(out, journal);
    std::string line;
    while (std::getline(std::cin, line)) {
        std::vector<std::string> args = split(line);
        if (!args.empty()) app.performCommand(args);
    }
    return 0;
}
