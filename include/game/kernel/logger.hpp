#pragma once

#include <raylib.h>

#include <cstdarg>

// Lets GCC/Clang check printf-style arguments against the format string.
#if defined(__GNUC__) || defined(__clang__)
    #define GAME_PRINTF_FORMAT(formatIndex, firstArgIndex) \
        __attribute__((format(printf, formatIndex, firstArgIndex)))
#else
    #define GAME_PRINTF_FORMAT(formatIndex, firstArgIndex)
#endif

namespace game {

    class Logger final {
    public:
        enum class ColorMode {
            automatic,
            always,
            never
        };

        Logger() = delete;

        // Application and Raylib levels are separate so application INFO logs can
        // remain visible while verbose Raylib initialization logs are suppressed.
        static void init(
            int applicationLevel = LOG_INFO,
            int raylibLevel = LOG_WARNING,
            ColorMode colorMode = ColorMode::automatic
        ) noexcept;

        static void log(int logLevel, const char* text, ...) noexcept GAME_PRINTF_FORMAT(2, 3);
        static void setLevel(int logLevel) noexcept;
        static void setRaylibLevel(int logLevel) noexcept;
        static void setColorMode(ColorMode colorMode) noexcept;
        static void toggle(bool enable) noexcept;

    private:
        static void raylibCallback(
            int logLevel,
            const char* text,
            std::va_list args
        ) noexcept;
    };

} // namespace game
