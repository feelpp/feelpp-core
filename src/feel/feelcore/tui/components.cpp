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
        return window( text( fmt::format( " {} ", label ) ) | bold | center, 
            vbox( { checkboxes->Render() } )
        );
    });
}

Component
RadioSelector( std::vector<std::string> const* entries, int * selected, std::string const& label )
{
    Component radiobox = Radiobox( entries, selected );

    return Renderer( radiobox, [label, radiobox] {
        return window( text( fmt::format( " {} ", label ) ) | bold | center,
            vbox( { radiobox->Render() })
        );
    });
}



} //namespace Feel::Core::ftxui
