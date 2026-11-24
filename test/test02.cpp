#include <cassert>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include "hw/l2_ApplicationLayer.h"
#include "hw/l4_InfrastructureLayer.h"
class TestOutput : public IOutput {
public:
    void Output(const std::string&) const override {}
};
static void expectAbort(void (*action)()) {
    pid_t pid = fork();
    if (pid == 0) { action(); std::_Exit(EXIT_SUCCESS); }
    int st = 0;
    waitpid(pid, &st, 0);
    assert(WIFSIGNALED(st) && WTERMSIG(st) == SIGABRT);
}
static void badType()   { Ship(static_cast<ShipType>(100), ShipSize::Small); }
static void badSize()   { Ship(ShipType::Cargo, static_cast<ShipSize>(100)); }
static void badTime() {
    Ship s(ShipType::Cargo, ShipSize::Small);
    Request(s, TimePoint(std::chrono::seconds(2)), TimePoint(std::chrono::seconds(1)),
            RequestStatus::Approved, Priority::Normal);
}
static void badStatus() {
    Ship s(ShipType::Cargo, ShipSize::Small);
    Request(s, TimePoint(std::chrono::seconds(1)), TimePoint(std::chrono::seconds(1)),
            static_cast<RequestStatus>(100), Priority::Normal);
}
static void badPriority() {
    Ship s(ShipType::Cargo, ShipSize::Small);
    Request(s, TimePoint(std::chrono::seconds(1)), TimePoint(std::chrono::seconds(1)),
            RequestStatus::Approved, static_cast<Priority>(100));
}
int main() {
    TestOutput out;
    RequestJournal1 journal;
    Application app(out, journal);
    assert(!app.performCommand({"unknown"}));
    assert(!app.performCommand({"add", "passenger"}));
    assert(!app.performCommand({"add", "foo", "small"}));
    assert(!app.performCommand({"add", "passenger", "huge"}));
    assert(!app.performCommand({"work", "x"}));
    assert(!app.performCommand({"view", "x"}));
    assert(!app.performCommand({"clean", "x"}));
    assert(!app.performCommand({"save", "a", "b"}));
    assert(!app.performCommand({"load", "nonexistent_file_xyz.bin"}));
    expectAbort(badType);
    expectAbort(badSize);
    expectAbort(badTime);
    expectAbort(badStatus);
    expectAbort(badPriority);
    std::cout << "Test 02 passed!\n";
    return 0;
}
