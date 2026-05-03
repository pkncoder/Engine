#include "Logger.h"

#include <iostream>

namespace Engine {

// Init the static attributes
// TODO: See actually why
std::unordered_map<std::string, TagMetadata> Logger::tagRegistry;
std::vector<std::string> Logger::stackedTagOrder;
std::vector<std::string> Logger::inPlaceTagOrder;
std::unordered_map<std::string, std::deque<LogEntry>> Logger::history;
int Logger::lastDashboardLogCount = 0;
std::ofstream Logger::logFile;
std::mutex Logger::logMutex;

// Init instructions
void Logger::init() {
    logFile.open("engine.log", std::ios::out | std::ios::trunc);
}

// Shutdown instructions for the logger
void Logger::shutdown() { logFile.close(); }

// Register a new tag into the registries
void Logger::registerTag(const std::string &tag, LogType type) {

    // Add the tag to the registy
    tagRegistry[tag].type = type;

    // Sort the tag into the tag order it belongs in
    if (type == LogType::IN_PLACE) {
        inPlaceTagOrder.push_back(tag);
    } else {
        stackedTagOrder.push_back(tag);
    }
}

// Save a new log into the tag history
void Logger::log(LogLevel level, std::string_view tag,
                 std::string_view message) {

    // Lock the thread
    std::lock_guard<std::mutex> lock(logMutex);
    std::string tagStr(tag);

    // If the file is open, push the new text before anything for debug
    if (logFile.is_open()) {
        logFile << "[" << getLevelName(level) << "][" << tag << "] " << message
                << "\n";
        logFile.flush();
    }

    // Push in the new log to the specific tag's history
    history[tagStr].push_back({level, std::string(message)});

    // If the history for that tag is too long, pop off the front
    if (history[tagStr].size() > 1000) // TODO: Fix that NUMBER
        history[tagStr].pop_front();
}

// Log wrappers
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

// Print out the logs (ansi)
void Logger::outputLogs() {

    // Lock the thread to avoid multi-threaded logging problems
    std::lock_guard<std::mutex> lock(logMutex);

    // Wipe the old dashboard lines
    for (int i = 0; i < lastDashboardLogCount; ++i)
        std::cout << "\033[F\033[K";

    // Loop the order for each stacked tag
    for (const auto &tag : stackedTagOrder) {

        // Get that tag's metadata
        auto &metadata = tagRegistry[tag];

        // Make sure it's stacked
        if (metadata.type == LogType::STACKED) {
            // Get the history of that tag
            auto &tagHistory = history[tag];

            // TODO: Better way?
            while (metadata.lastPrintedIndex < tagHistory.size()) {

                // Get the entry data
                auto &entry = tagHistory[metadata.lastPrintedIndex];

                // Print out the new log w/ ansi escape for the colors
                std::cout << getLevelColor(entry.level) << "["
                          << getLevelName(entry.level) << "][" << tag << "] "
                          << entry.message << "\033[0m\n";

                // TODO: Better way?
                metadata.lastPrintedIndex++;
            }
        }
    }

    // Save a variable to count the dashboard lines
    // TODO: Move to an approach not to create a variable, and to use for-loops
    // to set lastDashboardLogCount to 0, then increase it in the loop
    int currentDashboardLines = 0;

    // Print the dashboard seporater only if there is a dashboard
    if (!inPlaceTagOrder.empty()) {
        std::cout
            << "\033[90m-------------------------------------------\033[0m\n";
        currentDashboardLines++;
    }

    // Loop each inPlace tag
    for (const auto &tag : inPlaceTagOrder) {

        // Get the history for the tag
        auto &tagHistory = history[tag];

        // Loop each entry & print out the ansi data (color) + entry
        for (const auto &entry : tagHistory) {
            std::cout << getLevelColor(entry.level) << "["
                      << getLevelName(entry.level) << "] [" << tag << "] "
                      << entry.message << "\033[0m\n";
            currentDashboardLines++;
        }

        // Clear the tagHistory for next round
        tagHistory.clear();
    }

    lastDashboardLogCount = currentDashboardLines;

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
