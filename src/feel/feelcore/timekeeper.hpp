
//!

#pragma once

#include <chrono>
#include <stack>
#include <shared_mutex>

#include <feel/feelcore/environment.hpp>

namespace Feel::Core
{

//! Singleton class to keep track of elapsed times in different code sections
class FEELPP_CORE_EXPORT Timekeeper
{
public:

    ~Timekeeper();

    //! Get the singleton instance
    static Timekeeper* instance();

    //! Returns true if timekeeping is enabled
    bool isEnabled() const noexcept { return M_enabled; };

    //! Enable or disable timekeeping
    void setEnabled( bool enabled ) { M_enabled = enabled; };

    //! Get a snapshot of the recorded times as a JSON object
    nl::json const&
    getTimesSnapshot() const noexcept
    {
        std::shared_lock<std::shared_mutex> lock( M_mutex );
        return M_times;
    };

    //! Save the recorded times to a JSON file
    void save( fs::path const& filename ) const;


    //! Clear the recorder times
    void clear()
    {
        std::unique_lock<std::shared_mutex> lock( M_mutex );
        M_times.clear();
    };

    //! Get the thread local context
    std::vector<std::string> const& getContext() const noexcept;

    //! Get the thread local context
    void setContext( std::vector<std::string> const& ctx );

private:
    Timekeeper();
    Timekeeper( Timekeeper const& ) = delete;
    Timekeeper& operator=( Timekeeper const& ) = delete;

    //! Record elapsed time in seconds for the current context
    void recordTime( double time );

    //! Push a new context level
    void pushContext( std::string const& name );

    //! Pop the last context level
    void popContext();

private:
    class Impl;
    Impl* M_pimpl;

    static std::unique_ptr<Timekeeper> S_instance;
    nl::json M_times = nl::json::object();
    mutable std::shared_mutex M_mutex;
    static std::once_flag S_onceFlag;

    static inline std::atomic<bool> M_enabled{ false };

    friend class Timer;
};

//! Timer utility class to measure elapsed time of code hierarchical sections
class FEELPP_CORE_EXPORT Timer
{
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
public:

    Timer() = default;

    //! RAII timer, starts timing upon construction and stops upon destruction ( or until toc )
    template<typename... Ts>
    Timer( Ts && ... v )
    {
        this->tic( std::forward<Ts>( v )... );
    };

    //! Tocs if not already stopped
    ~Timer()
    {
        while ( !M_timeStack.empty() )
            this->toc();
    };

    //! Start timing a new section
    void tic(std::string const& name = "", std::string const& msg = "", int verboseLvl = 1);

    //! Stop timing the last started section and return elapsed time in seconds
    double toc();

private:
    std::stack<std::tuple<TimePoint, std::string, int>> M_timeStack; // start time, message, verbose level
};



}
