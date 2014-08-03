#include <iostream>
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <cmath>
#include <vector>
#include <algorithm>

#ifndef ANIM_REFRESH_MECHANISM_HPP
#define ANIM_REFRESH_MECHANISM_HPP

namespace animation_engine
{
namespace refresh_mechanism
{
    //Interface for the refresh mechanism
    template<typename F_T>
    class I_animation_engine_refresh
    {
    public:
        using function_type=std::function<F_T>;
    public:
        virtual int set_refresh_internal_clock_rate(int p_internal_clock_rate) = 0;
        virtual int get_refresh_internal_clock_rate() = 0;

        virtual bool start_internal_refresh_cycle() = 0;
        virtual bool stop_internal_refresh_cycle() = 0;
        virtual bool pause_internal_refresh_cycle() = 0;
        virtual void unpause_internal_refresh_cycle() = 0;

        virtual ~I_animation_engine_refresh() = 0;
        virtual int register_function(const function_type& p_function) = 0;
        virtual int unregister_function(const function_type& p_function) = 0;
    };

    template<typename T_F>
    I_animation_engine_refresh<T_F>::~I_animation_engine_refresh(){}

    //Implementation of the refresh mechanism
    template<typename F_T>
    class animation_engine_refresh : public I_animation_engine_refresh<F_T>
    {
    public:
        using function_type=std::function<F_T>;
    private:
        int m_internal_clock_rate{1000};
        std::thread m_refresh_thread;
        void refresh_all();
        std::mutex m_observers_mtx;
        std::vector<function_type> m_observers;
        bool m_continue_the_cycle,
             m_cycle_pause;
    public:
        animation_engine_refresh();
        int set_refresh_internal_clock_rate(int p_internal_clock_rate);
        int get_refresh_internal_clock_rate();

        bool start_internal_refresh_cycle();
        bool stop_internal_refresh_cycle();
        bool pause_internal_refresh_cycle();
        void unpause_internal_refresh_cycle();

        ~animation_engine_refresh();
        int register_function(const function_type& p_function);
        int unregister_function(const function_type& p_function);
    };
}//refresh_mechanism

namespace refresh_mechanism //Implementation
{
    template<typename F_T>
    animation_engine_refresh<F_T>::animation_engine_refresh():
        m_continue_the_cycle{false},
        m_cycle_pause{false}
    {
    }

    template<typename F_T>
    int animation_engine_refresh<F_T>::set_refresh_internal_clock_rate(int p_internal_clock_rate)
    {
        if(p_internal_clock_rate<=0)
        {
            throw std::runtime_error("p_internal_clock_rate<=0!");
        }
        int old_clock_rate=m_internal_clock_rate;
        m_internal_clock_rate=p_internal_clock_rate;
        return old_clock_rate;
    }


    template<typename F_T> inline
    int animation_engine_refresh<F_T>::get_refresh_internal_clock_rate()
    {
        return m_internal_clock_rate;
    }

    template<typename F_T>
    void animation_engine_refresh<F_T>::refresh_all()
    try{
        while(m_continue_the_cycle)
        {
            if(!m_cycle_pause&&m_observers_mtx.try_lock())
            {
                for(auto& elem:m_observers)
                {
                    elem();
                }
                m_observers_mtx.unlock();
            }
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
        }
    }catch(std::exception& xa)
    {
        m_continue_the_cycle=false;
    }

    /*
     * The function create a new thread which every m_internal_clock_rate seconds trigger
     * the 'refresh' function in each the registered animated objects.
     */
    template<typename F_T>
    bool animation_engine_refresh<F_T>::start_internal_refresh_cycle()
    {
        if(m_internal_clock_rate<=0)
        {
            return false;
        }
        //Set the running flag, if the flag is already set we had encountered an error
        if(m_continue_the_cycle)
        {
            //This is wrong, someone has already enabled the refresh loop
            return false;
        }
        m_continue_the_cycle=true;
        atomic_thread_fence(std::memory_order_release);
        m_refresh_thread=std::move(std::thread(&animation_engine_refresh::refresh_all,this));
        return true;
    }

    template<typename F_T>
    bool animation_engine_refresh<F_T>::stop_internal_refresh_cycle()
    {
        if(!m_continue_the_cycle)
        {
            return false;
        }
        m_continue_the_cycle=false;
        atomic_thread_fence(std::memory_order_release);
        if(m_refresh_thread.joinable())
        {
            m_refresh_thread.join();
        }
        m_cycle_pause=false;
        return true;
    }

    template<typename F_T>
    bool animation_engine_refresh<F_T>::pause_internal_refresh_cycle()
    {
        if(m_continue_the_cycle)
        {
            m_cycle_pause=true;
        }
        return m_cycle_pause;
    }

    template<typename F_T>
    void animation_engine_refresh<F_T>::unpause_internal_refresh_cycle()
    {
        m_cycle_pause=false;
    }

    template<typename F_T>
    animation_engine_refresh<F_T>::~animation_engine_refresh()
    {
        if(m_refresh_thread.joinable())
        {
            m_refresh_thread.join();
        }
    }

    template<typename F_T>
    int animation_engine_refresh<F_T>::register_function(const function_type& p_function)
    {
        std::lock_guard<std::mutex> lock{m_observers_mtx};
        m_observers.push_back(p_function);
        return m_observers.size();
    }

    template<typename F_T>
    int animation_engine_refresh<F_T>::unregister_function(const function_type& p_function)
    {
        std::lock_guard<std::mutex> lock{m_observers_mtx};
        m_observers.erase(std::remove_if(m_observers.begin(),m_observers.end(),
                          [&p_function](const function_type& p_function2)
                          {
                            return p_function.template target<function_type>()==p_function2.template target<function_type>();
                          }));
        return m_observers.size();
    }

}//refresh_mechanism
}//animation_engine

#endif//ANIM_REFRESH_MECHANISM_HPP
