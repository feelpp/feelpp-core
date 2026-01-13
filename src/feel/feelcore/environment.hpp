//!

#pragma once

#include <feel/feelcore/feelcore.hpp>
#include <feel/feelcore/assert.hpp>
#include <feel/feelcore/namedarguments.hpp>

namespace Feel::Core
{

class FEELPP_CORE_EXPORT EnvironmentPlugin
{
public:
  EnvironmentPlugin( std::string const& name, fs::path const& dataDir ) : M_name( name), M_dataDir( dataDir ) {}
  EnvironmentPlugin( EnvironmentPlugin const& ) = default;
  EnvironmentPlugin( EnvironmentPlugin && ) = default;
  std::string const& name() const { return M_name; }
  fs::path const& dataDir() const { return M_dataDir; }
  private:
  std::string M_name;
  fs::path M_dataDir;
};


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
  fs::path dataDir( std::string const& key = "" ) const;

  //! return string by expanding some keyword of format ${key}
  std::string expand( std::string const& expr ) const;

  template <typename T>
  void appendPlugin( T && plugin ) { M_plugins.emplace( plugin.name(), std::forward<T>( plugin ) ); }

private:
  //! return singleton instance
  static Environment* instance();
  static Environment* createInstance( int argc, char* argv[], po::options_description const& optionsDescription = {} );

  friend FEELPP_CORE_EXPORT Environment* createEnvironment( int argc, char* argv[], po::options_description const& optionsDescription );
  friend FEELPP_CORE_EXPORT Environment* environment();
  private:
  static std::unique_ptr<Environment> S_instance;
  po::variables_map M_vm;
  std::map<std::string,EnvironmentPlugin> M_plugins;
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
