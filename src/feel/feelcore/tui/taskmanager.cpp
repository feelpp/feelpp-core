//!


#include <fmt/format.h>

#include <feel/feelcore/tui/taskmanager.hpp>



namespace Feel::Core::ftxui
{


void
AsyncUiTask::executeTask()
{
    std::string internalResult;
    TaskStatus internalStatus = TaskStatus::WORKING; 
    M_state.status = internalStatus;

    try
    {
        internalResult = M_task();
        internalStatus = TaskStatus::SUCCESS;
    }
    catch ( std::exception const& e )
    {
        internalResult = fmt::format( "Error: {}", e.what() );
        internalStatus = TaskStatus::ERROR; 
    }

    M_screen.Post( [this, internalResult, internalStatus]()
    {
        M_state.status = internalStatus;
        M_state.result = internalResult;
    });
    M_screen.PostEvent( ::ftxui::Event::Custom );
}

void
AsyncUiTask::pumpTicks()
{
    while ( M_state.status == TaskStatus::WORKING )
    {
        std::this_thread::sleep_for( M_tickDuration );
        ++M_state.loadingFrameCount;
        M_screen.PostEvent( ::ftxui::Event::Custom );
    }
}

void
AsyncUiTask::start()
{
    // prevents double trigger
    if ( M_state.status == TaskStatus::WORKING ) return;

    std::thread( [this](){ this->executeTask(); }).detach();
    std::thread( [this](){ this->pumpTicks(); }).detach();
}

void
AsyncUiTask::reset()
{
    M_state.status = TaskStatus::IDLE;
    M_state.result = "";
    M_state.loadingFrameCount = 0;
}

} // namespace Feel::Core::ftxui
