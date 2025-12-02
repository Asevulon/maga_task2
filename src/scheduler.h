#pragma once

#include <functional>
#include <thread>
#include <list>
#include <cstdint>
#include <queue>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <unordered_set>

class Scheduler
{
    using thread = std::thread;
    using TaskFunc = std::function<void()>;
    using TaskPair = std::pair<uint64_t, TaskFunc>;

    // task_handlers
    std::list<thread> thread_list;
    std::queue<TaskPair> task_queue;
    std::unordered_set<std::thread::id> finished_thread_set;

    // params
    uint64_t max_thread_num = 4;
    // counters
    uint64_t active_thread_counter = 0;
    uint64_t task_counter = 0;
    uint64_t done_task_counter = 0;

    // thread helpers
    std::condition_variable waker_cv;
    std::mutex waker_m;
    std::mutex finisher_m;

    bool stop_is_requested = false;
    bool scheduler_is_active = false;

protected:
    void start_scheduler()
    {
        if (scheduler_is_active)
            return;

        thread thr(
            [&]()
            { schedule(); });
        thr.detach();
        scheduler_is_active = true;
        std::cerr << "Scheduler: scheduler started" << std::endl;
    }

    bool safe_thread_checker(std::thread &trg)
    {
        std::lock_guard<std::mutex> lk(finisher_m);
        if (finished_thread_set.contains(trg.get_id()))
        {
            finished_thread_set.erase(trg.get_id());
            trg.join();
            return true;
        }
        return false;
    }

    void schedule()
    {
        while (1)
        {

            std::unique_lock<std::mutex> lk(waker_m);
            waker_cv.wait(lk);

            if (stop_is_requested)
                break;

            thread_list.remove_if([&](std::thread &thr)
                                  { return safe_thread_checker(thr); });
            while ((active_thread_counter < max_thread_num) &&
                   (!task_queue.empty()))
            {

                TaskPair task = task_queue.front();
                task_queue.pop();

                thread_list.emplace_back(
                    [&, task]()
                    {
                        task.second();
                        --active_thread_counter;
                        ++done_task_counter;
                        std::cerr << "Scheduler: task " << task.first << " is done" << std::endl;

                        std::lock_guard<std::mutex> lk(finisher_m);
                        finished_thread_set.emplace(std::this_thread::get_id());
                        waker_cv.notify_all();
                    });
                ++active_thread_counter;

                std::cerr << "Scheduler: running task " << task.first << ", active threads: " << active_thread_counter << std::endl;
            }
        }
        scheduler_is_active = false;
        std::cerr << "Scheduler: scheduler was stopped" << std::endl;
    }

public:
    Scheduler() {}
    ~Scheduler() { stop(); }

    void start(const std::function<void()> &func)
    {
        start_scheduler();
        std::cerr << "Scheduler: added task " << task_counter << std::endl;
        task_queue.emplace(task_counter, func);
        ++task_counter;
        waker_cv.notify_all();
    }

    void set_max_thread_num(uint64_t val) { max_thread_num = val; }
    void stop()
    {
        stop_is_requested = true;
        waker_cv.notify_all();
    }

    auto get_task_counter() { return task_counter; }
    auto get_done_task_counter() { return done_task_counter; }
};