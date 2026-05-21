//!
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
//#include <catch2/catch_template_test_macros.hpp>
//#include <catch2/catch_approx.hpp>

#include <feel/feelcore/environment.hpp>
#include <feel/feelcore/threadpool.hpp>
#include <stdexcept>
#include <thread>

TEST_CASE( "Test basic lambda", "[Feel::Core::ThreadPool]" )
{
    using namespace Feel::Core;

    ThreadPool pool( 4 );
    auto future = pool.enqueue( [](){ return 42; } );
    REQUIRE( future.get() == 42 );
}


TEST_CASE( "Test argument forwarding", "[Feel::Core::ThreadPool]" )
{
    using namespace Feel::Core;

    ThreadPool pool( 4 );
    auto future1 = pool.enqueue( []( int a, int b ){ return a+b; }, 10, 20 );
    auto future2 = pool.enqueue( []( std::string const& str1, std::string const& str2 ){ return str1+str2; }, "Hello", " World" );

    REQUIRE( future1.get() == 30 );
    REQUIRE( future2.get() == "Hello World" );
}

TEST_CASE( "Test concurrency", "[Feel::Core::ThreadPool]" )
{
    using namespace Feel::Core;
    using namespace std::chrono_literals;

    ThreadPool pool( 10 );
    std::atomic<int> counter {0};

    std::vector<std::future<void>> futures;
    int numTasks = 1000;
    for ( int i = 0; i < numTasks; ++i )
    {
        futures.push_back( pool.enqueue( [&counter]() {
                                            std::this_thread::sleep_for( 100us );
                                            counter++;
                                        } ) );
    }

    //Wait for tasks to be done
    for ( auto & f : futures ) 
        f.get();

    REQUIRE( counter == numTasks );
}


TEST_CASE( "Test exception propagation", "[Feel::Core::ThreadPool]" )
{
    using namespace Feel::Core;

    ThreadPool pool;
    auto future = pool.enqueue( [](){ throw std::runtime_error("Test exception"); } );

    try
    {
        future.get();
        REQUIRE( false );
    }
    catch ( std::runtime_error const& e )
    {
        std::string msg = e.what();
        REQUIRE(  msg == "Test exception" );
    }
}


int main( int argc, char* argv[] )
{
    auto env = Feel::Core::createEnvironment( argc, argv );
    return Catch::Session().run( argc, argv );
}
