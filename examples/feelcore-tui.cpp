//!

#include <fstream>

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include <feel/feelcore/tui/taskmanager.hpp>
#include <feel/feelcore/tui/components.hpp>
#include <feel/feelcore/timekeeper.hpp>

#include <feel/feelcore/feelcore.hpp>



int main( int argc, char** argv )
{
    using namespace Feel::Core::tui;
    using namespace std::chrono_literals;
    using namespace ftxui;

    auto screen = ScreenInteractive::TerminalOutput();


    // ====== MULTI OPTION SELECTOR ========= //
    bool option1 = true;
    bool option2 = false;
    bool option3 = true;
    std::vector<std::pair<std::string, bool*>> checkboxOptions = {
        { "option1", &option1 }, { "option2", &option2 }, { "option3", &option3 }
    };

    Component multiOptionSelector = MultiOptionSelector( checkboxOptions, "MultiOption Selector" );
    //=======================================//


    // ========== RADIO SELECTOR ========= //
    int radioSelected = 0;
    std::vector<std::string> radioOptions = { "option1", "option2", "option3" };

    Component radioSelector = RadioSelector( &radioOptions, &radioSelected, "Radio Selector" );
    //=======================================//

    // ========== READOUT SLIDERS ========= //
    int intValue = 50;
    Component intSlider = ReadoutSlider<int>( &intValue, 0, 100, 1, "Int Readout Slider", 0 );

    float doubleValue = 0.5;
    Component doubleSlider = ReadoutSlider<float>( &doubleValue, 0, 1, 0.1, "Double Readout Slider", 2 );

    Component readoutSliders = Container::Vertical( { intSlider, doubleSlider } );
    //=======================================//


    //============= SPINBOX ==============//
    int spinboxValue = 0;
    Component spinbox = SpinBox( spinboxValue, "SpinBox " );
    //=======================================//

    //============= FILE INPUT ==============//
    std::string filepath;
    Component fileInput = FileInput( &filepath, " Enter your filepath..." );
    //=======================================//



    //=========== WORKER BUTTONS ===============//

    auto successTask = []() -> std::string
    {
        std::this_thread::sleep_for( 1s );
        return "Success!";
    };
    Component successButton = WorkerButton( screen, successTask, "Click for success" );

    auto failureTask = []() -> std::string {
        std::this_thread::sleep_for( 1s );
        throw std::runtime_error( "Failure" );
    };
    Component errorButton = WorkerButton( screen, failureTask, "Click for failure" );

    Component workerButtons = Container::Horizontal( { successButton, errorButton } );

    //=======================================//


    //==================== FILE LOADER =============== //

    class LineCounter
        : public IFileLoaderHandler
    {
    public:
        std::string load( Feel::fs::path const& fp ) override
        {
            int nLines = 0;
            std::ifstream f( fp );
            std::string line;
            while ( std::getline( f, line ) )
                ++nLines;
            return fmt::format("Your file contains {} lines;", nLines );
        }

        std::string unload() override { return "Your file was unloaded"; }
    };

    LineCounter lineCounter;
    std::string fileLoaderFilename;
    Component fileLoader = FileLoader( screen, &fileLoaderFilename, lineCounter, "Enter the filepath..." );

    //=======================================//


    //================== LOGVIEWER ==================
    auto logViewer = TuiLogViewer( 100 );
    spdlog::set_level( spdlog::level::debug );
    spdlog::info( "This is an info message" );
    spdlog::warn( "This is a warning message" );
    spdlog::error( "This is an error message" );
    spdlog::debug( "This is a debug message" );

    //===============================================//

    //============ TIMEKEEPER VIEWER ==================
    Feel::Core::Timekeeper::instance()->setEnabled( true );
    auto timekeeperViewer = TuiTimekeeperViewer( 3 );
    Component startTimersButton = WorkerButton( screen, []() -> std::string {
        {
            Feel::Core::Timer t1( "Task 1" );
            std::this_thread::sleep_for( 500ms );
            {
                Feel::Core::Timer t2( "Task 1.1" );
                std::this_thread::sleep_for( 200ms );
            }
            {
                Feel::Core::Timer t3( "Task 1.2" );
                std::this_thread::sleep_for( 300ms );
            }
        }
        {
            Feel::Core::Timer t4( "Task 2" );
            std::this_thread::sleep_for( 400ms );
        }
        return "Timers completed";
    }, "Start Timers" );
    Component timekeeperContainer = Container::Horizontal( { timekeeperViewer, startTimersButton} );

    //===============================================//

    //================== TABS ==================
    int selectedTab = 0;
    std::vector<std::string> inputTabs = {
        "MultiOptionSelector",
        "RadioSelector",
        "ReadoutSlider",
        "SpinBox",
        "FileInput",
        "WorkerButton",
        "FileLoader",
        "LogViewer",
        "TimekeeperViewer"
    };

    Component tabsMenu = Menu( &inputTabs, &selectedTab );

    Component tabsContainer = Container::Tab( {
        multiOptionSelector,
        radioSelector,
        readoutSliders,
        spinbox,
        fileInput,
        workerButtons,
        fileLoader,
        logViewer,
        timekeeperContainer
    }, &selectedTab );

    //=======================================//

    Component masterContainer = Container::Horizontal({ tabsMenu, tabsContainer });

    auto layout = Renderer( masterContainer, [&tabsMenu, &tabsContainer]() {
        return hbox( {
            tabsMenu->Render(),
            separator(),
            tabsContainer->Render() | flex
        } );
    } );


    screen.Loop(layout);


    return 0;
}
