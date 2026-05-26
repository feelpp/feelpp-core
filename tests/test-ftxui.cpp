//!

#include <fstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/loop.hpp>

#include <feel/feelcore/tui/taskmanager.hpp>
#include <feel/feelcore/tui/components.hpp>


#include <feel/feelcore/environment.hpp>



TEST_CASE( "Test MultiOptionSelector", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;

    bool option1 = true;
    bool option2 = false;
    bool option3 = true;
    std::vector<std::pair<std::string, bool*>> options = {
        { "option1", &option1 }, { "option2", &option2 }, { "option3", &option3 }
    };

    std::string title = "MultiOption Selector";
    ftxui::Component multiOptionSelector = MultiOptionSelector( options, title );

    auto document = multiOptionSelector->Render();

    auto screen = ftxui::Screen::Create( ftxui::Dimension::Full(), ftxui::Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( title ) );
    for ( auto & [label, v] : options )
        REQUIRE( screenContent.contains( label ) );


    //Cursor should already be in option one
    bool evtHandled = multiOptionSelector->OnEvent( ftxui::Event::Return );
    REQUIRE( evtHandled );
    REQUIRE( option1 == false );
    REQUIRE( option2 == false ); //unchanged
    REQUIRE( option3 == true ); //unchanged


    //Trigger second option
    multiOptionSelector->OnEvent( ftxui::Event::ArrowDown );
    multiOptionSelector->OnEvent( ftxui::Event::Return );
    REQUIRE( option1 == false );
    REQUIRE( option2 == true );
    REQUIRE( option3 == true ); //unchanged

}


TEST_CASE( "Test RadioSelector", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;

    int selected = 0;
    std::vector<std::string> options = { "option1", "option2", "option3" };

    std::string title = "Radio Selector";
    ftxui::Component radioSelector = RadioSelector( &options, &selected, title );

    auto document = radioSelector->Render();

    auto screen = ftxui::Screen::Create( ftxui::Dimension::Full(), ftxui::Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( title ) );
    for ( auto & label : options )
        REQUIRE( screenContent.contains( label ) );


    bool evtHandled = radioSelector->OnEvent( ftxui::Event::Return );
    REQUIRE( evtHandled );
    REQUIRE( selected == 0 );


    //Trigger last option (down two times)
    radioSelector->OnEvent( ftxui::Event::ArrowDown );
    radioSelector->OnEvent( ftxui::Event::ArrowDown );
    radioSelector->OnEvent( ftxui::Event::Return );
    REQUIRE( selected == 2 );
}


TEST_CASE( "Test ReadoutSlider", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;

    int intValue = 50;
    std::string intTitle = "Int Readout Slider";
    ftxui::Component intSlider = ReadoutSlider<int>( &intValue, 0, 100, 1, intTitle, 0 ); 

    float doubleValue = 0.5;
    std::string doubleTitle = "Double Readout Slider";
    ftxui::Component doubleSlider = ReadoutSlider<float>( &doubleValue, 0, 1, 0.1, doubleTitle, 2 ); 

    auto document = ftxui::hbox( {
        intSlider->Render(),
        doubleSlider->Render()
    } );

    auto screen = ftxui::Screen::Create( ftxui::Dimension::Full(), ftxui::Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( intTitle ) );
    REQUIRE( screenContent.contains( doubleTitle ) );

    //Check that initial values appear on screen
    REQUIRE( screenContent.contains( "50" ) );
    REQUIRE( screenContent.contains( "0.50" ) ); //Because display precision of 2

    bool evtHandled = intSlider->OnEvent( ftxui::Event::ArrowRight );
    REQUIRE( evtHandled );
    REQUIRE( intValue == 50 + 1 );
    intSlider->OnEvent( ftxui::Event::ArrowLeft );
    REQUIRE( intValue == 50 );

    doubleSlider->OnEvent( ftxui::Event::ArrowRight );
    REQUIRE( doubleValue == Catch::Approx( 0.5 + 0.1) );
    doubleSlider->OnEvent( ftxui::Event::ArrowLeft );
    REQUIRE( doubleValue == Catch::Approx( 0.5 ) );
}


TEST_CASE( "Test SpinBox", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;

    int value = 1;
    ftxui::Component spinBox = SpinBox( value, "SpinBox " ); 

    auto document = spinBox->Render();
    auto screen = ftxui::Screen::Create( ftxui::Dimension::Full(), ftxui::Dimension::Fit( document ) );
    Render( screen, document );

    auto screenContent = screen.ToString();

    REQUIRE( screenContent.contains("+") );
    REQUIRE( screenContent.contains("-") );


    bool evtHandled = spinBox->OnEvent( ftxui::Event::Return ); //cursor already on -, press it
    REQUIRE( evtHandled );
    REQUIRE( value == 0 );
    spinBox->OnEvent( ftxui::Event::ArrowRight ); //Position on the + elt
    spinBox->OnEvent( ftxui::Event::Return ); //Press on the + button
    spinBox->OnEvent( ftxui::Event::Return ); //Press on the + button
    REQUIRE( value == 2 );
}


TEST_CASE( "Test FileInput", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;

    std::string filepath;
    std::string placeholder = " Enter your filepath...";
    ftxui::Component fileInput = FileInput( &filepath, placeholder ); 

    auto document = fileInput->Render();
    auto screen = ftxui::Screen::Create( ftxui::Dimension::Full(), ftxui::Dimension::Fit( document ) );
    Render( screen, document );

    std::string screenContent = screen.ToString();

    REQUIRE( screenContent.contains( placeholder ) );

    bool evtHandled = fileInput->OnEvent( ftxui::Event::Character("./") );
    REQUIRE( evtHandled );

    //Check cycling
    fileInput->OnEvent( ftxui::Event::Tab );
    REQUIRE( filepath != "./" ); 
    REQUIRE( Feel::fs::exists( filepath ) );
    std::string oldPath = filepath;
    fileInput->OnEvent( ftxui::Event::Tab );
    REQUIRE(oldPath != filepath );
    REQUIRE( Feel::fs::exists( filepath ) );

}

//================= Interactive ===================
TEST_CASE( "Test TaskManager Success", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;

    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    AsyncUiTask task( [] { 
        return "Success!"; 
    }, screen );

    screen.Loop( ftxui::Renderer([&screen, &task] {
        if ( task.getState().status == TaskStatus::IDLE )
            task.start();
        else if ( task.getState().status != TaskStatus::WORKING )
            screen.Post( screen.ExitLoopClosure() );
        return ftxui::text( "Testing async UI task ..." );
    } ) );


    REQUIRE( task.getState().status == TaskStatus::SUCCESS );
    REQUIRE( task.getState().result == "Success!" );
}


TEST_CASE( "Test TaskManager Failure", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;

    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    AsyncUiTask task( [] -> std::string {
        throw std::runtime_error( "Failure" );
    }, screen );

    screen.Loop( ftxui::Renderer([&screen, &task] {
        if ( task.getState().status == TaskStatus::IDLE )
            task.start();
        else if ( task.getState().status != TaskStatus::WORKING )
            screen.Post( screen.ExitLoopClosure() );
        return ftxui::text( "Testing async UI task ..." );
    } ) );


    REQUIRE( task.getState().status == TaskStatus::ERROR );
    REQUIRE( task.getState().result == "Error: Failure" );

}


std::string
renderInteractiveComponent( ftxui::Component & component )
{
    using namespace Feel::Core::tui;
    auto screen = ftxui::Screen::Create( ftxui::Dimension::Full()  );
    Render( screen, component->Render() );
    return screen.ToString();
}


//! Runs a screen loop until given text is found for an interactive component. Returns true if found.
bool
waitForText( ftxui::Loop & loop, ftxui::Component & component, std::string const& text, int timeoutMs = 1000 )
{
    using namespace std::chrono_literals;

    int elapsed = 0;
    while ( elapsed < timeoutMs )
    {
        loop.RunOnce();
        if ( renderInteractiveComponent( component ).contains( text ) )
            return true;
        std::this_thread::sleep_for( 10ms );
        elapsed += 10;
    }
    return false;
}



TEST_CASE( "Test WorkerButton", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;
    using namespace std::chrono_literals;

    auto screen = ftxui::ScreenInteractive::FitComponent();

    std::atomic<bool> taskFinished;
    auto successTask = [&taskFinished] -> std::string
    { 
        std::this_thread::sleep_for( 50ms );
        taskFinished = true;
        return "Success!"; 
    };

    std::string btnTitle = "Click";
    ftxui::Component successButton = WorkerButton( screen, successTask, btnTitle );

    auto renderer = Renderer( successButton, [&successButton]{ return successButton->Render(); } );

    ftxui::Loop loop( &screen, renderer );

    REQUIRE(  renderInteractiveComponent( successButton ).contains( btnTitle ) );
    successButton->OnEvent( ftxui::Event::Return );
    loop.RunOnce();
    REQUIRE( renderInteractiveComponent( successButton ).contains( "Loading" ) );
    REQUIRE( waitForText( loop, successButton, "Success!"  ) );

}


TEST_CASE( "Test FileLoader", "[FEELCORE-TUI]" )
{
    using namespace Feel::Core::tui;
    using namespace std::chrono_literals;

    Feel::fs::path successPath = "test_success_file.txt";
    Feel::fs::path missingPath = "test_missing_file.txt";

    struct ScopedFileCleanup {
        Feel::fs::path p1, p2;

        ~ScopedFileCleanup() {
            Feel::fs::remove(p1);
            Feel::fs::remove(p2);
        }
    } cleanup{ successPath, missingPath }; 
    std::ofstream(successPath) << "dummy data";
    Feel::fs::remove(missingPath);

    auto screen = ftxui::ScreenInteractive::FitComponent();

    class DummyLoader : public IFileLoaderHandler
    {
    public:
        std::string load( Feel::fs::path const& fp ) override {
            std::this_thread::sleep_for( 50ms );
            return "Loaded";
        }
        std::string unload() override {
            std::this_thread::sleep_for( 50ms );
            return "Unloaded";
        }
    };

    DummyLoader dummyLoader;
    std::string fileLoaderFilename;
    std::string placeholder = "Enter the filepath...";

    ftxui::Component fileLoader = FileLoader( screen, &fileLoaderFilename, dummyLoader, placeholder);
    auto renderer = Renderer( fileLoader, [fileLoader]{ return fileLoader->Render(); } );
    ftxui::Loop loop( &screen, renderer );

    REQUIRE( renderInteractiveComponent( fileLoader ).contains( placeholder ) );
    REQUIRE( renderInteractiveComponent( fileLoader ).contains( "Load" ) );
    REQUIRE( renderInteractiveComponent( fileLoader ).contains( "Unload" ) );

    //Check load invalid file
    fileLoaderFilename = missingPath.string(); 
    fileLoader->OnEvent( ftxui::Event::Return ); // Press Enter on the input
    REQUIRE( waitForText( loop, fileLoader, "Could not load." ) );

    //Check load
    fileLoaderFilename = successPath.string();
    fileLoader->OnEvent( ftxui::Event::Return ); 
    REQUIRE( waitForText( loop, fileLoader, "Loaded"  ) );

    //Check unload
    fileLoaderFilename = "";
    fileLoader->OnEvent( ftxui::Event::ArrowRight ); // Move to Load
    fileLoader->OnEvent( ftxui::Event::ArrowRight ); // Move to Unload
    fileLoader->OnEvent( ftxui::Event::Return ); // Press Unload
    REQUIRE( waitForText( loop, fileLoader, "Unloaded" ) );
}


int main( int argc, char* argv[])
{
    auto env = Feel::Core::createEnvironment( argc, argv );
    return Catch::Session().run( argc, argv );
}

