//!
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
//#include <catch2/catch_template_test_macros.hpp>
//#include <catch2/catch_approx.hpp>

#include <feel/feelcore/environment.hpp>
#include <feel/feelcore/table.hpp>
#include <feel/feelcore/tabulateinformations.hpp>

TEST_CASE( "Test Table", "[Feel::Core::Table]" )
{
    using namespace Feel;
    //using namespace Feel::Core;

    auto logInfo = []( auto const& t )
    {
        std::ostringstream ss; ss << t;
        log::info( "\n{}", ss.str() );
    };

    Feel::Table tableA;
    tableA.add_row( {"Name","Value"} );
    tableA.add_row( {"Pi",3.14159} );

    Feel::Table tableB;
    tableB.add_row( {"Parameter","Description","Value"} );
    tableB.add_row( {"Alpha","Angle in degrees",45} );
    tableB.add_row( {"Beta","Another angle in degrees",60} );
    //logInfo( tableB );

    tableA.add_row( {"Trigonometric Parameters",tableB} );
    //logInfo( tableA );


    auto section = TabulateInformationsSections::New();
    section->add( "toto", TabulateInformations::New( tableA ) );
    //logInfo( *section );

    logInfo( section->exporterAscii( true ) );
}

int main( int argc, char* argv[])
{
    using namespace Feel;

    po::options_description desc("Allowed options");
    desc.add_options()
        ( "input.mesh.filename", po::value<std::string>(), "input mesh filename" )
        ( "input.mesh.format", po::value<std::string>()->default_value( "auto" ), "input mesh format" )
        ( "output.mesh.filename", po::value<std::string>()->default_value("mesh.stl"), "output mesh filename" )
        ( "output.mesh.format", po::value<std::string>()->default_value( "auto" ), "output mesh format" )
        ;

    auto env = Feel/*::Core*/::createEnvironment( argc, argv, desc );
    return Catch::Session().run( argc, argv );
}
