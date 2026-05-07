//!

#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <feel/feelcore/feelcore.hpp>

namespace Feel::Core::ftxui
{

using namespace ::ftxui; 

//! Styled and labeled window with vertical checkboxes
Component MultiOptionSelector( std::vector<std::pair<std::string,bool*>> const& options, std::string const& label = "" );

//! Styled and labeled window with vertical radio buttons
Component RadioSelector( std::vector<std::string> const* entries, int * selected, std::string const& label = "" );

//! Styled and labeled slider that displays the current value 
template <typename T>
Component ReadoutSlider( Ref<T> value, T min = 0., T max = 1., T step = 0.1,
                         std::string const& title = "", int displayPrecision = 1 );

//! Styled and labeled spinbox to decrease or increment an int
Component SpinBox( int & value, std::string const& title = "" );


//! Button that executes a background task. Displays success/error messages returned by the task and shows loading animation  
Component WorkerButton( ScreenInteractive & screen, std::function<std::string()> task, std::string const& label = "" );

//! Text input component with autocompletion integrated (using tabs)
Component FileInput( StringRef content, StringRef placeholder, InputOption options );

class IFileLoaderHandler
{
public:
    virtual ~IFileLoaderHandler() = default;
    virtual std::string load( fs::path const& fp ) = 0;
    virtual std::string unload() = 0;
};

//! Container containing a file input with load + unload buttons and feedback
Component FileLoader( ScreenInteractive & screen, StringRef content, IFileLoaderHandler & loadHandler,
                      StringRef placeholder = "", InputOption inputOptions = {} );

} //namespace Feel::Core::ftxui
