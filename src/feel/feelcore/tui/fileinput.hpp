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

    bool OnEvent( Event event ) override;


private:
    bool onReturn();
    bool handleAutocomplete( Event event );
    void findMatches( std::string const& currentInput, Event event );
    void cycleMatches( Event event );

    std::string longestCommonPrefix( std::vector<std::string> const& strings );
    std::string expandTilde( std::string const& pathStr );


private:
    StringRef M_content;
    InputOption M_options;
    Component M_input;

    AutocompleteState M_autocompleteState; 

};

} //namespace Feel::Core::ftxui
