///
/// \file 3-UserDefinedTimestamp.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using Monitoring = AliceO2::Monitoring::MonitoringFactory;
using AliceO2::Monitoring::Metric;

int main() {
  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto collector = Monitoring::Get("infologger://");

  // current timestamp
  std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
	
  // now send an application specific metric
  // 10 is the value timestamp
  // myCrazyMetric is the name of the metric
  collector->send(Metric{10, "myCrazyMetric"}.setTimestamp(timestamp));
  std::this_thread::sleep_for(std::chrono::seconds(1));
  collector->send(Metric{40, "myCrazyMetric"}.setTimestamp(timestamp));
}
