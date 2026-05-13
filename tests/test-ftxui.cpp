//!

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

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
#include <thread>



TEST_CASE( "Test MultiOptionSelector", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    bool option1 = true;
    bool option2 = false;
    bool option3 = true;
    std::vector<std::pair<std::string, bool*>> options = {
        { "option1", &option1 }, { "option2", &option2 }, { "option3", &option3 }
    };

    std::string title = "MultiOption Selector";
    Component multiOptionSelector = MultiOptionSelector( options, title );

    auto document = multiOptionSelector->Render();

    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( title ) );
    for ( auto & [label, v] : options )
        REQUIRE( screenContent.contains( label ) );


    //Cursor should already be in option one
    bool evtHandled = multiOptionSelector->OnEvent( Event::Return );
    REQUIRE( evtHandled );
    REQUIRE( option1 == false );
    REQUIRE( option2 == false ); //unchanged
    REQUIRE( option3 == true ); //unchanged


    //Trigger second option
    multiOptionSelector->OnEvent( Event::ArrowDown );
    multiOptionSelector->OnEvent( Event::Return );
    REQUIRE( option1 == false );
    REQUIRE( option2 == true );
    REQUIRE( option3 == true ); //unchanged

}


TEST_CASE( "Test RadioSelector", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    int selected = 0;
    std::vector<std::string> options = { "option1", "option2", "option3" };

    std::string title = "Radio Selector";
    Component radioSelector = RadioSelector( &options, &selected, title );

    auto document = radioSelector->Render();

    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( title ) );
    for ( auto & label : options )
        REQUIRE( screenContent.contains( label ) );


    bool evtHandled = radioSelector->OnEvent( Event::Return );
    REQUIRE( evtHandled );
    REQUIRE( selected == 0 );


    //Trigger last option (down two times)
    radioSelector->OnEvent( Event::ArrowDown );
    radioSelector->OnEvent( Event::ArrowDown );
    radioSelector->OnEvent( Event::Return );
    REQUIRE( selected == 2 );
}


TEST_CASE( "Test ReadoutSlider", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    int intValue = 50;
    std::string intTitle = "Int Readout Slider";
    Component intSlider = ReadoutSlider<int>( &intValue, 0, 100, 1, intTitle, 0 ); 

    float doubleValue = 0.5;
    std::string doubleTitle = "Double Readout Slider";
    Component doubleSlider = ReadoutSlider<float>( &doubleValue, 0, 1, 0.1, doubleTitle, 2 ); 

    auto document = hbox( {
        intSlider->Render() | flex_grow,
        doubleSlider->Render() | flex_grow
    } ) | flex;

    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( intTitle ) );
    REQUIRE( screenContent.contains( doubleTitle ) );

    //Check that initial values appear on screen
    REQUIRE( screenContent.contains( "50" ) );
    REQUIRE( screenContent.contains( "0.50" ) ); //Because display precision of 2

    bool evtHandled = intSlider->OnEvent( Event::ArrowRight );
    REQUIRE( evtHandled );
    REQUIRE( intValue == 50 + 1 );
    intSlider->OnEvent( Event::ArrowLeft );
    REQUIRE( intValue == 50 );

    doubleSlider->OnEvent( Event::ArrowRight );
    REQUIRE( doubleValue == Catch::Approx( 0.5 + 0.1) );
    doubleSlider->OnEvent( Event::ArrowLeft );
    REQUIRE( doubleValue == Catch::Approx( 0.5 ) );
}


TEST_CASE( "Test SpinBox", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    int value = 1;
    Component spinBox = SpinBox( value, "SpinBox " ); 

    auto document = spinBox->Render();
    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );

    auto screenContent = screen.ToString();

    REQUIRE( screenContent.contains("+") );
    REQUIRE( screenContent.contains("-") );


    bool evtHandled = spinBox->OnEvent( Event::Return ); //cursor already on -, press it
    REQUIRE( evtHandled );
    REQUIRE( value == 0 );
    spinBox->OnEvent( Event::ArrowRight ); //Position on the + elt
    spinBox->OnEvent( Event::Return ); //Press on the + button
    spinBox->OnEvent( Event::Return ); //Press on the + button
    REQUIRE( value == 2 );
}


TEST_CASE( "Test FileInput", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    std::string filepath;
    std::string placeholder = " Enter your filepath...";
    Component fileInput = FileInput( &filepath, placeholder ); 

    auto document = fileInput->Render();
    auto screen = Screen::Create( Dimension::Full(), Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( placeholder ) );

    bool evtHandled = fileInput->OnEvent( Event::Character("./") );
    REQUIRE( evtHandled );

    //Check cycling
    fileInput->OnEvent( Event::Tab );
    REQUIRE( filepath != "./" ); 
    REQUIRE( Feel::fs::exists( filepath ) );
    std::string oldPath = filepath;
    fileInput->OnEvent( Event::Tab );
    REQUIRE(oldPath != filepath );
    REQUIRE( Feel::fs::exists( filepath ) );

}

//================= Interactive ===================
TEST_CASE( "Test TaskManager Success", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    auto screen = ScreenInteractive::TerminalOutput();

    AsyncUiTask task( [] { 
        return "Success!"; 
    }, screen );

    screen.Loop( Renderer([&screen, &task] {
        if ( task.getState().status == TaskStatus::IDLE )
            task.start();
        else if ( task.getState().status != TaskStatus::WORKING )
            screen.Post( screen.ExitLoopClosure() );
        return text( "Testing async UI task ..." );
    } ) );


    REQUIRE( task.getState().status == TaskStatus::SUCCESS );
    REQUIRE( task.getState().result == "Success!" );
}


TEST_CASE( "Test TaskManager Failure", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::ftxui;

    auto screen = ScreenInteractive::TerminalOutput();

    AsyncUiTask task( [] -> std::string {
        throw std::runtime_error( "Failure" );
    }, screen );

    screen.Loop( Renderer([&screen, &task] {
        if ( task.getState().status == TaskStatus::IDLE )
            task.start();
        else if ( task.getState().status != TaskStatus::WORKING )
            screen.Post( screen.ExitLoopClosure() );
        return text( "Testing async UI task ..." );
    } ) );


    REQUIRE( task.getState().status == TaskStatus::ERROR );
    REQUIRE( task.getState().result == "Error: Failure" );

}

TEST_CASE( "Test WorkerButton", "[FEELCORE-TUI]" )
{
#if 0
    using namespace Feel::Core::ftxui;
    using namespace std::chrono_literals;


    auto screen = ScreenInteractive::FitComponent();
    auto successTask = [] -> std::string
    { 
        std::this_thread::sleep_for( 1s );
        return "Success!"; 
    };
   
    std::string btnTitle = "Click";
    Component successButton = WorkerButton( screen, successTask, btnTitle );

    std::string initialState;
    std::string loadingState;
    std::string successState;

    std::thread testSequence([&] {
        std::this_thread::sleep_for( 100ms );

        screen.Post( [&] {
            initialState = screen.ToString();
            screen.PostEvent( Event::Return );
        });

        std::this_thread::sleep_for( 200ms );
        screen.Post( [&] { loadingState = screen.ToString(); });

        std::this_thread::sleep_for( 1200ms );
        screen.Post( [&] {
            successState = screen.ToString();
            screen.ExitLoopClosure()();
        });
    });

    screen.Loop( Renderer(successButton, [&successButton]{ return successButton->Render(); } ) );

    if (testSequence.joinable())
        testSequence.join();
 
    REQUIRE( initialState.contains(btnTitle) );
    REQUIRE( loadingState.contains("Loading") );
    REQUIRE( successState.contains("Success!") );

    std::this_thread::sleep_for( 1200ms );

#endif
}


TEST_CASE( "Test FileLoader", "[FEELCORE-TUI]" )
{
}


int main( int argc, char* argv[])
{
    auto env = Feel::Core::createEnvironment( argc, argv );
    return Catch::Session().run( argc, argv );
}

