#ifndef PLATFORMDEFINE_H
#define PLATFORMDEFINE_H

#ifndef MACOS

#include <format>

#define FORMAT std::format
#define VFORMAT std::vformat
#define MK_FMT_ARGS std::make_format_args
#define THREAD std::jthread

#else   // MACOS

#include <fmt/format.h>
#include <fmt/std.h>

#define FORMAT fmt::format
#define VFORMAT fmt::vformat
#define MK_FMT_ARGS fmt::make_format_args
#define THREAD std::thread

#endif  // MACOS


#endif   // PLATFORMDEFINE_H
