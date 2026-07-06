
//!

#pragma once

#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component.hpp>

#include <feel/feelcore/feelcore.hpp>

namespace Feel::Core::tui
{

namespace detail
{
    inline std::string
    toLower( std::string const& text )
    {
        std::string textLower = text;
        std::transform( textLower.begin(), textLower.end(), textLower.begin(),
                        []( unsigned char c ){ return std::tolower( c ); } );
        return textLower;
    }
};


//! Scrollable list base class with optional header and footer.
//! To be used as a base class and implement the buildElements() method to provide the list elements.
//! Will automatically handle scrolling and focus management of the list elements.
class ScrollableList
    : public ftxui::ComponentBase
{
    using Element = ftxui::Element;
public:

    //! @param title The title of the list.
    //! @param autoScroll If true, the list will automatically scroll to the bottom when new items are added (e.g for logging).
    explicit ScrollableList( std::string const& title = "", bool autoScroll = false )
        : M_title( title ), M_autoScroll( autoScroll )
    {}

    //! Build the list elements to be displayed in the scrollable list.
    virtual std::vector<Element> buildElements() = 0;

    //! Build the optional sticky header element to be displayed above the list.
    virtual std::optional<Element> buildHeader() { return std::nullopt; }

    //! Build the optional sticky footer element to be displayed below the list.
    virtual std::optional<Element> buildFooter() { return std::nullopt; }

    //! Assembles the list elements, header and footer into a single element to be rendered.
    Element OnRender() override;

    //! Handles scrolling and focus management of the list elements.
    bool OnEvent( ftxui::Event event ) override;

private:
    std::string M_title;
    int M_scrollState = 0;
    bool M_autoScroll = false;
    ftxui::Box M_box;
};



//! Scrollable list with search bar and optional bulk action buttons.
//! To be used as a base class and implement the onBulkAction() method to handle bulk
//! And optionally implement the buildElements() method to provide the list elements.
//! Will automatically handle filtering of the list elements based on the search query and scrolling and focus management of the list elements.
template <typename T>
class SearchableList
    : public ScrollableList
{
protected:
    struct Item
    {
        std::string searchTest;
        ftxui::Component component;
        T data;
    };

public:
    //! @param title The title of the list.
    //! @param autoScroll If true, the list will automatically scroll to the bottom when new items are added.
    //! @param enableBulkActions If true, bulk action buttons will be displayed below the search bar.
    explicit SearchableList( std::string const& title = "",
                                      bool autoScroll = false,
                                      bool enableBulkActions = false )
        : ScrollableList( title, autoScroll ), M_enableBulkActions( enableBulkActions )
    {
        M_searchInput = ftxui::Input( &M_searchQuery, "Search...", { .on_change = [this](){ applyFilter(); } } );

        if ( M_enableBulkActions )
        {
            initBulkActionButtons();
            ScrollableList::Add( ftxui::Container::Vertical( {
                M_searchInput,
                ftxui::Container::Horizontal( { M_selectAllBtn, M_deselectAllBtn } ),
                M_listContainer
            } ) );
        }
        else
            ScrollableList::Add( ftxui::Container::Vertical( { M_searchInput, M_listContainer } ) );
    }

    //! Add a data item to the list.
    //! @param searchTest The text to be used for filtering the item.
    //! @param component The component to be displayed for the item (e.g. ftxui::Checkbox)
    //! @param data The data associated with the item.
    void addItem( std::string const& searchTest, ftxui::Component component, T const& data = T() )
    {
        M_allItems.push_back( { searchTest, component, data } );
        applyFilter();
    }

    //! Clear all items from the list.
    void clearItems()
    {
        M_allItems.clear();
        applyFilter();
    }

protected:

    //! Called when the bulk action buttons are pressed.
    //! @param active If true, the "Select All" button was pressed, otherwise the "Deselect All" button was pressed.
    virtual void onBulkAction( bool active ) {}

    //! Get the filtered items after applying the search query.
    std::vector<Item*> const& filteredItems() const { return M_filteredItems; }

    //! Get the current search query.
    std::string const& searchQuery() const { return M_searchQuery; }

    //! Build the header element with the search bar and optional bulk action buttons.
    std::optional<ftxui::Element> buildHeader() override
    {
         auto searchbar = ftxui::hbox({ ftxui::text( "🔍 " ), M_searchInput->Render() | ftxui::flex });
         if ( M_enableBulkActions )
         {
            auto bulkActions = ftxui::hbox({
                 ftxui::filler(), ftxui::text("Select: ") | ftxui::dim,
                 M_selectAllBtn->Render(), ftxui::text("  "), M_deselectAllBtn->Render()
             });

             return ftxui::vbox( { searchbar, bulkActions } );
         }
         else
             return searchbar;
    }

    //! Build the list elements to be displayed in the scrollable list.
    virtual std::vector<ftxui::Element> buildElements() override
    {
        std::vector<ftxui::Element> elements;
        for ( auto & item : M_filteredItems )
            elements.push_back( item->component->Render() );
        return elements;
    }

private:

    //! Initialize the bulk action buttons with custom styling and event handling.
    void initBulkActionButtons()
    {
        auto styleButton = []( ftxui::EntryState const& s, ftxui::Color const& color )
        {
            auto element = ftxui::text( s.label ) | ftxui::color( color );
            if ( s.focused )
                element |= ftxui::inverted;
            return element;
        };

        auto selectOpt = ftxui::ButtonOption::Ascii();
        selectOpt.transform = [&styleButton]( ftxui::EntryState const& s ) { return styleButton( s, ftxui::Color::GreenLight ); };
        M_selectAllBtn = ftxui::Button( "✓ All", [this](){ onBulkAction(true); }, selectOpt );

        auto deselectOpt = ftxui::ButtonOption::Ascii();
        deselectOpt.transform = [&styleButton]( ftxui::EntryState const& s ) { return styleButton( s, ftxui::Color::RedLight ); };
        M_deselectAllBtn = ftxui::Button("✗ None", [this](){ onBulkAction(false); }, deselectOpt );
    }

    //! Apply the search query to filter the list items and update the displayed list.
    void applyFilter()
    {
        M_listContainer->DetachAllChildren();
        M_filteredItems.clear();

        std::string query = detail::toLower( M_searchQuery );

        for ( auto & item : M_allItems )
        {
            if ( query.empty() || detail::toLower( item.searchTest ).find( query ) != std::string::npos )
            {
                M_filteredItems.push_back( &item );
                M_listContainer->Add( item.component );
            }
        }
    }

private:
    std::string M_searchQuery;
    ftxui::Component M_searchInput;
    ftxui::Component M_listContainer = ftxui::Container::Vertical( {} );

    ftxui::Component M_selectAllBtn;
    ftxui::Component M_deselectAllBtn;
    bool M_enableBulkActions = true;

    std::vector<Item> M_allItems;
    std::vector<Item*> M_filteredItems;
};



}

