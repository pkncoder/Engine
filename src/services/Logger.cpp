#include "Logger.h"
#include <iostream>

namespace Engine {

std::unordered_map<std::string, TagMetadata> Logger::s_TagRegistry;
std::vector<std::string> Logger::s_TagOrder;
std::vector<std::string> Logger::s_InPlaceTags;
std::unordered_map<std::string, std::deque<LogEntry>> Logger::s_HistoryMap;
int Logger::s_LastInPlaceLineCount = 0;
std::ofstream Logger::s_LogFile;
std::mutex Logger::s_LogMutex;

void Logger::init() {
    s_LogFile.open("engine.log", std::ios::out | std::ios::trunc);
    // Pre-register some defaults
    registerTag("SYSTEM", LogType::STACKED);
    registerTag("PROFILE", LogType::IN_PLACE);
}

void Logger::shutdown() { s_LogFile.close(); }

void Logger::registerTag(const std::string &tag, LogType type) {
    s_TagRegistry[tag].type = type;
    s_TagOrder.push_back(tag);
    if (type == LogType::IN_PLACE) {
        s_InPlaceTags.push_back(tag);
    }
}
void Logger::log(LogLevel level, std::string_view tag,
                 std::string_view message) {
    std::lock_guard<std::mutex> lock(s_LogMutex);
    std::string tagStr(tag);

    // 1. Immediate File Write (Safety first if we crash!)
    if (s_LogFile.is_open()) {
        s_LogFile << "[" << getLevelName(level) << "][" << tag << "] "
                  << message << "\n";
        s_LogFile.flush();
    }

    // 2. Store in history
    s_HistoryMap[tagStr].push_back({level, std::string(message)});
    if (s_HistoryMap[tagStr].size() > 1000)
        s_HistoryMap[tagStr].pop_front();
}

void Logger::outputLogs() {
    std::lock_guard<std::mutex> lock(s_LogMutex);

    // 1. "WIPE" the old dashboard
    // We move up the exact number of lines we printed last frame and clear them
    for (int i = 0; i < s_LastInPlaceLineCount; ++i) {
        // \033[F moves to the start of the previous line
        // \033[K clears from cursor to end of line
        std::cout << "\033[F\033[K";
    }

    // 2. PRINT NEW STACKED LOGS
    // We loop through our registered tags in order
    for (const auto &tag : s_TagOrder) {
        auto &metadata = s_TagRegistry[tag];
        if (metadata.type == LogType::STACKED) {
            auto &history = s_HistoryMap[tag];
            while (metadata.lastPrintedIndex < history.size()) {
                auto &entry = history[metadata.lastPrintedIndex];
                std::cout << getLevelColor(entry.level) << "["
                          << getLevelName(entry.level) << "][" << tag << "] "
                          << entry.message << "\033[0m\n";
                metadata.lastPrintedIndex++;
            }
        }
    }

    // 3. PRINT THE DASHBOARD (IN_PLACE)
    int currentDashboardLines = 0;

    // Optional: Print a divider so you can see where history ends and dashboard
    // begins
    if (!s_InPlaceTags.empty()) {
        std::cout
            << "\033[90m------------------------------------------\033[0m\n";
        currentDashboardLines++;
    }

    for (const auto &tag : s_InPlaceTags) {
        auto &history = s_HistoryMap[tag];
        // Print everything logged to this IN_PLACE tag this frame
        for (const auto &entry : history) {
            std::cout << getLevelColor(entry.level) << "["
                      << getLevelName(entry.level) << "][" << tag << "] "
                      << entry.message << "\033[0m\n";
            currentDashboardLines++;
        }
        // IMPORTANT: Clear the history for IN_PLACE tags so they don't
        // accumulate
        history.clear();
    }

    s_LastInPlaceLineCount = currentDashboardLines;
    std::cout << std::flush;
}

void Logger::info(std::string_view tag, std::string_view message) {
    log(LogLevel::INFO, tag, message);
}
void Logger::warn(std::string_view tag, std::string_view message) {
    log(LogLevel::WARNING, tag, message);
}
void Logger::error(std::string_view tag, std::string_view message) {
    log(LogLevel::ERR, tag, message);
}
void Logger::fatal(std::string_view tag, std::string_view message) {
    log(LogLevel::FATAL, tag, message);
}

const char *Logger::getLevelName(LogLevel level) {
    switch (level) {
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARN";
    case LogLevel::ERR:
        return "ERROR";
    case LogLevel::FATAL:
        return "FATAL";
    default:
        return "UNKN ";
    }
}

const char *Logger::getLevelColor(LogLevel level) {
    // ANSI escape codes for terminal coloring
    switch (level) {
    case LogLevel::INFO:
        return "\033[32m"; // Green
    case LogLevel::WARNING:
        return "\033[33m"; // Yellow
    case LogLevel::ERR:
        return "\033[31m"; // Red
    case LogLevel::FATAL:
        return "\033[41m\033[37m"; // White on Red background
    default:
        return "\033[0m"; // Reset
    }
}

} // namespace Engine
