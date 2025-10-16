//!
#include <feel/feelcore/environment.hpp>

#include <iostream>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/preprocessor/stringize.hpp>

namespace Feel
{

Environment::Environment( int argc, char* argv[], po::options_description const& optionsDescription )
{
    po::options_description fullOptionsDescription = this->options().add( optionsDescription );
    po::store( po::command_line_parser( argc, argv ).options( fullOptionsDescription ).allow_unregistered().run(), M_vm );
    po::notify( M_vm );

    if ( M_vm.count( "help" ) )
    {
        std::cout << "Usage: options_description [options]\n";
        std::cout << optionsDescription;
        std::exit(0);
    }

    std::vector<std::string> configFiles;
    if ( M_vm.count( "config-files" ) )
    {
        std::vector<std::string> configFilesOptVec = M_vm["config-files"].as<std::vector<std::string> >();
        configFiles.insert( configFiles.end(), configFilesOptVec.begin(), configFilesOptVec.end() );
    }

    // reverse order (priorty for the last)
    std::reverse(configFiles.begin(),configFiles.end());
    for ( std::string const& cfgfile : configFiles )
    {
        if ( !fs::exists( cfgfile ) ) continue;
        fs::path cfgAbsolutePath = fs::canonical( cfgfile );
        log::info("Reading config-file {}",cfgAbsolutePath.string() );
        std::ifstream ifs( cfgfile );
        po::store( parse_config_file( ifs, fullOptionsDescription, true ), M_vm );
    }
    po::notify( M_vm );
}

Environment*
Environment::instance()
{
    // TODO : assert if not init
    return S_instance.get();
}
Environment*
Environment::createInstance( int argc, char* argv[], po::options_description const& optionsDescription )
{
    S_instance = std::unique_ptr<Environment>{ new Environment( argc, argv, optionsDescription ) };
    return S_instance.get();
}


po::options_description
Environment::options() const
{
    po::options_description desc("Environment options");
    desc.add_options()
        ( "help", "produce help message" )
        ( "verbose,v", po::value<int>()->implicit_value(1), "enable verbosity (optionally specify level)" )
        ( "config-files", po::value<std::vector<std::string> >()->multitoken(), "specify a list of .cfg file" )
        ;
    return desc;

}

fs::path Environment::dataDir( std::string const& key ) const
{
    if ( key.empty() )
        return fs::path( BOOST_PP_STRINGIZE(FEELPP_CORE_DATA_DIR) );
    else
        return M_plugins.at( key ).dataDir();
}

std::string
Environment::expand( std::string const& expr ) const
{
    std::string res = expr;
    std::vector<std::pair<std::string,std::string> > keyToValue = { std::make_pair( "${datadir}", this->dataDir().string() ) };
    for ( auto const& [key,plugin] : M_plugins )
        keyToValue.push_back( std::make_pair( fmt::format( "${{{}.datadir}}", key ), plugin.dataDir().string() ) );

    for ( auto const& [key,val] : keyToValue )
        boost::ireplace_all( res, key, val );
    return res;
}


std::unique_ptr<Environment> Environment::S_instance = nullptr;

Environment* environment()
{
    return Environment::instance();
}
Environment* createEnvironment( int argc, char* argv[], po::options_description const& optionsDescription )
{
    return Environment::createInstance( argc, argv, optionsDescription );
}

}// namespace Feel::Ktirio::Geom

