#include "Logger.h"

#include <iostream>
#include <string>

namespace Engine {

// Init the static attributes
std::unordered_map<LogType, std::deque<LogEntry>> Logger::pendingLogsByType;
int Logger::MAX_STACKED_PENDING = 100;
int Logger::MAX_IN_PLACE_PENDING = 30;
int Logger::lastDashboardLogCount = 0;
std::ofstream Logger::logFile;
std::mutex Logger::logMutex;

// Init instructions
void Logger::init() {
    logFile.open("engine.log", std::ios::out | std::ios::trunc);
}

// Shutdown instructions for the logger
void Logger::shutdown() { logFile.close(); }

// Add a new log to the pending logs list
// TODO: Why std::string_view
void Logger::log(LogLevel level, std::string_view tag, std::string_view message,
                 LogType type) {

    // Lock the thread
    std::lock_guard<std::mutex> lock(logMutex);
    std::string tagStr(tag);

    // If the file is open, push the new text before anything for debug
    if (type == LogType::STACKED && logFile.is_open()) {
        logFile << "[" << getLevelName(level) << "][" << tag << "] " << message
                << "\n";
        logFile.flush();
    }

    // Push in the new log to the specific tag's pending list
    pendingLogsByType[type].push_back(
        {level, std::string(tag), std::string(message)});

    // If the pending list is to long, pop off the front
    if (type == LogType::STACKED &&
        pendingLogsByType[type].size() > MAX_STACKED_PENDING)
        pendingLogsByType[type].pop_back();
    else if (pendingLogsByType[type].size() > MAX_IN_PLACE_PENDING)
        pendingLogsByType[type].pop_back();
}

// Log wrappers
void Logger::info(std::string_view tag, std::string_view message,
                  LogType type) {
    log(LogLevel::INFO, tag, message, type);
}
void Logger::warn(std::string_view tag, std::string_view message,
                  LogType type) {
    log(LogLevel::WARNING, tag, message, type);
}
void Logger::error(std::string_view tag, std::string_view message,
                   LogType type) {
    log(LogLevel::ERR, tag, message, type);
}
void Logger::fatal(std::string_view tag, std::string_view message,
                   LogType type) {
    log(LogLevel::FATAL, tag, message, type);
}

// Print out the logs (ansi)
void Logger::outputLogs() {

    // Lock the thread to avoid multi-threaded logging problems
    std::lock_guard<std::mutex> lock(logMutex);

    // Move up the dashboard, wiping it, and reseting lastDashboardLogCount
    while (lastDashboardLogCount > 0) {
        std::cout << "\033[F\033[K"; // \033[F up line & \033[K wipe line
        lastDashboardLogCount--;
    }

    // Loop each log for the stacked logs
    for (const auto &log : pendingLogsByType[LogType::STACKED]) {

        // Print out the log w/ ansi data (colors)
        std::cout << getLevelColor(log.level) << "[" << getLevelName(log.level)
                  << "][" << log.tag << "] " << log.message << "\033[0m\n";
    }

    // Wipe the pending logs for the stacked logs
    pendingLogsByType[LogType::STACKED].clear();

    // Print the dashboard seporater only if there is a dashboard
    if (!pendingLogsByType[LogType::IN_PLACE].empty()) {
        std::cout
            << "\033[90m-------------------------------------------\033[0m\n";

        // Count this line for this itteration
        lastDashboardLogCount++;
    }

    // Loop each log for the in_place/dashboard logs
    for (const auto &entry : pendingLogsByType[LogType::IN_PLACE]) {

        // Print out the log w/ ansi data (colors)
        std::cout << getLevelColor(entry.level) << "["
                  << getLevelName(entry.level) << "] [" << entry.tag << "] "
                  << entry.message << "\033[0m\n";

        // Count this line for next itteration
        lastDashboardLogCount++;
    }

    // Clear the pending logs for the in place logs
    pendingLogsByType[LogType::IN_PLACE].clear();

    // Flush the cout
    std::cout << std::flush;
}

// Switch statement to get a string of the level name
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
        return "UNKN";
    }
}

// Switch statement to get the ansi colr of the level name
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
