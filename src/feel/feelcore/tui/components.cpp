//!

#include <fmt/format.h>

#include <feel/feelcore/tui/components.hpp>

#include <feel/feelcore/tui/fileinput.hpp>
#include <feel/feelcore/tui/taskmanager.hpp>


namespace Feel::Core::tui
{


ftxui::Component
MultiOptionSelector( std::vector<std::pair<std::string,bool*>> const& options, std::string const& label )
{
    ftxui::Component checkboxes = ftxui::Container::Vertical( {} );
    for ( auto const& opt : options )
        checkboxes->Add( ftxui::Checkbox( opt.first, opt.second ) );

    return ftxui::Renderer( checkboxes, [label,checkboxes] {
        return ftxui::window(
            ftxui::text( fmt::format( " {} ", label ) ) | ftxui::bold | ftxui::center, 
            ftxui::vbox( { checkboxes->Render() } )
        );
    } );
}

ftxui::Component
RadioSelector( std::vector<std::string> const* entries, int * selected, std::string const& label )
{
    ftxui::Component radiobox = ftxui::Radiobox( entries, selected );

    return ftxui::Renderer( radiobox, [label, radiobox] {
        return ftxui::window(
            ftxui::text( fmt::format( " {} ", label ) ) | ftxui::bold | ftxui::center,
            ftxui::vbox( { radiobox->Render() })
        );
    } );
}


template <typename T>
ftxui::Component
ReadoutSlider( ftxui::Ref<T> value, T min, T max, T step, std::string const& title, int displayPrecision )
{
    ftxui::Component slider = Slider( "", value, min, max, step );

    return ftxui::Renderer( slider, [slider, value, title, displayPrecision] {
        return ftxui::vbox( {
            ftxui::text( title ) | ftxui::dim,
            ftxui::hbox( {
                slider->Render() | ftxui::flex,
                ftxui::text( fmt::format("{:>3.{}f}", static_cast<float>( *value ), displayPrecision ) ) | size( ftxui::WIDTH, ftxui::EQUAL, 6 ) 
            } ),
            ftxui::separatorEmpty()
        } );
    } );
}
template FEELPP_CORE_EXPORT ftxui::Component ReadoutSlider<float>( ftxui::Ref<float>, float, float, float, const std::string&, int );
template FEELPP_CORE_EXPORT ftxui::Component ReadoutSlider<int>( ftxui::Ref<int>, int, int, int, const std::string&, int );



ftxui::Component
SpinBox( int & value, std::string const& title )
{
    //TODO: Make this template to accept float
    ftxui::Component inputMinus = Button( "-", [&value]{ value--; }, ftxui::ButtonOption::Ascii() );
    ftxui::Component inputPlus = Button( "+", [&value]{ value++; }, ftxui::ButtonOption::Ascii() );
    ftxui::Component stepperContainer = ftxui::Container::Horizontal( { inputMinus, inputPlus } );

    return ftxui::Renderer( stepperContainer, [&value, inputMinus, inputPlus, title] {
        return ftxui::hbox( {
            ftxui::text( title ),
            ftxui::hbox( {
                inputMinus->Render(),
                ftxui::text( std::to_string( value ) ) | ftxui::center,
                inputPlus->Render(),
            } ) | ftxui::center,
        } );
    } );
}


ftxui::Component
WorkerButton( ftxui::ScreenInteractive & screen, std::function<std::string()> task, std::string const& label )
{
    auto asyncTask = std::make_shared<AsyncUiTask>( [task]{ return task(); }, screen );

    ftxui::Component createMeshBtn = ftxui::Button( label, [asyncTask]{ asyncTask->start(); } );
    ftxui::Component buttonContainer = ftxui::Container::Vertical( { createMeshBtn } );

    return ftxui::Renderer( buttonContainer, [createMeshBtn, asyncTask] {
        return ftxui::vbox( {
            createMeshBtn->Render(),
            asyncTask->getStateUiElement()
        } );
    } );

}


ftxui::Component
FileInput( ftxui::StringRef content, ftxui::StringRef placeholder, ftxui::InputOption options )
{
    return Make<FileInputComponent>(content, placeholder, options);
}



ftxui::Component FileLoader( ftxui::ScreenInteractive & screen, ftxui::StringRef content, IFileLoaderHandler & loadHandler,
                      ftxui::StringRef placeholder, ftxui::InputOption inputOptions )
{
    auto onLoadTask = std::make_shared<AsyncUiTask>( [content, &loadHandler] () -> std::string {
            auto contentPath = fs::path( *content );
            if ( !fs::exists( contentPath ) )  //Todo add custom check, eg if empty, or if file, or if dir
                throw std::runtime_error( "Could not load." );
            return loadHandler.load( contentPath );
        },
        screen
    );

    auto onUnloadTask = std::make_shared<AsyncUiTask>( [&loadHandler]{ return loadHandler.unload(); } , screen );

    inputOptions.on_enter = [onLoadTask, onUnloadTask] {
        onUnloadTask->reset();
        onLoadTask->start();
    };

    ftxui::Component fileInput = FileInput( content, placeholder, inputOptions );


    ftxui::Component loadButton = ftxui::Button("Load", [onLoadTask, onUnloadTask] {
        onUnloadTask->reset();
        onLoadTask->start();
    });
    ftxui::Component unloadButton = ftxui::Button("Unload", [onLoadTask, onUnloadTask] {
        onLoadTask->reset();
        onUnloadTask->start();
    });

    ftxui::Component fileLoaderContainer = ftxui::Container::Horizontal( { fileInput, loadButton, unloadButton } );

    return Renderer(fileLoaderContainer, [=] {
        auto & loadTaskState = onLoadTask->getState();
        auto & unloadTaskState = onUnloadTask->getState();

        auto getStatusText = [&loadTaskState,&unloadTaskState] () -> ftxui::Element
        {
            if ( loadTaskState.status == TaskStatus::SUCCESS )
                return ftxui::text( loadTaskState.result ) | color( ftxui::Color::Green );
            if ( unloadTaskState.status == TaskStatus::SUCCESS )
                return ftxui::text( unloadTaskState.result ) | color( ftxui::Color::Green );

            if ( loadTaskState.status == TaskStatus::ERROR )
                return ftxui::text( loadTaskState.result ) | color( ftxui::Color::Red );
            if ( unloadTaskState.status == TaskStatus::ERROR )
                return ftxui::text( unloadTaskState.result ) | color( ftxui::Color::Red );

            if ( loadTaskState.status == TaskStatus::WORKING )
                return ftxui::hbox( { ftxui::text( "Loading " ), ftxui::spinner( 8, loadTaskState.loadingFrameCount ) } ) | ftxui::color( ftxui::Color::Yellow );
            if ( unloadTaskState.status == TaskStatus::WORKING )
                return ftxui::hbox( { ftxui::text( "Unloading " ), ftxui::spinner( 8, unloadTaskState.loadingFrameCount ) } ) | ftxui::color( ftxui::Color::Yellow );

            return ftxui::text( "" );
        };

        return ftxui::vbox( {
            window( ftxui::text( " File Loader " ) | ftxui::bold | ftxui::center, 
                ftxui::vbox( {
                    ftxui::hbox( {
                        ftxui::text( " Path: " ) | ftxui::vcenter, 
                        fileInput->Render() | ftxui::xflex,
                        ftxui::text( "   " ),
                        loadButton->Render(),
                        ftxui::text( " " ),
                        unloadButton->Render()
                    } ),
                    ftxui::separator(),
                    getStatusText() | ftxui::center | ftxui::yflex_shrink
                } )
            ),
            ftxui::filler()
        } );
    } );
}





} //namespace Feel::Core::tui
