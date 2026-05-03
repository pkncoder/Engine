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

    // Call this in Application::init() to set behavior for specific tags
    static void registerTag(const std::string &tag, LogType type);

    static void trace(std::string_view tag, std::string_view message);
    static void info(std::string_view tag, std::string_view message);
    static void warn(std::string_view tag, std::string_view message);
    static void error(std::string_view tag, std::string_view message);
    static void fatal(std::string_view tag, std::string_view message);

    // THE NEW HEARTBEAT: Call this once per frame in Application::run()
    static void outputLogs();

  private:
    static void log(LogLevel level, std::string_view tag,
                    std::string_view message);
    static const char *getLevelColor(LogLevel level);
    static const char *getLevelName(LogLevel level);

  private:
    static std::unordered_map<std::string, std::deque<LogEntry>> s_HistoryMap;
    static std::unordered_map<std::string, TagMetadata> s_TagRegistry;

    // Add to Logger class private members
    static std::vector<std::string>
        s_TagOrder; // Tracks the order tags were registered
    static std::vector<std::string>
        s_InPlaceTags; // Specific order for the dashboard

    static std::ofstream s_LogFile;
    static std::mutex s_LogMutex;
    static int s_LastInPlaceLineCount;
};

} // namespace Engine
