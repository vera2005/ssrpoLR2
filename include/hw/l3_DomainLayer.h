#ifndef HW_L3_DOMAINLAYER_H_
#define HW_L3_DOMAINLAYER_H_
#include <chrono>
using TimePoint = std::chrono::system_clock::time_point;
enum class ShipType { Passenger, Cargo, Military };
enum class ShipSize { Large, Medium, Small };
enum class RequestStatus { Approved, Rejected };
enum class Priority { Normal, Emergency };
class Ship {
public:
    Ship(ShipType type, ShipSize size);
    ShipType getType() const;
    ShipSize getSize() const;
private:
    bool checkInvariant() const;
    ShipType _type;
    ShipSize _size;
};
class Request {
public:
    Request(Ship ship, TimePoint request_time, TimePoint response_time,
            RequestStatus status, Priority priority);
    const Ship& getShip() const;
    TimePoint getRequestTime() const;
    TimePoint getResponseTime() const;
    RequestStatus getStatus() const;
    Priority getPriority() const;
    bool isEmergency() const;
private:
    bool checkInvariant() const;
    Ship _ship;
    TimePoint _request_time;
    TimePoint _response_time;
    RequestStatus _status;
    Priority _priority;
};
bool checkCanLand(const Ship& ship, Priority priority);
#endif
