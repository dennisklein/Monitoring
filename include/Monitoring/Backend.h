///
/// \file Backend.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
/// \author Dennis Klein <d.klein@gsi.de>
///

#ifndef ALICEO2_MONITORING_CORE_BACKEND_H
#define ALICEO2_MONITORING_CORE_BACKEND_H

#include "Monitoring/Metric.h"

#include <boost/dll/alias.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>

#include <chrono>
#include <ostream>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{

struct BackendVersion
{
    const int fkMajor, fkMinor, fkPatch;

    friend auto operator< (const BackendVersion& lhs, const BackendVersion& rhs) -> bool { return std::tie(lhs.fkMajor, lhs.fkMinor, lhs.fkPatch) < std::tie(rhs.fkMajor, rhs.fkMinor, rhs.fkPatch); }
    friend auto operator> (const BackendVersion& lhs, const BackendVersion& rhs) -> bool { return rhs < lhs; }
    friend auto operator<=(const BackendVersion& lhs, const BackendVersion& rhs) -> bool { return !(lhs > rhs); }
    friend auto operator>=(const BackendVersion& lhs, const BackendVersion& rhs) -> bool { return !(lhs < rhs); }
    friend auto operator==(const BackendVersion& lhs, const BackendVersion& rhs) -> bool { return std::tie(lhs.fkMajor, lhs.fkMinor, lhs.fkPatch) == std::tie(rhs.fkMajor, rhs.fkMinor, rhs.fkPatch); }
    friend auto operator!=(const BackendVersion& lhs, const BackendVersion& rhs) -> bool { return !(lhs == rhs); }
    friend auto operator<<(std::ostream& os, const BackendVersion& v) -> std::ostream& { return os << v.fkMajor << "." << v.fkMinor << "." << v.fkPatch; }
};

/// Backend pure virtual interface
///
/// Interface that allows to send a metric to remote backend.
/// In addition, default tagset (for all handled metrics) can be created.
class Backend
{
  public:
    /// Default constructor
    Backend() = delete;

    Backend(const std::string name, const BackendVersion version, const std::string maintainer, const std::string homepage);
    Backend(const Backend&) = delete;

    /// Default destructor
    virtual ~Backend();
	
    auto GetName() const -> const std::string& { return fkName; }
    auto GetVersion() const -> const BackendVersion { return fkVersion; }
    auto GetMaintainer() const -> const std::string& { return fkMaintainer; }
    auto GetHomepage() const -> const std::string& { return fkHomepage; }

    friend auto operator==(const Backend& lhs, const Backend& rhs) -> bool { return std::make_tuple(lhs.GetName(), lhs.GetVersion()) == std::make_tuple(rhs.GetName(), rhs.GetVersion()); }
    friend auto operator!=(const Backend& lhs, const Backend& rhs) -> bool { return !(lhs == rhs); }
    friend auto operator<<(std::ostream& os, const Backend& p) -> std::ostream&
    {
        return os << "'" << p.GetName() << "', "
                  << "version '" << p.GetVersion() << "', "
                  << "maintainer '" << p.GetMaintainer() << "', "
                  << "homepage '" << p.GetHomepage() << "'";
    }

    /// Sends metric via backend
    virtual void send(const Metric& metric) = 0;

    /// Sends multiple metrics (if supported), otherwise falls back into sending single metrics
    virtual void sendMultiple(std::string measurement, std::vector<Metric>&& metrics) = 0;

    /// Sets a tag
    virtual void addGlobalTag(std::string name, std::string value) = 0;

  private:
    const std::string fkName;
    const BackendVersion fkVersion;
    const std::string fkMaintainer;
    const std::string fkHomepage;
};

} // namespace Monitoring
} // namespace AliceO2

#define REGISTER_O2MON_BACKEND(KLASS, NAME, VERSION, MAINTAINER, HOMEPAGE) \
static auto Make_##NAME##_Backend() -> std::shared_ptr<AliceO2::Monitoring::Backend> \
{ \
    return std::make_shared<KLASS>(std::string{#NAME}, VERSION, std::string{MAINTAINER}, std::string{HOMEPAGE}); \
} \
BOOST_DLL_ALIAS(Make_##NAME##_Backend, make_##NAME##_backend)

#endif // ALICEO2_MONITORING_CORE_BACKEND_H
