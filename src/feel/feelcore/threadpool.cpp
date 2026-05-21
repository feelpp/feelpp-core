//!

#include <feel/feelcore/threadpool.hpp>

namespace Feel::Core
{

ThreadPool::ThreadPool( std::size_t nWorkers )
{
    M_stop = false;
    M_workers.reserve( nWorkers );
    for ( std::size_t i = 0; i < nWorkers; ++i )
        M_workers.emplace_back( &ThreadPool::worker, this );
};

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock( M_mutex );
        M_stop = true;
    }
    M_condition.notify_all();
    for (  std::thread & worker : M_workers )
        worker.join();
}

void
ThreadPool::worker()
{
    while( true )
    {
        std::function<void()> currentTask;
        {
            std::unique_lock<std::mutex> lock( M_mutex );
            M_condition.wait( lock, [this]() { return M_stop || !M_tasks.empty(); } );

            if ( M_stop && M_tasks.empty() )
                break;
            if ( M_tasks.empty() )
                continue;

            currentTask = M_tasks.front();
            M_tasks.pop();
        }
        currentTask();
    }
}


} // namespace Feel::Core
