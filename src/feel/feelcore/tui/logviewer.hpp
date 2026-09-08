//!

#pragma once

#include <deque>

#include <spdlog/sinks/base_sink.h>

#include <feel/feelcore/feelcore.hpp>
#include <feel/feelcore/tui/decorators.hpp>

namespace Feel::Core::tui
{

//! A single log entry for the TUI log viewer.
struct TuiLogEntry
{
    std::string message;
    log::level::level_enum level;
    std::chrono::system_clock::time_point timestamp;
};

//! Storage for log entries, with a maximum number of entries to keep.
struct TuiLogStorage {
    std::mutex mutex;
    std::deque<TuiLogEntry> entries;
    std::size_t maxEntries = 100;
};

//! A custom spdlog sink that stores log entries in a TuiLogStorage for display in a TUI log viewer.
template<typename MutexType>
class TuiLogSink
    : public log::sinks::base_sink<MutexType>
{
public:
    explicit TuiLogSink( std::shared_ptr<TuiLogStorage> storage ) : M_storage( storage ) {}

protected:

    //! Called by spdlog when a log message is emitted. Stores the log message in the TuiLogStorage.
    void sink_it_( log::details::log_msg const& msg ) override;

    void flush_() override {}

private:
    std::shared_ptr<TuiLogStorage> M_storage;
};


//! A TUI component that displays log entries in a scrollable list, with optional filters for log levels.
class FEELPP_CORE_EXPORT TuiLogViewerComponent
    : public ScrollableList
{
public:
    //! @param maxEntries The maximum number of log entries to keep in the viewer.
    explicit TuiLogViewerComponent( std::size_t maxEntries = 100 );

    //! Clear all log entries from the viewer.
    void clear()
    {
        std::lock_guard<std::mutex> lock( M_storage->mutex );
        M_storage->entries.clear();
    }

private:
    //! Build the header element with log level filters.
    std::optional<ftxui::Element> buildHeader() override;

    //! Build the logs paragraph elements to be displayed in the scrollable list, applying log level filters.
    std::vector<ftxui::Element> buildElements() override;

private:
    std::shared_ptr<TuiLogStorage> M_storage;

    //Filters
    bool M_showDebug = true;
    ftxui::Component M_debugCheckbox = ftxui::Checkbox( "Debug", &M_showDebug );

    bool M_showInfo  = true;
    ftxui::Component M_infoCheckbox = ftxui::Checkbox( "Info", &M_showInfo );

    bool M_showWarn  = true;
    ftxui::Component M_warnCheckbox = ftxui::Checkbox( "Warn", &M_showWarn );

    bool M_showError = true;
    ftxui::Component M_errorCheckbox = ftxui::Checkbox( "Error", &M_showError );

    std::map<log::level::level_enum, ftxui::Color> M_levelColorMap = {
        { log::level::debug, ftxui::Color::GrayDark },
        { log::level::trace, ftxui::Color::GrayLight },
        { log::level::info, ftxui::Color::GreenLight },
        { log::level::warn, ftxui::Color::Yellow },
        { log::level::critical, ftxui::Color::Red },
        { log::level::err, ftxui::Color::RedLight }
    };

};

}


