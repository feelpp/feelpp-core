//!

#include <ftxui/component/event.hpp>

#include <feel/feelcore/tui/timekeeperviewer.hpp>


namespace Feel::Core::tui
{


std::vector<ftxui::Element>
TuiTimekeeperViewerComponent::buildElements()
{
    nl::json timesSnapshot = Timekeeper::instance()->getTimesSnapshot();

    std::vector<ftxui::Element> lines;
    M_nodeBoxes.clear();

    auto now = std::chrono::system_clock::now().time_since_epoch();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    std::function<void( std::string const &, nl::json const&, int, std::string const&)> renderNode;
    renderNode = [&]( std::string const& name, nl::json const& node, int currentDepth, std::string const& currentPath)
    {
        if ( currentDepth > M_maxDepth ) return;

        bool hasChildren = node.contains( "subsections" ) && !node["subsections"].empty();
        bool isCollapsed = M_collapsedNodes.contains( currentPath );

        std::string timeStr = "0.00s";
        std::string detailStr = "";
        ftxui::Color timeColor;
        bool isMultiThreaded = false;
        if ( node.contains( "times" ) )
        {
            auto const& times = node[ "times" ];
            isMultiThreaded = times.contains( "max" ) && times.contains( "elapsed_sum" ) && times.value( "count", 1 ) > 1;
            if ( isMultiThreaded )
            {
                double sum_time = times[ "elapsed_sum" ].get<double>();
                double max_time = times[ "max" ].get<double>();
                int count = times.value( "count", 1 );

                timeStr = fmt::format( "{:.2f}s", max_time );
                detailStr = fmt::format( "sum: {:.2f}s ({}x)", sum_time, count );
            }
            else if ( times.contains( "elapsed" ) )
                timeStr = fmt::format( "{:.2f}s", times["elapsed"].get<double>() );
            else
                timeStr = "0.00s";

            timeColor = ftxui::Color::Cyan;
        }
        else
        {
            std::vector<std::string> spinner = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
            timeStr = spinner[(ms / 100) % spinner.size()];
            timeColor = ftxui::Color::Yellow;
        }

        std::string indent( currentDepth * 2, ' ' );
        std::string prefix = ( currentDepth == 0 ) ? "" : "├─ ";

        std::string collapseIcon = hasChildren ? ( isCollapsed ? "▶ " : "▼ " ) : "  ";

        ftxui::Color branchColor = isMultiThreaded ? ftxui::Color::Magenta : ftxui::Color::GrayLight;

        ftxui::Elements block;

        block.push_back(
            ftxui::hbox({
                ftxui::text( indent + prefix + collapseIcon ) |  ftxui::color( branchColor ),
                ftxui::text( name ),
                ftxui::filler(),
                ftxui::text( timeStr ) | ftxui::bold | ftxui::color( timeColor )
            })
        );

        if ( !detailStr.empty() )
        {
            std::string detailIndent( ( currentDepth * 2 ) + 2 + (hasChildren ? 2 : 0), ' ' );
            block.push_back(
                ftxui::hbox({
                    ftxui::text( detailIndent + "└ " ) | ftxui::color( branchColor ),
                    ftxui::text( detailStr ) | ftxui::dim | ftxui::color( ftxui::Color::GrayLight ),
                    ftxui::filler()
                })
            );
        }

        M_nodeBoxes.push_back( { currentPath, ftxui::Box{}, hasChildren } );
        lines.push_back( ftxui::vbox( block ) | ftxui::reflect( M_nodeBoxes.back().box ) );

        if ( currentDepth < M_maxDepth && hasChildren && !isCollapsed )
            for ( auto it = node[ "subsections" ].begin(); it != node[ "subsections" ].end(); ++it )
                renderNode( it.key(), it.value(), currentDepth + 1, currentPath + "." + it.key());
    };

    for ( auto it = timesSnapshot.begin(); it != timesSnapshot.end(); ++it )
        renderNode( it.key(), it.value(), 0, it.key() );

    return lines;
}



std::optional<ftxui::Element>
TuiTimekeeperViewerComponent::buildFooter()
{
    return ftxui::hbox({
        ftxui::text( "Legend: " ) | ftxui::dim,
        ftxui::filler(),
        ftxui::text( "■ " ) | ftxui::color( ftxui::Color::Magenta ),
        ftxui::text( "Multi " ),
        ftxui::filler(),
        ftxui::text( "⠋ " ) | ftxui::color( ftxui::Color::Yellow ),
        ftxui::text( "Running " ),
        ftxui::filler(),
        ftxui::text( "Done" ) | ftxui::color( ftxui::Color::Cyan ) | ftxui::bold
    }) | ftxui::center;
}

bool
TuiTimekeeperViewerComponent::OnEvent( ftxui::Event event )
{
    if ( event.is_mouse() && event.mouse().button == ftxui::Mouse::Left && event.mouse().motion == ftxui::Mouse::Pressed )
    {
        for ( auto & node : M_nodeBoxes )
        {
            if ( node.hasChildren && node.box.Contain( event.mouse().x, event.mouse().y ) )
            {
                if ( M_collapsedNodes.contains( node.path ) )
                    M_collapsedNodes.erase( node.path );
                else
                    M_collapsedNodes.insert( node.path );

                return true;
            }
        }
    }

    return ScrollableList::OnEvent( event );
}


}
