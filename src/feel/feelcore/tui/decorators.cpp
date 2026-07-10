//!


#include <ftxui/component/event.hpp>

#include <feel/feelcore/tui/decorators.hpp>


namespace Feel::Core::tui
{

ftxui::Element
ScrollableList::OnRender()
{
    std::vector<ftxui::Element> listElements = buildElements();

    int maxScroll = std::max( 0, ( int )listElements.size() - 1 );
    M_scrollState = std::clamp( M_scrollState, 0, maxScroll );

    if ( !listElements.empty() )
        listElements[ M_scrollState ] |= ftxui::focus;

    if ( M_autoScroll && M_scrollState >= (int)listElements.size() - 2 )
        M_scrollState = (int)listElements.size();

    ftxui::Elements content;

    if ( auto header = buildHeader() )
    {
        content.push_back( *header );
        content.push_back( ftxui::separatorLight() );
    }

    content.push_back( ftxui::vbox( listElements ) | ftxui::vscroll_indicator
                                                   | ftxui::yframe
                                                   | ftxui::reflect( M_box )
                                                   | ftxui::flex
    );

    if ( auto footer = buildFooter() )
    {
        content.push_back( ftxui::separatorLight() );
        content.push_back( *footer );
    }

    return ftxui::window(
        ftxui::text( M_title ) | ftxui::bold,
        ftxui::vbox( content )
    );

}


bool
ScrollableList::OnEvent( ftxui::Event event )
{
    if ( ComponentBase::OnEvent( event ) )
        return true;

    if ( !event.is_mouse() )
        return false;

    if ( !M_box.Contain(event.mouse().x, event.mouse().y) )
        return false;

    if ( event.mouse().button == ftxui::Mouse::WheelDown )
    {
        M_scrollState += 1;
        return true;
    }
    if ( event.mouse().button == ftxui::Mouse::WheelUp )
    {
        M_scrollState -= 1;
        return true;
    }

    return false;
}


}
