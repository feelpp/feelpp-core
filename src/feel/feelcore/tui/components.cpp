//!

#include <fmt/format.h>

#include <feel/feelcore/tui/components.hpp>


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

} //namespace Feel::Core::ftxui
