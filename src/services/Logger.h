#pragma once

#include <deque>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Engine {

enum class LogLevel { TRACE, INFO, WARNING, ERR, FATAL };
enum class LogType { STACKED, IN_PLACE };

struct LogEntry {
    LogLevel level;
    std::string message;
};

struct TagMetadata {
    LogType type = LogType::STACKED;
    size_t lastPrintedIndex = 0; // Only used for STACKED
};

class Logger {
  public:
    static void init();
    static void shutdown();

    static void registerTag(const std::string &tag, LogType type);

    static void outputLogs();

    static void info(std::string_view tag, std::string_view message);
    static void warn(std::string_view tag, std::string_view message);
    static void error(std::string_view tag, std::string_view message);
    static void fatal(std::string_view tag, std::string_view message);

    static const char *getLevelColor(LogLevel level);
    static const char *getLevelName(LogLevel level);

  private:
    static void log(LogLevel level, std::string_view tag,
                    std::string_view message);

    static std::unordered_map<std::string, TagMetadata> tagRegistry;

    static std::vector<std::string> stackedTagOrder;
    static std::vector<std::string> inPlaceTagOrder;

    static std::unordered_map<std::string, std::deque<LogEntry>> history;

    static int lastLogCount;
    static std::mutex logMutex;
    static std::ofstream logFile;
};

} // namespace Engine
