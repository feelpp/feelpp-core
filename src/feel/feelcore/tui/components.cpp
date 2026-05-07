//!

#include <fmt/format.h>

#include <feel/feelcore/tui/components.hpp>

#include <feel/feelcore/tui/fileinput.hpp>
#include <feel/feelcore/tui/taskmanager.hpp>


namespace Feel::Core::ftxui
{


Component
MultiOptionSelector( std::vector<std::pair<std::string,bool*>> const& options, std::string const& label )
{
    Component checkboxes = Container::Vertical( {} );
    for ( auto const& opt : options )
        checkboxes->Add( Checkbox( opt.first, opt.second ) );

    return Renderer( checkboxes, [label,checkboxes] {
        return window(
            text( fmt::format( " {} ", label ) ) | bold | center, 
            vbox( { checkboxes->Render() } )
        );
    } );
}

Component
RadioSelector( std::vector<std::string> const* entries, int * selected, std::string const& label )
{
    Component radiobox = Radiobox( entries, selected );

    return Renderer( radiobox, [label, radiobox] {
        return window(
            text( fmt::format( " {} ", label ) ) | bold | center,
            vbox( { radiobox->Render() })
        );
    } );
}


template <typename T>
Component
ReadoutSlider( Ref<T> value, T min, T max, T step, std::string const& title, int displayPrecision )
{
    Component slider = Slider( "", value, min, max, step );

    return Renderer( slider, [slider, value, title, displayPrecision] {
        return vbox( {
            text( title ) | dim,
            hbox( {
                slider->Render() | flex,
                text( fmt::format("{:>3.{}f}", static_cast<float>( *value ), displayPrecision ) ) | size( WIDTH, EQUAL, 6 ) 
            } ),
            separatorEmpty()
        } );
    } );
}
template Component ReadoutSlider<float>( Ref<float>, float, float, float, const std::string&, int );
template Component ReadoutSlider<int>( Ref<int>, int, int, int, const std::string&, int );



Component
SpinBox( int & value, std::string const& title )
{
    Component inputMinus = Button( "-", [&value]{ value--; }, ButtonOption::Ascii() );
    Component inputPlus = Button( "+", [&value]{ value++; }, ButtonOption::Ascii() );
    Component stepperContainer = Container::Horizontal( { inputMinus, inputPlus } );

    return Renderer( stepperContainer, [&value, inputMinus, inputPlus, title] {
        return hbox( {
            text( title ),
            hbox( {
                inputMinus->Render(),
                text( std::to_string( value ) ) | center,
                inputPlus->Render(),
            } ) | center,
        } );
    } );
}


Component
WorkerButton( ScreenInteractive & screen, std::function<std::string()> task, std::string const& label )
{
    auto asyncTask = std::make_shared<AsyncUiTask>( [task]{ return task(); }, screen );

    Component createMeshBtn = Button( label, [asyncTask]{ asyncTask->start(); } );
    Component buttonContainer = Container::Vertical( { createMeshBtn } );

    return Renderer( buttonContainer, [createMeshBtn, asyncTask] {
        return vbox( {
            createMeshBtn->Render(),
            asyncTask->getStateUiElement()
        } );
    } );

}


Component
FileInput( StringRef content, StringRef placeholder, InputOption options )
{
    return Make<FileInputComponent>(content, placeholder, options);
}



Component FileLoader( ScreenInteractive & screen, StringRef content, IFileLoaderHandler & loadHandler,
                      StringRef placeholder, InputOption inputOptions )
{


    auto onLoadTask = std::make_shared<AsyncUiTask>(
        [content, &loadHandler] -> std::string
        {
            auto contentPath = fs::path( *content );
            if ( !fs::exists(contentPath) )  //Todo add custom check, eg if empty, or if file, or if dir
                throw std::runtime_error("Could not load."); 
            return loadHandler.load(contentPath);
        },
        screen
    );

    auto onUnloadTask = std::make_shared<AsyncUiTask>( [&loadHandler]{ return loadHandler.unload();} , screen );

    inputOptions.on_enter = [onLoadTask, onUnloadTask]{
        onUnloadTask->reset();
        onLoadTask->start(); 
    };

    Component fileInput = FileInput( content, placeholder, inputOptions );


    Component loadButton = Button("Load", [onLoadTask, onUnloadTask]{
        onUnloadTask->reset();
        onLoadTask->start();
    });
    Component unloadButton = Button("Unload", [onLoadTask, onUnloadTask]{
        onLoadTask->reset();
        onUnloadTask->start();
    });

    Component fileLoaderContainer = Container::Horizontal({ fileInput, loadButton, unloadButton });

    return Renderer(fileLoaderContainer, [=] {
        auto & loadTaskState = onLoadTask->getState();
        auto & unloadTaskState = onUnloadTask->getState();

        auto getStatusText = [&loadTaskState,&unloadTaskState] -> Element
        {
            if ( loadTaskState.status == TaskStatus::SUCCESS )
                return text(loadTaskState.result) | color(Color::Green);
            if ( unloadTaskState.status == TaskStatus::SUCCESS )
                return text(unloadTaskState.result) | color(Color::Green);

            if ( loadTaskState.status == TaskStatus::ERROR )
                return text(loadTaskState.result) | color(Color::Red);
            if ( unloadTaskState.status == TaskStatus::ERROR )
                return text(unloadTaskState.result) | color(Color::Red);

            if ( loadTaskState.status == TaskStatus::WORKING )
                return hbox({ text("Loading "), spinner(8,loadTaskState.loadingFrameCount) }) | color(Color::Yellow);
            if ( unloadTaskState.status == TaskStatus::WORKING )
                return hbox({ text("Unloading "), spinner(8,unloadTaskState.loadingFrameCount) }) | color(Color::Yellow);

            return text("");
        };

        return vbox({
            window(text(" File Loader ") | bold | center, 
                vbox({
                    hbox({
                        text(" Path: ") | vcenter, 
                        fileInput->Render() | xflex,
                        text("   "),
                        loadButton->Render(),
                        text(" "),
                        unloadButton->Render()
                    }),
                    separator(),
                    getStatusText() | center | yflex_shrink
                })
            ),
            filler()
        });
    });
}





} //namespace Feel::Core::ftxui
