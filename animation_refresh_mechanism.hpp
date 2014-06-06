#include <iostream>
#include <functional>

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
        float m_internal_clock_rate{1};
        std::thread m_refresh_thread;
        void refresh_all();
        std::vector<function_type> observers;
    public:
        animation_engine_refresh();
        float set_refresh_internal_clock(float p_internal_clock_rate);
        std::thread::id start_internal_refresh_clock();
        ~animation_engine_refresh();
        int register_function(const function_type p_function);
    };
}//refresh_mechanism

namespace refresh_mechanism //Implementation
{
    template<typename F_T>
    animation_engine_refresh<F_T>::animation_engine_refresh()
    {
    }

    template<typename F_T>
    float animation_engine_refresh<F_T>::set_refresh_internal_clock(float p_internal_clock_rate)
    {
        if(p_internal_clock_rate<=0)
        {
            throw std::runtime_error("p_internal_clock_rate<=0!");
        }
        float old_clock_rate=m_internal_clock_rate;
        m_internal_clock_rate=p_internal_clock_rate;
    }

    template<typename F_T>
    void animation_engine_refresh<F_T>::refresh_all()
    {
        for(auto& elem:observers)
        {
            elem();
        }
    }

    /*
     * The function create a new thread which every m_internal_clock_rate seconds trigger
     * the 'refresh' function in each the registered animated objects.
     */
    template<typename F_T>
    std::thread::id animation_engine_refresh<F_T>::start_internal_refresh_clock()
    {
        if(m_internal_clock_rate<=0)
        {
            throw std::runtime_error("m_internal_clock_rate<=0!");
        }
        m_refresh_thread=std::move(std::thread(&animation_engine_refresh::refresh_all,this));
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
        observers.push_back(p_function);
        return observers.size();
    }

}//refresh_mechanism
}//animation_engine
