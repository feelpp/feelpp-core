//!


#include "feel/feelcore/tui/decorators.hpp"
#include <feel/feelcore/tui/logviewer.hpp>


namespace Feel::Core::tui
{


template<typename MutexType>
void
TuiLogSink<MutexType>::sink_it_( log::details::log_msg const& msg )
{
    log::memory_buf_t formatted;
    this->formatter_->format( msg, formatted );
    std::string logMsg = fmt::to_string( formatted );

    // Strip trailing newline if present
    if ( !logMsg.empty() && logMsg.back() == '\n' )
        logMsg.pop_back();

    std::lock_guard<std::mutex> lock( M_storage->mutex );
    M_storage->entries.push_back( TuiLogEntry{ logMsg, msg.level, std::chrono::system_clock::now() } );

    // Limit the queue
    if ( M_storage->entries.size() > M_storage->maxEntries )
        M_storage->entries.pop_front();
}


TuiLogViewerComponent::TuiLogViewerComponent( std::size_t maxEntries )
    : ScrollableList( " Logs ", true )
{
    M_storage = std::make_shared<TuiLogStorage>();
    M_storage->maxEntries = maxEntries;
    auto sink = std::make_shared<TuiLogSink<std::mutex>>( M_storage );
    log::set_default_logger( std::make_shared<log::logger>( "TuiLogViewer", sink ) );

    ScrollableList::Add(
        ftxui::Container::Horizontal( { M_debugCheckbox, M_infoCheckbox , M_warnCheckbox, M_errorCheckbox } )
    );
}




std::optional<ftxui::Element>
TuiLogViewerComponent::buildHeader()
{
    return ftxui::hbox({
        ftxui::text("Filters:  ") | ftxui::dim,
        M_debugCheckbox->Render() | ftxui::color( ftxui::Color::GrayDark ),
        ftxui::text("   "),
        M_infoCheckbox->Render() | ftxui::color( ftxui::Color::GreenLight ),
        ftxui::text("   "),
        M_warnCheckbox->Render() | ftxui::color( ftxui::Color::Yellow ),
        ftxui::text("   "),
        M_errorCheckbox->Render() | ftxui::color( ftxui::Color::RedLight )
    });
}


std::vector<ftxui::Element>
TuiLogViewerComponent::buildElements()
{
    std::lock_guard<std::mutex> lock( M_storage->mutex );
    std::vector<ftxui::Element> logElements;
    for ( const auto& entry : M_storage->entries )
    {
        if ( entry.level == log::level::trace && !M_showDebug ) continue;
        if ( entry.level == log::level::debug && !M_showDebug ) continue;
        if ( entry.level == log::level::info  && !M_showInfo )  continue;
        if ( entry.level == log::level::warn  && !M_showWarn )  continue;
        if ( (entry.level == log::level::err || entry.level == log::level::critical) && !M_showError ) continue;


        ftxui::Color color = ftxui::Color::White;
        if ( M_levelColorMap.contains( entry.level ) )
            color = M_levelColorMap.at( entry.level );

        logElements.push_back( ftxui::paragraph( entry.message ) | ftxui::color( color ) );
    }
    return logElements;
}



}


