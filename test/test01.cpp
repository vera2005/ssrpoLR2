#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include "hw/l2_ApplicationLayer.h"
#include "hw/l4_InfrastructureLayer.h"
class TestOutput : public IOutput {
public:
    mutable std::string last;
    void Output(const std::string& s) const override { last = s; }
};
int main() {
    TestOutput out;
    RequestJournal1 journal;
    Application app(out, journal);

    // 3 normal + 3 emergency
    assert(app.performCommand({"add", "passenger", "small"}));
    assert(app.performCommand({"add", "cargo", "medium"}));
    assert(app.performCommand({"add", "military", "small"}));
    assert(app.performCommand({"add", "cargo", "medium", "emergency"}));
    assert(app.performCommand({"add", "military", "medium", "emergency"}));
    assert(app.performCommand({"add", "cargo", "large", "emergency"}));
    assert(app.performCommand({"work"}));

    // РЕФАКТОРИНГ-ПРОВЕРКА: счётчики как в ЛР1 (3/3) — старая функциональность цела
    assert(journal.getInfo().size() == 6);
    std::size_t approved = 0, rejected = 0;
    for (const Request& r : journal.getInfo())
        if (r.getStatus() == RequestStatus::Approved) ++approved; else ++rejected;
    assert(approved == 3);
    assert(rejected == 3);

    // НОВАЯ ФУНКЦИОНАЛЬНОСТЬ: экстренные запросы обработаны первыми
    assert(journal.getInfo()[0].getPriority() == Priority::Emergency);
    assert(journal.getInfo()[1].getPriority() == Priority::Emergency);
    assert(journal.getInfo()[2].getPriority() == Priority::Emergency);
    assert(journal.getInfo()[3].getPriority() == Priority::Normal);
    assert(journal.getInfo()[4].getPriority() == Priority::Normal);
    assert(journal.getInfo()[5].getPriority() == Priority::Normal);
    // isEmergency() — обратная совместимость с ЛР1
    assert(journal.getInfo()[0].isEmergency());
    assert(!journal.getInfo()[3].isEmergency());

    assert(app.performCommand({"view"}));
    assert(!out.last.empty());

    // save -> clean -> load
    assert(app.performCommand({"save", "t1.data"}));
    assert(app.performCommand({"clean"}));
    assert(journal.getInfo().empty());
    assert(app.performCommand({"load", "t1.data"}));
    assert(journal.getInfo().size() == 6);
    assert(journal.getInfo()[0].getPriority() == Priority::Emergency);

    std::cout << "Test 01 passed!\n";
    return 0;
}
