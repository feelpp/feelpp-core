//!

#pragma once


#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <feel/feelcore/feelcore.hpp>


namespace Feel::Core::tui
{

struct AutocompleteState
{
    std::string last_input;
    std::string parent_str;
    std::vector<std::string> matches;
    std::size_t cycle_index = 0;
    bool cycling = false;
};


//! Component that handles text input to select files and autocompletes/cycles through options
class FileInputComponent
    : public ::ftxui::ComponentBase
{
public:
    FileInputComponent( ::ftxui::StringRef content, ::ftxui::StringRef placeholder = {}, ::ftxui::InputOption options = {} )
        : M_content( content ), M_options( options )
    {
        M_input = Input( content, placeholder, options );
        this->Add( M_input );
    };

    //! Is triggered on any input event such as typing
    bool OnEvent( ::ftxui::Event event ) override;

private:

    //! Stops cycling, completes the event and executes the on_enter option input options 
    bool onReturn();

    //! Handles file and directory name autocompletion from disk
    bool handleAutocomplete( ::ftxui::Event event );

    //! Selects the closest match to a given string inside a directory and replaces the component content
    void findMatches( std::string const& currentInput, ::ftxui::Event event );

    //! Selects the next match inside a directory and replaces the component content
    void cycleMatches( ::ftxui::Event event );

    //! filters the longest common prefix in a given vector of strings
    std::string longestCommonPrefix( std::vector<std::string> const& strings );

    //! Expands the tile character (~) to the home directory
    std::string expandTilde( std::string const& pathStr );


private:
    ::ftxui::StringRef M_content;
    ::ftxui::InputOption M_options;
    ::ftxui::Component M_input;

    AutocompleteState M_autocompleteState; 

};

} //namespace Feel::Core::ftxui
