///
/// \file DerivedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///
#include "Monitoring/DerivedMetrics.h"
#include "Exceptions/MonitoringInternalException.h"
#include <boost/lexical_cast.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "MonLogger.h"
#include "VariantVisitorRate.h"

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring 
{

DerivedMetrics::DerivedMetrics(const unsigned int cacheSize) : mMaxVectorSize(cacheSize) 
{
}

void DerivedMetrics::registerMetric(std::string name, DerivedMetricMode mode)
{
  mRegistered.emplace(std::pair<std::string, DerivedMetricMode>(name, mode));
  MonLogger::Get() << "Monitoring : Metric " << name << " added to derived metrics" 
                       << MonLogger::End();
}

bool DerivedMetrics::isRegistered(std::string name)
{
  auto search = mRegistered.find(name);
  return (search != mRegistered.end());
}

Metric DerivedMetrics::calculateRate(std::string name)
{
  auto search = mCache.find(name);
  int size = search->second.size();
  if (search == mCache.end() || size < 2) {
    throw MonitoringInternalException("DerivedMetrics/Calculate rate", "Not enough values");
  }

  auto timestampDifference = std::chrono::duration_cast<std::chrono::milliseconds>(
    search->second.at(size - 1).getTimestamp()
    - search->second.at(size - 2).getTimestamp()
  );
  boost::variant< int, std::string, double, uint64_t > current = search->second.at(size - 1).getValue();
  boost::variant< int, std::string, double, uint64_t > previous = search->second.at(size - 2).getValue();
  int timestampCount = timestampDifference.count();

  // disallow dividing by 0
  if (timestampCount == 0) {
    throw MonitoringInternalException("DerivedMetrics/Calculate rate", "Division by 0");
  }

  auto rate =  boost::apply_visitor(VariantVisitorRate(timestampCount), current, previous);
  return Metric{rate, name + "Rate"};
}

Metric DerivedMetrics::calculateAverage(std::string name)
{
  double total = 0;
  for (auto& m : mCache.at(name)) {
    total += boost::lexical_cast<double>(m.getValue());
  }
  double average = total / mCache.at(name).size();
  return Metric{average, name + "Average"};
}

Metric DerivedMetrics::processMetric(Metric& metric)
{
  std::string name = metric.getName();
  if (metric.getType() == MetricType::STRING) {
    throw MonitoringInternalException("DerivedMetrics/ProcessMetric", "Not able to process string values");
  }
  auto search = mCache.find(name);
  // create vector if this is first metric of this kind
  if (search == mCache.end()) {
    mCache.emplace(std::make_pair(name, std::vector<Metric>()));
  }
  // remove first value if vector too large
  if (mCache.at(name).size() > mMaxVectorSize) {
    mCache.at(name).erase( mCache.at(name).begin() );
  }
  mCache[name].push_back(metric);

  auto derived = mRegistered.find(name);
  if (derived->second == DerivedMetricMode::RATE) {
    return calculateRate(name);
  }
  else if (derived->second == DerivedMetricMode::AVERAGE)  {
    return calculateAverage(name);
  } else {
    throw MonitoringInternalException("DerivedMetrics/ProcessMetric", "Processing mode not supported");
  }
}

} // namespace Monitoring
} // namespace AliceO2
