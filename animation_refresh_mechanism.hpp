#include <iostream>
#include <functional>
#include <atomic>

namespace animation_engine
{
namespace refresh_mechanism
{
    template<typename F_T>
    class animation_engine_refresh
    {
    public:
        using function_type=std::function<F_T>;
    private:
        int m_internal_clock_rate{1000};
        std::thread m_refresh_thread;
        void refresh_all();
        std::vector<function_type> m_observers;
        bool m_continue_the_cycle;
    public:
        animation_engine_refresh();
        int set_refresh_internal_clock(int p_internal_clock_rate);
        bool start_internal_refresh_cycle();
        bool stop_internal_refresh_cycle();
        ~animation_engine_refresh();
        int register_function(const function_type p_function);
    };
}//refresh_mechanism

namespace refresh_mechanism //Implementation
{
    template<typename F_T>
    animation_engine_refresh<F_T>::animation_engine_refresh():
        m_continue_the_cycle(false)
    {
    }

    template<typename F_T>
    int animation_engine_refresh<F_T>::set_refresh_internal_clock(int p_internal_clock_rate)
    {
        if(p_internal_clock_rate<=0)
        {
            throw std::runtime_error("p_internal_clock_rate<=0!");
        }
        int old_clock_rate=m_internal_clock_rate;
        m_internal_clock_rate=p_internal_clock_rate;
        return old_clock_rate;
    }

    template<typename F_T>
    void animation_engine_refresh<F_T>::refresh_all()
    {
        while(m_continue_the_cycle)
        {
            for(auto& elem:m_observers)
            {
                elem();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{m_internal_clock_rate});
        }
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
        return true;
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
    int animation_engine_refresh<F_T>::register_function(const function_type p_function)
    {
        m_observers.push_back(p_function);
        return m_observers.size();
    }

}//refresh_mechanism
}//animation_engine
