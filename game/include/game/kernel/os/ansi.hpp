#pragma once

#include <string_view>

// ANSI escape sequences used for coloured terminal output.
namespace game::kernel::ansi {

inline constexpr std::string_view reset = "\x1b[0m";

namespace style {
inline constexpr std::string_view bold = "\x1b[1m";
inline constexpr std::string_view dim = "\x1b[2m";
} // namespace style

namespace foreground {
inline constexpr std::string_view defaultColor = "\x1b[39m";
inline constexpr std::string_view red = "\x1b[31m";
inline constexpr std::string_view yellow = "\x1b[33m";
inline constexpr std::string_view magenta = "\x1b[35m";
inline constexpr std::string_view brightBlack = "\x1b[90m";
inline constexpr std::string_view brightRed = "\x1b[91m";
inline constexpr std::string_view brightYellow = "\x1b[93m";
inline constexpr std::string_view brightMagenta = "\x1b[95m";
inline constexpr std::string_view brightCyan = "\x1b[96m";
inline constexpr std::string_view brightWhite = "\x1b[97m";
} // namespace foreground

} // namespace game::kernel::ansi