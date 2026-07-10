
//!

#pragma once

#include <list>
#include <set>

#include <feel/feelcore/timekeeper.hpp>
#include <feel/feelcore/tui/decorators.hpp>

namespace Feel::Core::tui
{

//! A TUI component that displays the recorded times from the Timekeeper in a scrollable and collapsible list.
class TuiTimekeeperViewerComponent
    : public ScrollableList
{
public:

    //! @param maxDepth The maximum depth of the timekeeping hierarchy to display.
    explicit TuiTimekeeperViewerComponent( int maxDepth = 3 )
        : ScrollableList( " Timekeeper " ), M_maxDepth( maxDepth )
    {}


    //! Clear all recorded times from the Timekeeper.
    void clear() { Timekeeper::instance()->clear(); }

    //! Builds the timer hierarchy elements to be displayed in the scrollable list.
    std::vector<ftxui::Element> buildElements() override;

    //! Builds the legend footer element to be displayed below the list.
    std::optional<ftxui::Element> buildFooter() override;

    //! Handles mouse events for collapsing and expanding nodes in the timer hierarchy.
    bool OnEvent( ftxui::Event event ) override;

private:
    int M_maxDepth;

    struct NodeBox
    {
        std::string path;
        ftxui::Box box;
        bool hasChildren;
    };

    std::list<NodeBox> M_nodeBoxes;
    std::set<std::string> M_collapsedNodes;
};

}
