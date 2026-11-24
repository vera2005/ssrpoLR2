#include "hw/l2_ApplicationLayer.h"
#include <algorithm>
#include <cassert>
#include <chrono>
namespace { const std::string DEFAULT_FILE = "requests.data"; }
Application::Application(const IOutput& out, RequestJournal1& journal)
    : _out(out), _journal(journal) {}
bool Application::performCommand(const std::vector<std::string>& args) {
    assert(!args.empty());
    const std::string& c = args[0];
    if (c == "add")   return add(args);
    if (c == "work")  return work(args);
    if (c == "view")  return view(args);
    if (c == "clean") return clean(args);
    if (c == "save")  return save(args);
    if (c == "load")  return load(args);
    return error("unknown command '" + c + "'");
}
bool Application::add(const std::vector<std::string>& args) {
    if (args.size() < 3 || args.size() > 4)
        return error("usage: add <type> <size> [emergency]");
    ShipType type;
    if (args[1] == "passenger")      type = ShipType::Passenger;
    else if (args[1] == "cargo")     type = ShipType::Cargo;
    else if (args[1] == "military")  type = ShipType::Military;
    else return error("unknown type '" + args[1] + "'");
    ShipSize size;
    if (args[2] == "small")       size = ShipSize::Small;
    else if (args[2] == "medium") size = ShipSize::Medium;
    else if (args[2] == "large")  size = ShipSize::Large;
    else return error("unknown size '" + args[2] + "'");
    const Priority pr = (args.size() == 4 && args[3] == "emergency")
        ? Priority::Emergency : Priority::Normal;
    _pending.push_back(Pending{Ship(type, size), pr, std::chrono::system_clock::now()});
    _out.Output("Request registered");
    return true;
}
bool Application::work(const std::vector<std::string>& args) {
    if (args.size() != 1) return error("usage: work");
    // НОВАЯ ФУНКЦИОНАЛЬНОСТЬ: экстренные запросы обрабатываются первыми.
    std::stable_sort(_pending.begin(), _pending.end(),
        [](const Pending& a, const Pending& b) {
            return a.priority == Priority::Emergency && b.priority == Priority::Normal;
        });
    for (const Pending& p : _pending) {
        const TimePoint resp = std::chrono::system_clock::now();
        const RequestStatus st = checkCanLand(p.ship, p.priority)
            ? RequestStatus::Approved : RequestStatus::Rejected;
        _journal.addRequest(Request(p.ship, p.time, resp, st, p.priority));
    }
    _pending.clear();
    _out.Output("Requests processed");
    return true;
}
bool Application::view(const std::vector<std::string>& args) {
    if (args.size() != 1) return error("usage: view");
    const ReportSystem1 report(_journal);
    _out.Output(report.makeReport());
    return true;
}
bool Application::clean(const std::vector<std::string>& args) {
    if (args.size() != 1) return error("usage: clean");
    _journal.clean();
    _pending.clear();
    _out.Output("Collection cleaned");
    return true;
}
bool Application::save(const std::vector<std::string>& args) {
    if (args.size() > 2) return error("usage: save [file]");
    const std::string f = args.size() == 2 ? args[1] : DEFAULT_FILE;
    if (!_journal.saveCollection(f)) return error("cannot save to '" + f + "'");
    _out.Output("Collection saved to '" + f + "'");
    return true;
}
bool Application::load(const std::vector<std::string>& args) {
    if (args.size() > 2) return error("usage: load [file]");
    const std::string f = args.size() == 2 ? args[1] : DEFAULT_FILE;
    if (!_journal.loadCollection(f)) return error("cannot load from '" + f + "'");
    _out.Output("Loaded " + std::to_string(_journal.getInfo().size()) + " items from '" + f + "'");
    return true;
}
bool Application::error(const std::string& msg) {
    _out.Output("Error: " + msg);
    return false;
}
