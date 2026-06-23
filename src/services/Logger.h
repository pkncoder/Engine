#pragma once

#include <deque>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Engine {

// Log severity + log printing status
enum class LogLevel { FORMATTING, INFO, WARNING, ERR, FATAL, DEBUG };
enum class LogType { STACKED, IN_PLACE };

// Log entry
struct LogEntry {
    LogLevel level;
    std::string tag;
    std::string message;
};

class Logger {
  public:
    // Init & shutdown
    static void init();
    static void shutdown();

    // Log wrappers
    static void info(std::string_view tag, std::string_view message,
                     LogType type = LogType::STACKED);
    static void warn(std::string_view tag, std::string_view message,
                     LogType type = LogType::STACKED);
    static void error(std::string_view tag, std::string_view message,
                      LogType type = LogType::STACKED);
    static void fatal(std::string_view tag, std::string_view message,
                      LogType type = LogType::STACKED);

    // Debug log
    static inline void check() {
        log(LogLevel::DEBUG, "DEBUG", "--- CHECK ---", LogType::STACKED);
    };

    // Special log wrappers
    static inline void space(LogType type = LogType::STACKED) {
        log(LogLevel::FORMATTING, "NULL", "", type);
    };
    static inline void line(int length = 15, LogType type = LogType::STACKED) {
        std::string line(length, '-');

        log(LogLevel::FORMATTING, "NULL", line, type);
    };

    // Output all logs (ansi)
    static void outputLogs();

    // Set the boolean to print a log right when added
    static inline void setNoPendingLogs(bool newValue) {
        no_periodic_wait = newValue;
    }

    // Get string & ansi color for a specific level
    static const char *getLevelName(LogLevel level);
    static const char *getLevelColor(LogLevel level);

  private:
    // Add a new log to the pending list
    static void log(LogLevel level, std::string_view tag,
                    std::string_view message, LogType type);

  private:
    // Boolean that, if true, will print whenever a new log is sent
    static inline bool no_periodic_wait = false;

    // Logs waiting to be outputed
    static inline std::unordered_map<LogType, std::deque<LogEntry>>
        pendingLogsByType;

    // Count of the length of the dashboard so it can be overwritten
    static inline int lastDashboardLogCount = 0;

    // Log file
    static inline std::ofstream logFile;
};

} // namespace Engine
