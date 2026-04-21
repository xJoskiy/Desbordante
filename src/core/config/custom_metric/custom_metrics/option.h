#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <sstream>
#include <string_view>

#include "core/config/column_index/type.h"
#include "core/config/custom_metric/custom_metrics/type.h"
#include "core/config/descriptions.h"
#include "core/config/exceptions.h"
#include "core/config/names.h"
#include "core/config/option.h"

namespace config {
/// @brief Option for a collection of user-defined metrics
class MetricsOption {
private:
    std::string_view name_;
    std::string_view description_;

    static CustomMetricsType MakeDefaultMetrics(std::size_t indices_count) {
        return CustomMetricsType(indices_count, nullptr);
    }

    static void CheckMetrics(CustomMetricsType const& value, std::size_t indices_count) {
        if (value.size() != indices_count) {
            std::ostringstream msg;
            msg << "Expected " << indices_count << " user-defined metrics, got " << value.size();
            throw ConfigurationError(msg.str());
        }
    }

    /// User can pass @c nullptr to use the default metric explicitly
    static void NormalizeMetrics(CustomMetricsType& value) {
        auto default_metric = std::make_shared<util::DefaultCustomMetric>();

        std::ranges::replace_if(value, std::logical_not{}, default_metric);
    }

public:
    MetricsOption(std::string_view name = names::kCustomMetrics,
                  std::string_view description = descriptions::kDCustomMetrics)
        : name_(name), description_(description) {}

    // NOTE: This option should depend on indices option (see @c SetConditionalOpts)
    // to properly get column count
    [[nodiscard]] Option<CustomMetricsType> operator()(
            CustomMetricsType* value_ptr, std::function<IndexType()> get_col_count) const;
};
}  // namespace config
