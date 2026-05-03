#pragma once

#include <deque>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Engine {

// Log severity + log printing status
enum class LogLevel { TRACE, INFO, WARNING, ERR, FATAL };
enum class LogType { STACKED, IN_PLACE };

// Log entry
struct LogEntry {
    LogLevel level;
    std::string message;
};

// Data for each log used during output
struct TagMetadata {
    LogType type = LogType::STACKED;
    size_t lastPrintedIndex = 0; // Only used for STACKED
};

class Logger {
  public:
    // Init & shutdown
    static void init();
    static void shutdown();

    // Register a tag with the logger
    static void registerTag(const std::string &tag, LogType type);

    // Log wrappers
    static void info(std::string_view tag, std::string_view message);
    static void warn(std::string_view tag, std::string_view message);
    static void error(std::string_view tag, std::string_view message);
    static void fatal(std::string_view tag, std::string_view message);

    // Output all logs (ansi)
    static void outputLogs();

    // Get string & ansi color for a specific level
    static const char *getLevelName(LogLevel level);
    static const char *getLevelColor(LogLevel level);

  private:
    // Add a new log to the registries
    static void log(LogLevel level, std::string_view tag,
                    std::string_view message);

    // List off all <tag name, tag datas>
    static std::unordered_map<std::string, TagMetadata> tagRegistry;

    // Tag order output
    static std::vector<std::string> stackedTagOrder;
    static std::vector<std::string> inPlaceTagOrder;

    // History of all log data
    // TODO: Change from history to a list off all not-yet-printed logs
    static std::unordered_map<std::string, std::deque<LogEntry>> history;

    // Count of the length of the dashboard so it can be overwritten
    static int lastDashboardLogCount;

    // Cap of how many logs are stored in the logger history before poped
    static int MAX_LOGGER_HISTORY_PER_TAG; // TODO: temp

    // Log file & thread locker
    static std::mutex logMutex;
    static std::ofstream logFile;
};

} // namespace Engine
