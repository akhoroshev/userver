#pragma once

#include <storages/postgres/options.hpp>
#include <storages/postgres/result_set.hpp>
#include <storages/postgres/transaction.hpp>

#include <engine/task/task_processor.hpp>
#include <storages/postgres/detail/query_parameters.hpp>

#include <string>

namespace storages {
namespace postgres {

enum class ConnectionState {
  kOffline,     //!< Not connected
  kIdle,        //!< Connected, not in transaction
  kTranIdle,    //!< In a valid transaction block, idle
  kTranActive,  //!< In a transaction, processing a SQL statement
  kTranError    //!< In a failed transaction block, idle
};

namespace detail {
/// @brief PostreSQL connection class
/// Handles connecting to Postgres, sending commands, processing command results
/// and closing Postgres connection.
/// Responsible for all asynchronous operations
class Connection {
 public:
  Connection(const Connection&) = delete;
  Connection(Connection&&) = delete;
  ~Connection();
  // clang-format off
  /// Connect to database using conninfo string
  ///
  /// Will suspend current coroutine
  ///
  /// @param conninfo Connection string, @see https://www.postgresql.org/docs/10/static/libpq-connect.html#LIBPQ-CONNSTRING
  /// @param bg_task_processor task processor for blocking operations
  /// @throws ConnectionFailed
  // clang-format on
  static std::unique_ptr<Connection> Connect(
      const std::string& conninfo, engine::TaskProcessor& bg_task_processor);

  /// Close the connection
  /// TODO When called from another thread/coroutine will wait for current
  /// transaction to finish.
  void Close();

  bool IsReadOnly() const;
  /// Get current connection state
  ConnectionState GetState() const;
  /// Check if the connection is active
  bool IsConnected() const;
  /// Check if the connection is currently idle (IsConnected &&
  /// !IsInTransaction)
  bool IsIdle() const;

  //@{
  /// Check if connection is currently in transaction
  bool IsInTransaction() const;
  /** @name Transaction interface */
  /// Begin a transaction in Postgres
  /// Suspends coroutine for execution
  /// @throws AlreadyInTransaction
  void Begin(const TransactionOptions& options);
  /// Commit current transaction
  /// Suspends coroutine for execution
  /// @throws NotInTransaction
  void Commit();
  /// Rollback current transaction
  /// Suspends coroutine for execution
  /// @throws NotInTransaction
  void Rollback();
  //@}

  //@{
  /** @name Command sending interface */
  /// Cancel current operation
  void Cancel();
  ResultSet Execute(const std::string& statement,
                    const detail::QueryParameters& = {});
  template <typename... T>
  ResultSet Execute(const std::string& statement, const T&... args) {
    detail::QueryParameters params;
    params.Write(args...);
    return Execute(statement, params);
  }

  /// @brief Set session parameter
  /// Parameters documentation
  /// https://www.postgresql.org/docs/current/sql-set.html
  void SetParameter(const std::string& param, const std::string& value);

  //@{
  /** @name Command sending interface for experimenting */
  /// Separate method for experimenting with PostgreSQL protocol and parsing
  /// Not visible to users of PostgreSQL driver
  ResultSet ExperimentalExecute(
      const std::string& statement,
      io::DataFormat reply_format = io::DataFormat::kTextDataFormat,
      const detail::QueryParameters& = {});
  template <typename... T>
  ResultSet ExperimentalExecute(const std::string& statement,
                                io::DataFormat reply_format, const T&... args) {
    detail::QueryParameters params;
    params.Write(args...);
    return ExperimentalExecute(statement, reply_format, params);
  }
  //@}
  //@}
 private:
  Connection();

  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};

}  // namespace detail
}  // namespace postgres
}  // namespace storages
