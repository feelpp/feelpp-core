//!

#include <cmath>
#include <memory>
#include <filesystem>
#include <boost/program_options.hpp>
// #include <fmt/core.h>
// #include <fmt/format.h>
// #include <spdlog/spdlog.h>
// #include <nlohmann/json.hpp>

// #include <ktirio/geom/assert.hpp>
#include <feel/feelcore/namedarguments.hpp>

#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #if defined(KTIRIO_GEOM_LIB_LIBRARY_EXPORTING)
    #ifdef __GNUC__
      #define FEELPP_DECL_EXPORT __attribute__ ((dllexport))
    #else
      #define FEELPP_DECL_EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define FEELPP_DECL_IMPORT __attribute__ ((dllimport))
    #else
      #define FEELPP_DECL_IMPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
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

#if defined(KTIRIO_GEOM_LIB_LIBRARY_EXPORTING)
#define KTIRIO_GEOM_EXPORT FEELPP_DECL_EXPORT
#else
#define KTIRIO_GEOM_EXPORT FEELPP_DECL_IMPORT
#endif


namespace Feel
{

  //! class build as singleton that represent environment
  class KTIRIO_GEOM_EXPORT Environment
    {
    public:
    Environment();
    };
}
