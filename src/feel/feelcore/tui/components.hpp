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

template <typename T>
Component ReadoutSlider( Ref<T> value, T min = 0., T max = 1., T step = 0.1,
                         std::string const& title = "", int displayPrecision = 1 );

Component SpinBox( int & value, std::string const& title = "" );


} //namespace Feel::Core::ftxui
