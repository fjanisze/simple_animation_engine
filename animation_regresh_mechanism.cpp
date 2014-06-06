#include "animation.hpp"
#include <iostream>

namespace animation_engine
{
namespace refresh_mechanism
{
    float animation_engine_refresh::set_refresh_internal_clock(float p_internal_clock_rate)
    {
        if(p_internal_clock_rate<=0)
        {
            throw std::runtime_error("p_internal_clock_rate<=0!");
        }
        float old_clock_rate=m_internal_clock_rate;
        m_internal_clock_rate=p_internal_clock_rate;
    }

    void animation_engine_refresh::refresh_all()
    {
        for(int i=0;i<10;i++)
        {
            std::cout<<"Refreshing\n";
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
    }

    /*
     * The function create a new thread which every m_internal_clock_rate seconds trigger
     * the 'refresh' function in each the registered animated objects.
     */
    std::thread::id animation_engine_refresh::start_internal_refresh_clock()
    {
        if(m_internal_clock_rate<=0)
        {
            throw std::runtime_error("m_internal_clock_rate<=0!");
        }
        m_refresh_thread=std::move(std::thread(&animation_engine_refresh::refresh_all,this));
    }

    animation_engine_refresh::~animation_engine_refresh()
    {
        if(m_refresh_thread.joinable())
        {
            m_refresh_thread.join();
        }
    }

}//refresh_mechanism
}//animation_engine
