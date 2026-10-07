// File created by kwlew on 2026-12-09 @ 6:47 AM
#include "game/kernel/logger.hpp"

#include "game/kernel/ansi.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <io.h>
    #include <windows.h>
#else
    #include <unistd.h>
#endif

namespace game {
namespace {

    struct LogAppearance {
        std::string_view icon;
        std::string_view label;
        std::string_view accent;
        std::string_view message;
    };

    std::atomic<int> applicationLogLevel{LOG_INFO};
    std::atomic<int> configuredRaylibLevel{LOG_WARNING};
    std::atomic<bool> loggingEnabled{true};
    std::atomic<Logger::ColorMode> configuredColorMode{
        Logger::ColorMode::automatic
    };
    std::mutex outputMutex;

    [[nodiscard]] LogAppearance appearanceFor(int logLevel) noexcept {
        using namespace std::string_view_literals;

        switch (logLevel) {
            case LOG_TRACE:
                return {"\xC2\xB7"sv, "TRACE"sv, ansi::foreground::brightBlack,
                        ansi::foreground::brightBlack};
            case LOG_DEBUG:
                return {"\xE2\x97\x86"sv, "DEBUG"sv, ansi::foreground::brightMagenta,
                        ansi::foreground::magenta};
            case LOG_INFO:
                return {"\xE2\x97\x8F"sv, "INFO"sv, ansi::foreground::brightCyan,
                        ansi::foreground::defaultColor};
            case LOG_WARNING:
                return {"\xE2\x96\xB2"sv, "WARN"sv, ansi::foreground::brightYellow,
                        ansi::foreground::yellow};
            case LOG_ERROR:
                return {"\xE2\x9C\x98"sv, "ERROR"sv, ansi::foreground::brightRed,
                        ansi::foreground::red};
            case LOG_FATAL:
                return {"\xE2\x9C\x96"sv, "FATAL"sv, ansi::foreground::brightWhite,
                        ansi::foreground::brightRed};
            default:
                return {"?"sv, "UNKNOWN"sv, ansi::foreground::brightBlack,
                        ansi::foreground::defaultColor};
        }
    }

    [[nodiscard]] bool environmentVariablePresent(const char* name) noexcept {
        const char* value = std::getenv(name);
        return value != nullptr && value[0] != '\0';
    }

    [[nodiscard]] bool forceColorEnabled() noexcept {
        const char* value = std::getenv("FORCE_COLOR");
        return value != nullptr && value[0] != '\0' && std::strcmp(value, "0") != 0;
    }

    [[nodiscard]] bool terminalSupportsColor(std::FILE* output) noexcept {
        // NO_COLOR follows https://no-color.org/: any non-empty value opts out.
        if (environmentVariablePresent("NO_COLOR")) {
            return false;
        }

        if (forceColorEnabled()) {
            return true;
        }

        const char* term = std::getenv("TERM");
        if (term != nullptr && std::strcmp(term, "dumb") == 0) {
            return false;
        }

#ifdef _WIN32
        const int descriptor = ::_fileno(output);
        if (descriptor < 0 || ::_isatty(descriptor) == 0) {
            return false;
        }

        const DWORD stream = output == stderr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE;
        const HANDLE handle = ::GetStdHandle(stream);
        if (handle == INVALID_HANDLE_VALUE || handle == nullptr) {
            return false;
        }

        DWORD mode = 0;
        if (::GetConsoleMode(handle, &mode) == 0) {
            return false;
        }

        return ::SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
        return ::isatty(::fileno(output)) != 0;
#endif
    }

    [[nodiscard]] bool shouldUseColor(std::FILE* output) noexcept {
        switch (configuredColorMode.load()) {
            case Logger::ColorMode::always:
                return true;
            case Logger::ColorMode::never:
                return false;
            case Logger::ColorMode::automatic:
                return terminalSupportsColor(output);
        }

        return false;
    }

    void writeSequence(std::FILE* output, std::string_view sequence) noexcept {
        std::fwrite(sequence.data(), sizeof(char), sequence.size(), output);
    }

    [[nodiscard]] std::array<char, 16> currentTime() noexcept {
        const auto now = std::chrono::system_clock::now();
        const std::time_t rawTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTime{};

#ifdef _WIN32
        ::localtime_s(&localTime, &rawTime);
#else
        ::localtime_r(&rawTime, &localTime);
#endif

        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ) % 1000;

        std::array<char, 16> result{};
        std::snprintf(
            result.data(),
            result.size(),
            "%02d:%02d:%02d.%03d",
            localTime.tm_hour,
            localTime.tm_min,
            localTime.tm_sec,
            static_cast<int>(milliseconds.count())
        );
        return result;
    }

    [[nodiscard]] std::string formatMessage(
        const char* format,
        std::va_list args
    ) {
        if (format == nullptr) {
            return "(null log message)";
        }

        std::va_list sizeArgs;
        va_copy(sizeArgs, args);
        const int requiredSize = std::vsnprintf(nullptr, 0, format, sizeArgs);
        va_end(sizeArgs);

        if (requiredSize < 0) {
            return format;
        }

        std::vector<char> buffer(static_cast<std::size_t>(requiredSize) + 1U);
        std::va_list writeArgs;
        va_copy(writeArgs, args);
        std::vsnprintf(buffer.data(), buffer.size(), format, writeArgs);
        va_end(writeArgs);

        std::string message(buffer.data(), static_cast<std::size_t>(requiredSize));
        while (!message.empty() && (message.back() == '\n' || message.back() == '\r')) {
            message.pop_back();
        }
        return message;
    }

    void writeLogLine(
        int logLevel,
        std::string_view message,
        std::FILE* output
    ) noexcept {
        const LogAppearance appearance = appearanceFor(logLevel);
        const auto timestamp = currentTime();
        const bool useColor = shouldUseColor(output);
        const std::lock_guard lock(outputMutex);

        if (useColor) {
            writeSequence(output, ansi::style::dim);
            writeSequence(output, ansi::foreground::brightBlack);
        }
        std::fprintf(output, "%s  ", timestamp.data());

        if (useColor) {
            writeSequence(output, ansi::reset);
            writeSequence(output, appearance.accent);
            writeSequence(output, ansi::style::bold);
        }
        std::fprintf(
            output,
            "%.*s %-5.*s",
            static_cast<int>(appearance.icon.size()),
            appearance.icon.data(),
            static_cast<int>(appearance.label.size()),
            appearance.label.data()
        );

        if (useColor) {
            writeSequence(output, ansi::reset);
            writeSequence(output, ansi::style::dim);
            writeSequence(output, ansi::foreground::brightBlack);
        }
        std::fputs("  |  ", output);

        if (useColor) {
            writeSequence(output, ansi::reset);
            writeSequence(output, appearance.message);
            if (logLevel == LOG_FATAL) {
                writeSequence(output, ansi::style::bold);
            }
        }
        std::fwrite(message.data(), sizeof(char), message.size(), output);

        if (useColor) {
            writeSequence(output, ansi::reset);
        }
        std::fputc('\n', output);
        std::fflush(output);
    }

} // namespace

void Logger::init(
    int applicationLevel,
    int raylibLevel,
    ColorMode colorMode
) noexcept {
    applicationLogLevel.store(applicationLevel);
    configuredRaylibLevel.store(raylibLevel);
    configuredColorMode.store(colorMode);
    loggingEnabled.store(true);

    SetTraceLogCallback(Logger::raylibCallback);
    SetTraceLogLevel(raylibLevel);
}

void Logger::log(int logLevel, const char* text, ...) noexcept {
    if (!loggingEnabled.load() || logLevel < applicationLogLevel.load()) {
        return;
    }

    std::va_list args;
    va_start(args, text);
    raylibCallback(logLevel, text, args);
    va_end(args);
}

void Logger::setLevel(int logLevel) noexcept {
    applicationLogLevel.store(logLevel);
}

void Logger::setRaylibLevel(int logLevel) noexcept {
    configuredRaylibLevel.store(logLevel);
    if (loggingEnabled.load()) {
        SetTraceLogLevel(logLevel);
    }
}

void Logger::setColorMode(ColorMode colorMode) noexcept {
    configuredColorMode.store(colorMode);
}

void Logger::toggle(bool enable) noexcept {
    loggingEnabled.store(enable);
    SetTraceLogLevel(enable ? configuredRaylibLevel.load() : LOG_NONE);
}

void Logger::raylibCallback(
    int logLevel,
    const char* text,
    std::va_list args
) noexcept {
    try {
        const std::string message = formatMessage(text, args);
        std::FILE* output = logLevel >= LOG_WARNING ? stderr : stdout;
        writeLogLine(logLevel, message, output);
    } catch (...) {
        // Never allow an exception to cross Raylib's C callback boundary.
        std::fputs("[LOGGER ERROR] Failed to format a log message.\n", stderr);
        std::fflush(stderr);
    }

    // A custom Raylib callback bypasses Raylib's normal LOG_FATAL exit path.
    if (logLevel == LOG_FATAL) {
        std::exit(EXIT_FAILURE);
    }
}

} // namespace game
