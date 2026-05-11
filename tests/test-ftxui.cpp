//!

#include <ftxui/component/component.hpp>
#include <iostream>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include <feel/feelcore/tui/taskmanager.hpp>
#include <feel/feelcore/tui/components.hpp>


#include <feel/feelcore/environment.hpp>
#include <stdexcept>

TEST_CASE( "Test Screen Basic", "[FTXUI]" )
{
    using namespace ftxui;
    Element document = hbox({
            text("left")   | border,
            text("middle") | border | flex,
            text("right")  | border,
        });
    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    screen.Print();
}

TEST_CASE( "Test Gauge", "[FTXUI]" )
{
    using namespace ftxui;
    using namespace std::chrono_literals;

    std::string reset_position;
    for (float percentage = 0.0f; percentage <= 1.0f; percentage += 0.01f) {
        std::string data_downloaded =
            std::to_string(int(percentage * 5000)) + "/5000";
        auto document = hbox({
                text("downloading:"),
                gauge(percentage) | flex,
                text(" " + data_downloaded),
            });
        auto screen = Screen(100, 1);
        Render(screen, document);
        std::cout << reset_position;
        screen.Print();
        reset_position = screen.ResetPosition();

        std::this_thread::sleep_for(0.01s);
    }
    std::cout << std::endl;
}





TEST_CASE( "Test MultiOptionSelector", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    bool option1 = true;
    bool option2 = false;
    bool option3 = true;
    std::vector<std::pair<std::string, bool*>> options = {
        { "option1", &option1 }, { "option2", &option2 }, { "option3", &option3 }
    };

    Component multiOptionSelector = MultiOptionSelector( options, "MultiOption Selector" );

    auto document = multiOptionSelector->Render();

    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );
    screen.Print();
}


TEST_CASE( "Test RadioSelector", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    int selected = 0;
    std::vector<std::string> options = { "option1", "option2", "option3" };

    Component radioSelector = RadioSelector( &options, &selected, "Radio Selector" );

    auto document = radioSelector->Render();

    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );
    screen.Print();
}


TEST_CASE( "Test ReadoutSlider", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    int intValue = 50;
    Component intSlider = ReadoutSlider<int>( &intValue, 0, 100, 1, "Int Readout Slider", 0 ); 

    float doubleValue = 0.5;
    Component doubleSlider = ReadoutSlider<float>( &doubleValue, 0, 1, 0.1, "Double Readout Slider", 2 ); 

    auto document = hbox( {
        intSlider->Render() | flex_grow,
        doubleSlider->Render() | flex_grow
    } ) | flex;

    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );
    screen.Print();
}


TEST_CASE( "Test SpinBox", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    int value = 0;
    Component spinBox = SpinBox( value, "SpinBox " ); 

    auto document = spinBox->Render();
    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );
    screen.Print();
}


TEST_CASE( "Test FileInput", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    std::string filepath;
    Component fileInput = FileInput( filepath, " Enter your filepath..." ); 

    auto document = fileInput->Render();
    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );
    screen.Print();


}

//================= Interactive ===================
TEST_CASE( "Test TaskManager Success", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;
    using namespace std::chrono_literals;

    auto screen = ScreenInteractive::TerminalOutput();

    AsyncUiTask task( [] { 
        return "Success!"; 
    }, screen );

    screen.Loop( Renderer([&screen, &task] {
        if ( task.getState().status == TaskStatus::IDLE )
            task.start();

        if ( task.getState().status != TaskStatus::WORKING )
            screen.Post( screen.ExitLoopClosure() );
        return text( "Testing async UI task ..." );
    } ) );


    REQUIRE( task.getState().status == TaskStatus::SUCCESS );
    REQUIRE( task.getState().result == "Success!" );
}


TEST_CASE( "Test TaskManager Failure", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;
    using namespace std::chrono_literals;

    auto screen = ScreenInteractive::TerminalOutput();

    AsyncUiTask task( [] -> std::string {
        throw std::runtime_error( "Failure" );
    }, screen );

    screen.Loop( Renderer([&screen, &task] {
        if ( task.getState().status == TaskStatus::IDLE )
            task.start();

        if ( task.getState().status != TaskStatus::WORKING )
            screen.Post( screen.ExitLoopClosure() );
        return text( "Testing async UI task ..." );
    } ) );


    REQUIRE( task.getState().status == TaskStatus::ERROR );
    REQUIRE( task.getState().result == "Error: Failure" );

}

TEST_CASE( "Test WorkerButton", "[FEELCORE-TUI]" )
{
}


TEST_CASE( "Test FileLoader", "[FEELCORE-TUI]" )
{
}


int main( int argc, char* argv[])
{
    auto env = Feel::Core::createEnvironment( argc, argv );
    return Catch::Session().run( argc, argv );
}

