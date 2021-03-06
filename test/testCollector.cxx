#include "../include/Monitoring/MonitoringFactory.h"
#include <chrono>
#include <vector>

#define BOOST_TEST_MODULE Test Monitoring Colletor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace AliceO2 {
namespace Monitoring {
namespace Test {

using Monitoring = AliceO2::Monitoring::MonitoringFactory;

BOOST_AUTO_TEST_CASE(createCollector)
{
  auto collector = Monitoring::Get("infologger://");

  int intMetric = 10; 
  std::string stringMetric("monitoringString");
  double doubleMetric = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

  collector->send(intMetric, "myCrazyMetricI");
  collector->send(stringMetric, "myCrazyMetricS");
  collector->send(doubleMetric, "myCrazyMetricD");
}

BOOST_AUTO_TEST_CASE(testSymbols)
{
  BOOST_WARN_MESSAGE(!BOOST_IS_DEFINED( _WITH_APPMON ), "ApMon Backend disabled");
  BOOST_WARN_MESSAGE(!BOOST_IS_DEFINED( _WITH_INFLUX ), "InfluxDB Backend disabled");
  BOOST_WARN_MESSAGE(!BOOST_IS_DEFINED( _WITH_FLUME ), "Flume backend disabled");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_LINUX ), "Linux OS detected");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_WINDOWS ), "Windows OS detected");
  BOOST_WARN_MESSAGE(BOOST_IS_DEFINED( _OS_MAC ), "Mac OS detected");
}

} // namespace Test
} // namespace Monitoring
} // namespace AliceO2
