///
/// \file Backend.cxx
/// \author Dennis Klein <d.klein@gsi.de>
///

#include <Monitoring/Backend.h>
#include "MonLogger.h"

using namespace std;

AliceO2::Monitoring::Backend::Backend(const string name, const BackendVersion version, const string maintainer, const string homepage)
    : fkName{name}
    , fkVersion(version)
    , fkMaintainer{maintainer}
    , fkHomepage{homepage}
{
    MonLogger::Get() << "Loaded Backend: " << *this << MonLogger::End();
}

AliceO2::Monitoring::Backend::~Backend()
{
    MonLogger::Get() << "Unloaded Backend: " << *this << MonLogger::End();
}
