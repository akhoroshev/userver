#pragma once

#include <string>

namespace utils::statistics {

/// @brief Metric description
///
/// Use `MetricTag<Metric>` for declarative style of metric registration and
/// call `MetricStorage::GetMetric<Metric>()` for accessing metric data. Please
/// note that metrics can be accessed from multiple coroutines, so `Metric` must
/// be thread-safe (e.g. std::atomic<T>, rcu::Variable<T>, rcu::RcuMap<T>,
/// concurrent::Variable<T>, etc.).
///
/// For custom type of `Metric` you have to define method to dump your type to
/// JSON:
///
/// ```
/// formats::json::ValueBuilder DumpMetric(const Metric& m);
/// ```
template <typename Metric>
class MetricTag final {
 public:
  /// Register metric
  explicit MetricTag(const std::string& path);

  std::string GetPath() const { return path_; };

 private:
  const std::string path_;
};

}  // namespace utils::statistics

#include <utils/statistics/metric_tag_impl.hpp>
