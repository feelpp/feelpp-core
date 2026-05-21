//!

#pragma once

#include <condition_variable>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <functional>

namespace Feel::Core
{

//Inspired from https://dev.to/ish4n10/making-a-thread-pool-in-c-from-scratch-bnm
class ThreadPool
{
public:
    ThreadPool( std::size_t nWorkers = std::thread::hardware_concurrency() );
    ~ThreadPool();
    ThreadPool( ThreadPool const& ) = delete; 
    ThreadPool( ThreadPool && ) = delete; 
    ThreadPool & operator=( ThreadPool const& ) = delete; 
    ThreadPool & operator=( ThreadPool && ) = delete; 

    template<typename F, typename... Args>
    std::future<F( Args... )> enqueue( F && f, Args &&... args )
    {
        auto func = std::bind( std::forward<F>(f), std::forward<Args>(args)... );
        auto encapsulatedPtr = std::make_shared<std::packaged_task<F(Args...)>>( func );

        std::future<F( Args... )> futureObj = encapsulatedPtr->get_future();
        {
            std::unique_lock<std::mutex> lock( M_mutex );
            M_tasks.emplace( [encapsulatedPtr] { *(encapsulatedPtr)(); } );
        }
        M_condition.notify_one();
        return futureObj;
    }

private:
    void worker();

private:
    std::vector<std::thread> M_workers;
    std::mutex M_mutex;
    std::queue<std::function<void()>> M_tasks;
    std::condition_variable_any M_condition;
    bool M_stop;

};


} // namespace Feel::Core
