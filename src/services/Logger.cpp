#include "Logger.h"
#include <iostream>

namespace Engine {

std::unordered_map<std::string, TagMetadata> Logger::tagRegistry;
std::vector<std::string> Logger::stackedTagOrder;
std::vector<std::string> Logger::inPlaceTagOrder;
std::unordered_map<std::string, std::deque<LogEntry>> Logger::history;
int Logger::lastLogCount = 0;
std::ofstream Logger::logFile;
std::mutex Logger::logMutex;

void Logger::init() {
    logFile.open("engine.log", std::ios::out | std::ios::trunc);
}

void Logger::shutdown() { logFile.close(); }

void Logger::registerTag(const std::string &tag, LogType type) {
    tagRegistry[tag].type = type;
    stackedTagOrder.push_back(tag);

    if (type == LogType::IN_PLACE) {
        inPlaceTagOrder.push_back(tag);
    }
}
void Logger::log(LogLevel level, std::string_view tag,
                 std::string_view message) {

    std::lock_guard<std::mutex> lock(logMutex);
    std::string tagStr(tag);

    if (logFile.is_open()) {
        logFile << "[" << getLevelName(level) << "][" << tag << "] " << message
                << "\n";
        logFile.flush();
    }

    history[tagStr].push_back({level, std::string(message)});
    if (history[tagStr].size() > 1000) // TODO: Fix that NUMBER
        history[tagStr].pop_front();
}

void Logger::outputLogs() {
    std::lock_guard<std::mutex> lock(logMutex);

    // Wipe the old lines
    for (int i = 0; i < lastLogCount; ++i)
        std::cout << "\033[F\033[K";

    // TODO: Stacked (temp comment)
    for (const auto &tag : stackedTagOrder) {
        auto &metadata = tagRegistry[tag];

        if (metadata.type == LogType::STACKED) {
            auto &tagHistory = history[tag];

            // TODO: Better way?
            while (metadata.lastPrintedIndex < tagHistory.size()) {
                auto &entry = tagHistory[metadata.lastPrintedIndex];

                std::cout << getLevelColor(entry.level) << "["
                          << getLevelName(entry.level) << "][" << tag << "] "
                          << entry.message << "\033[0m\n";

                metadata.lastPrintedIndex++;
            }
        }
    }

    int currentDashboardLines = 0;

    if (!inPlaceTagOrder.empty()) {
        std::cout
            << "\033[90m------------------------------------------\033[0m\n";
        currentDashboardLines++;
    }

    for (const auto &tag : inPlaceTagOrder) {
        auto &tagHistory = history[tag];

        for (const auto &entry : tagHistory) {
            std::cout << getLevelColor(entry.level) << "["
                      << getLevelName(entry.level) << "][" << tag << "] "
                      << entry.message << "\033[0m\n";
            currentDashboardLines++;
        }

        tagHistory.clear();
    }

    lastLogCount = currentDashboardLines;
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
