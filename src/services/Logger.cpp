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

    if (s_LogFile.is_open()) {
        s_LogFile << "[" << getLevelName(level) << "][" << tag << "] "
                  << message << "\n";
        s_LogFile.flush();
    }

    s_HistoryMap[tagStr].push_back({level, std::string(message)});
    if (s_HistoryMap[tagStr].size() > 1000) // TODO: Fix that NUMBER
        s_HistoryMap[tagStr].pop_front();
}

void Logger::outputLogs() {
    std::lock_guard<std::mutex> lock(s_LogMutex);

    // Wipe the old lines
    for (int i = 0; i < s_LastInPlaceLineCount; ++i)
        std::cout << "\033[F\033[K";

    // TODO: Stacked (temp comment)
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

    int currentDashboardLines = 0;

    if (!s_InPlaceTags.empty()) {
        std::cout
            << "\033[90m------------------------------------------\033[0m\n";
        currentDashboardLines++;
    }

    for (const auto &tag : s_InPlaceTags) {
        auto &history = s_HistoryMap[tag];

        for (const auto &entry : history) {
            std::cout << getLevelColor(entry.level) << "["
                      << getLevelName(entry.level) << "][" << tag << "] "
                      << entry.message << "\033[0m\n";
            currentDashboardLines++;
        }

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
