//!

#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <feel/feelcore/feelcore.hpp>

namespace Feel::Core::tui
{

//! Styled and labeled window with vertical checkboxes
ftxui::Component FEELPP_CORE_EXPORT MultiOptionSelector( std::vector<std::pair<std::string,bool*>> const& options, std::string const& label = "" );

//! Styled and labeled window with vertical radio buttons
ftxui::Component FEELPP_CORE_EXPORT RadioSelector( std::vector<std::string> const* entries, int * selected, std::string const& label = "" );

//! Styled and labeled slider that displays the current value 
template <typename T>
ftxui::Component ReadoutSlider( ftxui::Ref<T> value, T min = 0., T max = 1., T step = 0.1,
                                std::string const& title = "", int displayPrecision = 1 );

//! Styled and labeled spinbox to decrease or increment an int
ftxui::Component FEELPP_CORE_EXPORT SpinBox( int & value, std::string const& title = "" );


//! Button that executes a background task. Displays success/error messages returned by the task and shows loading animation  
ftxui::Component FEELPP_CORE_EXPORT WorkerButton( ftxui::ScreenInteractive & screen, std::function<std::string()> task, std::string const& label = "" );

//! Text input component with autocompletion integrated (using tabs)
ftxui::Component FEELPP_CORE_EXPORT FileInput( ftxui::StringRef content, ftxui::StringRef placeholder, ftxui::InputOption options = {} );

class FEELPP_CORE_EXPORT IFileLoaderHandler
{
public:
    virtual ~IFileLoaderHandler() = default;
    virtual std::string load( fs::path const& fp ) = 0;
    virtual std::string unload() = 0;
};

//! Container containing a file input with load + unload buttons and feedback
ftxui::Component FEELPP_CORE_EXPORT FileLoader( ftxui::ScreenInteractive & screen, ftxui::StringRef content, IFileLoaderHandler & loadHandler,
                                                ftxui::StringRef placeholder = "", ftxui::InputOption inputOptions = {} );

} //namespace Feel::Core::tui
