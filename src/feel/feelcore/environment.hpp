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

#include <feel/feelcore/assert.hpp>
#include <feel/feelcore/namedarguments.hpp>

#if defined _WIN32 || defined __CYGWIN__
#if defined(FEELPP_CORE_LIB_LIBRARY_EXPORTING)
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

//! class build as singleton that represent environment
class FEELPP_CORE_EXPORT Environment
{
    //! support singleton with only private constuctor.
    Environment( int argc, char* argv[], po::options_description const& optionsDescription = {} );
  public :
  //! singletons should not be cloneable.
  Environment(Environment &other) = delete;
  //! singletons should not be assignable.
  void operator=(const Environment &) = delete;

  po::variables_map const& vm() const { return M_vm; }
  //! return option description of environment
  po::options_description options() const;

  //! return data directory (Warning currently defined on sources directory)
  fs::path dataDir() const;

  //! return string by expanding some keyword of format ${key}
  std::string expand( std::string const& expr ) const;

  private:
  //! return singleton instance
  static Environment* instance();
  static Environment* createInstance( int argc, char* argv[], po::options_description const& optionsDescription = {} );

  friend FEELPP_CORE_EXPORT Environment* createEnvironment( int argc, char* argv[], po::options_description const& optionsDescription );
  friend FEELPP_CORE_EXPORT Environment* environment();
  private:
  static std::unique_ptr<Environment> S_instance;
  po::variables_map M_vm;
};

FEELPP_CORE_EXPORT Environment* createEnvironment( int argc, char* argv[], po::options_description const& optionsDescription = {} );

FEELPP_CORE_EXPORT Environment* environment();



template <typename ... Ts>
auto optionIterator( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    std::string const& name  = args.get(_name);
    std::string const& prefix = args.get_else(_prefix, "" );
    po::variables_map const& vm = args.get_else(_vm, environment()->vm() );
    auto opt = fmt::memory_buffer();
    if ( !prefix.empty() )
        fmt::format_to( std::back_inserter(opt), "{}.",prefix);
    fmt::format_to( std::back_inserter( opt ), "{}",name);
    std::string optname = fmt::to_string(opt);
    auto it = vm.find( optname );
    return std::make_tuple( it != vm.end(), it, std::move( optname ) );
}

template <typename ... Ts>
bool hasOption( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    return std::get<0>( optionIterator( std::forward<Ts>(v)... ) );
}

template <typename ... Ts>
po::variable_value option( Ts && ... v )
{
    auto args = NA::make_arguments( std::forward<Ts>(v)... );
    po::variables_map const& vm = args.get_else(_vm, environment()->vm() );
    auto [exist,it,optname] = optionIterator( std::forward<Ts>(v)... );
    ASSERT( exist, fmt::format( "invalid or missing option {}",optname ) );
    return it->second;
}

}
