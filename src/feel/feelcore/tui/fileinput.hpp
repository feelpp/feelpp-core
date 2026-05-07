//!

#pragma once


#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>

#include <feel/feelcore/feelcore.hpp>


namespace Feel::Core::ftxui
{
using namespace ::ftxui;

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
    : public ComponentBase
{
public:
    FileInputComponent( StringRef content, StringRef placeholder = {}, InputOption options = {} )
        : M_content( content ), M_options( options )
    {
        M_input = Input( content, placeholder, options );
        this->Add( M_input );
    };

    //! Is triggered on any input event such as typing
    bool OnEvent( Event event ) override;

private:

    //! Stops cycling, completes the event and executes the on_enter option input options 
    bool onReturn();

    //! Handles file and directory name autocompletion from disk
    bool handleAutocomplete( Event event );

    //! Selects the closest match to a given string inside a directory and replaces the component content
    void findMatches( std::string const& currentInput, Event event );

    //! Selects the next match inside a directory and replaces the component content
    void cycleMatches( Event event );

    //! filters the longest common prefix in a given vector of strings
    std::string longestCommonPrefix( std::vector<std::string> const& strings );

    //! Expands the tile character (~) to the home directory
    std::string expandTilde( std::string const& pathStr );


private:
    StringRef M_content;
    InputOption M_options;
    Component M_input;

    AutocompleteState M_autocompleteState; 

};

} //namespace Feel::Core::ftxui
