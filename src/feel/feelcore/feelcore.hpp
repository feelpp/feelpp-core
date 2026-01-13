//!

#pragma once

#include <cmath>
#include <memory>
#include <filesystem>
#include <boost/program_options.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define FEELPP_DECL_EXPORT __attribute__ ((dllexport))
#else
#define FEELPP_DECL_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#ifdef __GNUC__
#define FEELPP_DECL_IMPORT __attribute__ ((dllimport))
#else
#define FEELPP_DECL_IMPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#define FEELPP_DECL_HIDDEN
#else
#if __GNUC__ >= 4 || defined(__clang__)
#define FEELPP_DECL_EXPORT     __attribute__((visibility("default")))
#define FEELPP_DECL_IMPORT     __attribute__((visibility("default")))
#define FEELPP_DECL_HIDDEN     __attribute__((visibility("hidden")))
#else
#define FEELPP_DECL_EXPORT
#define FEELPP_DECL_IMPORT
#define FEELPP_DECL_HIDDEN
#endif
#endif

#if defined(FEELPP_CORE_LIB_LIBRARY_EXPORTING)
#define FEELPP_CORE_EXPORT FEELPP_DECL_EXPORT
#else
#define FEELPP_CORE_EXPORT FEELPP_DECL_IMPORT
#endif


namespace Feel
{

namespace fs = std::filesystem;
namespace po = boost::program_options;
namespace log = spdlog;
namespace nl = nlohmann;
//using json = nl::json;

}
