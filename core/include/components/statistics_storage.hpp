#pragma once

#include "component_base.hpp"

#include <components/manager.hpp>
#include <utils/statistics/metrics_storage.hpp>
#include <utils/statistics/storage.hpp>

namespace components {

class StatisticsStorage final : public LoggableComponentBase {
 public:
  static constexpr auto kName = "statistics-storage";

  StatisticsStorage(const ComponentConfig& config,
                    const components::ComponentContext&);

  ~StatisticsStorage() override;

  void OnAllComponentsLoaded() override;

  utils::statistics::Storage& GetStorage() { return storage_; }

  const utils::statistics::Storage& GetStorage() const { return storage_; }

  utils::statistics::MetricsStorage& GetMetricsStorage() {
    return metrics_storage_;
  }

 private:
  formats::json::ValueBuilder ExtendStatistics(
      const utils::statistics::StatisticsRequest&);

  utils::statistics::Storage storage_;
  utils::statistics::MetricsStorage metrics_storage_;
  utils::statistics::Entry statistics_holder_;
};

}  // namespace components
