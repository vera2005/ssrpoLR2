#ifndef HW_L4_INFRASTRUCTURELAYER_H_
#define HW_L4_INFRASTRUCTURELAYER_H_
#include "hw/l3_DomainLayer.h"
#include <string>
#include <vector>
class IRequestJournal {
public:
    virtual ~IRequestJournal() = default;
    virtual void addRequest(const Request& request) = 0;
    virtual const std::vector<Request>& getInfo() const = 0;
    virtual void clean() = 0;
    virtual bool loadCollection(const std::string& file_name) = 0;
    virtual bool saveCollection(const std::string& file_name) const = 0;
};
class RequestJournal1 : public IRequestJournal {
public:
    RequestJournal1();
    void addRequest(const Request& request) override;
    const std::vector<Request>& getInfo() const override;
    void clean() override;
    bool loadCollection(const std::string& file_name) override;
    bool saveCollection(const std::string& file_name) const override;
private:
    bool checkInvariant() const;
    std::vector<Request> _requests;
};
class IReportSystem {
public:
    virtual ~IReportSystem() = default;
    virtual std::string makeReport() const = 0;
};
class ReportSystem1 : public IReportSystem {
public:
    explicit ReportSystem1(const IRequestJournal& request_journal);
    std::string makeReport() const override;
private:
    bool checkInvariant() const;
    const IRequestJournal& _request_journal;
};
#endif
