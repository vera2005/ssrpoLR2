#include "hw/l3_DomainLayer.h"
#include <cassert>
#include <utility>
Ship::Ship(ShipType type, ShipSize size) : _type(type), _size(size) {
    assert(checkInvariant());
}
ShipType Ship::getType() const { return _type; }
ShipSize Ship::getSize() const { return _size; }
bool Ship::checkInvariant() const {
    bool t = (_type == ShipType::Passenger || _type == ShipType::Cargo || _type == ShipType::Military);
    bool s = (_size == ShipSize::Large || _size == ShipSize::Medium || _size == ShipSize::Small);
    return t && s;
}
Request::Request(Ship ship, TimePoint request_time, TimePoint response_time,
                 RequestStatus status, Priority priority)
    : _ship(std::move(ship)), _request_time(request_time),
      _response_time(response_time), _status(status), _priority(priority) {
    assert(checkInvariant());
}
const Ship& Request::getShip() const { return _ship; }
TimePoint Request::getRequestTime() const { return _request_time; }
TimePoint Request::getResponseTime() const { return _response_time; }
RequestStatus Request::getStatus() const { return _status; }
Priority Request::getPriority() const { return _priority; }
bool Request::isEmergency() const { return _priority == Priority::Emergency; }
bool Request::checkInvariant() const {
    bool v_status = (_status == RequestStatus::Approved || _status == RequestStatus::Rejected);
    bool v_prio   = (_priority == Priority::Normal || _priority == Priority::Emergency);
    return v_status && v_prio && _response_time >= _request_time;
}
bool checkCanLand(const Ship& ship, Priority priority) {
    const bool is_emergency = (priority == Priority::Emergency);
    const bool small_civil = ship.getSize() == ShipSize::Small && ship.getType() != ShipType::Military;
    const bool emerg_ok = is_emergency && (ship.getSize() == ShipSize::Medium || ship.getType() == ShipType::Military);
    return small_civil || emerg_ok;
}
