//!

#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <functional>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>



namespace Feel::Core::ftxui
{

//! Current execution lifecycle of the async task.
enum class TaskStatus { IDLE, WORKING, SUCCESS, ERROR };

//! Thread safe state of the async task
struct AsyncUiTaskState
{
    std::string result;
    std::atomic<TaskStatus> status = { TaskStatus::IDLE };
    std::atomic<std::size_t> loadingFrameCount = { 0 };
};

//! Manages a background task while keeping an FTXUI screen responsive 
//! Updates the screen each tick (allows load animations)
class AsyncUiTask
{
public:

    //! task: Background task to execute, should return either a success message if OK and can raise an exception on error
    //! screen: An FTXUI screen to post UI refresh events to
    //! tickDuration: The interval between forced UI redraws while the task is running
    AsyncUiTask( std::function<std::string()> task,
                 ::ftxui::ScreenInteractive & screen,
                 std::chrono::milliseconds tickDuration = std::chrono::milliseconds( 200 ) )
        : M_task( std::move( task ) ), M_screen( screen ), M_tickDuration( tickDuration )
    {}

    //! Spawns a worker and tick threads.
    void start();

    //! Clears the task state and goes back to IDLE .
    void reset();

    //! Retrieves the current task state.
    AsyncUiTaskState const& getState() const { return M_state; }

    //! Returns a colored UI Element displaying the current task state
    ::ftxui::Element getStateUiElement();

private:
    //! Executes a task and posts the result to the UI
    void executeTask();

    //! Sends a UI event each tick milliseconds to redraw
    void pumpTicks();

private:
    std::function<std::string()> M_task;
    ::ftxui::ScreenInteractive & M_screen;
    AsyncUiTaskState M_state;
    std::chrono::milliseconds M_tickDuration;
};


}
