//!

#pragma once

#include <ftxui/component/component.hpp>


namespace Feel::Core::ftxui
{

using namespace ::ftxui; 

//! Styled and labeled window with vertical checkboxes
Component MultiOptionSelector( std::vector<std::pair<std::string,bool*>> const& options, std::string const& label = "" );

//! Styled and labeled window with vertical radio buttons
Component RadioSelector( std::vector<std::string> const* entries, int * selected, std::string const& label = "" );


} //namespace Feel::Core::ftxui
