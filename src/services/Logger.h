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
    static void info(const std::string_view tag, const std::string_view message,
                     const LogType type = LogType::STACKED);
    static void warn(const std::string_view tag, const std::string_view message,
                     const LogType type = LogType::STACKED);
    static void error(const std::string_view tag,
                      const std::string_view message,
                      const LogType type = LogType::STACKED);
    static void fatal(const std::string_view tag,
                      const std::string_view message,
                      const LogType type = LogType::STACKED);
    static void debug(const std::string_view message,
                      const LogType type = LogType::STACKED);

    // Debug log
    static inline void check() {
        log(LogLevel::DEBUG, "DEBUG", "--- CHECK ---", LogType::STACKED);
    };

    // Special log wrappers
    static inline void space(const LogType type = LogType::STACKED) {
        log(LogLevel::FORMATTING, "NULL", "", type);
    };
    static inline void line(const int length = 15,
                            const LogType type = LogType::STACKED) {
        std::string line(length, '-');

        log(LogLevel::FORMATTING, "NULL", line, type);
    };

    // Output all pending logs (in terminal - ansii && file - text)
    static void outputLogs();

    // Bool flag to stop the dashboard from overwriting (used at shutdown)
    static inline void setSkipDashboard(const bool newValue) {
        skipDashboard = newValue;
    }
    // Set the boolean to print a log right when added
    static inline void setNoPendingLogs(const bool newValue) {
        noPeriodicWait = newValue;
    }

    // Get string & ansi color for a specific level
    static const char *getLevelName(const LogLevel level);
    static const char *getLevelColor(const LogLevel level);

  private:
    // Add a new log to the pending list
    static void log(const LogLevel level, const std::string_view tag,
                    const std::string_view message, const LogType type);

  private:
    // Logger flags
    static inline bool skipDashboard = false;
    static inline bool noPeriodicWait = false;

    // Logs waiting to be outputed
    static inline std::unordered_map<LogType, std::deque<LogEntry>>
        pendingLogsByType;

    // Count of the length of the dashboard so it can be overwritten
    static inline int lastDashboardLogCount = 0;

    // Log file steam
    static inline std::ofstream logFile;
};

} // namespace Engine
