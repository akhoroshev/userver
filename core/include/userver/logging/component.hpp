#pragma once

/// @file userver/logging/component.hpp
/// @brief @copybrief components::Logging

#include <string>
#include <unordered_map>

#include <userver/alerts/storage.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/components/raw_component_base.hpp>
#include <userver/concurrent/async_event_source.hpp>
#include <userver/os_signals/component.hpp>

#include <userver/rcu/rcu_map.hpp>
#include <userver/utils/fast_pimpl.hpp>
#include <userver/utils/periodic_task.hpp>
#include <userver/utils/statistics/entry.hpp>
#include <userver/utils/statistics/writer.hpp>

#include "logger.hpp"

USERVER_NAMESPACE_BEGIN

namespace logging::impl {
class TpLogger;
class TcpSocketSink;
}  // namespace logging::impl

namespace components {

// clang-format off

/// @ingroup userver_components
///
/// @brief %Logging component
///
/// Allows to configure the default logger and/or additional loggers for your
/// needs.
///
/// ## Static options:
/// Name | Description | Default value
/// ---- | ----------- | -------------
/// file_path | path to the log file | -
/// level | log verbosity | info
/// format | log output format, one of `tskv`, `ltsv`, `json`, `json_yadeploy` | tskv
/// flush_level | messages of this and higher levels get flushed to the file immediately | warning
/// message_queue_size | the size of internal message queue, must be a power of 2 | 65536
/// overflow_behavior | message handling policy while the queue is full: `discard` drops messages, `block` waits until message gets into the queue | discard
/// testsuite-capture | if exists, setups additional TCP log sink for testing purposes | {}
/// fs-task-processor | task processor for disk I/O operations for this logger | fs-task-processor of the loggers component
///
/// ### Logs output
/// You can specify logger output, in `file_path` option:
/// - Use `@stdout` to write your logs to standard output stream;
/// - Use `@stderr` to write your logs to standard error stream;
/// - Use `@null` to suppress sending of logs;
/// - Use `%file_name%` to write your logs in file. Use USR1 signal or `OnLogRotate` handler to reopen files after log rotation;
/// - Use `unix:%socket_name%` to write your logs to unix socket. Socket must be created before the service starts and closed by listener after service is shut down.
///
/// ### testsuite-capture options:
/// Name | Description | Default value
/// ---- | ----------- | -------------
/// host | testsuite hostname, e.g. localhost | -
/// port | testsuite port | -
///
/// ## Static configuration example:
///
/// @snippet components/common_component_list_test.cpp Sample logging component config
///
/// `default` section configures the default logger for LOG_*.

// clang-format on

class Logging final : public RawComponentBase {
public:
    /// @ingroup userver_component_names
    /// @brief The default name of components::Logging component
    static constexpr std::string_view kName = "logging";

    /// The component constructor
    Logging(const ComponentConfig&, const ComponentContext&);
    ~Logging() override;

    /// @brief Returns a logger by its name
    /// @param name Name of the logger
    /// @returns Pointer to the Logger instance
    /// @throws std::runtime_error if logger with this name is not registered
    logging::LoggerPtr GetLogger(const std::string& name);

    /// @brief Returns a text logger by its name
    /// @param name Name of the logger
    /// @returns Pointer to the Logger instance
    /// @throws std::runtime_error if logger with this name is not registered
    /// @throws std::runtime_error if logger is not a text logger
    logging::TextLoggerPtr GetTextLogger(const std::string& name);

    /// @brief Sets a logger
    /// @param name Name of the logger
    /// @param logger Logger to set
    void SetLogger(const std::string& name, logging::LoggerPtr logger);

    /// @brief Returns a logger by its name
    /// @param name Name of the logger
    /// @returns Pointer to the Logger instance, or `nullptr` if not registered
    logging::LoggerPtr GetLoggerOptional(const std::string& name);

    void StartSocketLoggingDebug(const std::optional<logging::Level>& log_level);
    void StopSocketLoggingDebug(const std::optional<logging::Level>& log_level);

    /// Reopens log files after rotation
    void OnLogRotate();
    void TryReopenFiles();

    void WriteStatistics(utils::statistics::Writer& writer) const;

    static yaml_config::Schema GetStaticConfigSchema();

private:
    void Init(const ComponentConfig&, const ComponentContext&);
    void Stop() noexcept;

    void FlushLogs();

    engine::TaskProcessor* fs_task_processor_{nullptr};
    std::unordered_map<std::string, std::shared_ptr<logging::impl::TpLogger>> loggers_;
    rcu::RcuMap<std::string, logging::LoggerPtr> extra_loggers_;
    utils::PeriodicTask flush_task_;
    logging::impl::TcpSocketSink* socket_sink_{nullptr};
    alerts::Storage& alert_storage_;

    // Subscriptions must be the last fields.
    os_signals::Subscriber signal_subscriber_;
    utils::statistics::Entry statistics_holder_;
};

template <>
inline constexpr bool kHasValidate<Logging> = true;

}  // namespace components

USERVER_NAMESPACE_END
