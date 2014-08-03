#include "animation.hpp"
#include <iostream>

namespace animation_engine
{

    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animation_functions
    //
    //
    ///////////////////////////////////////////////////////////////////

    int linear_interpolation::calculate_interpolation(std::vector<sf::Vector2f>& object_positions,
                                                     const sf::Vector2f& p_begin,
                                                     const sf::Vector2f& p_end)
    {
        auto get_y=[&](int x)
        {
            return m*(x-x_end)+y_end;
        };
        auto get_x=[&](int y)
        {
            return m!=0?((y-y_end)/m)+x_end:x_end;
        };
        object_positions.clear();
        x_beg=p_begin.x;
        y_beg=p_begin.y;
        x_end=p_end.x;
        y_end=p_end.y;
        //y=mx+q
        if((x_end-x_beg)==0)
        {
            m=0;
        }
        else
        {
            m=(y_end-y_beg)/(x_end-x_beg);
        }
        using std::abs;
        if(abs(x_end-x_beg)>abs(y_end-y_beg))
        {

            interpolation_impl(object_positions,
                              get_y,
                              [&object_positions](int a,int b){object_positions.push_back(sf::Vector2f(a,b));},
                              x_beg,x_end);
        }
        else
        {
            interpolation_impl(object_positions,
                               get_x,
                               [&object_positions](int a,int b){object_positions.push_back(sf::Vector2f(b,a));},
                               y_beg,y_end);
        }

        return object_positions.size();
    }

    void linear_interpolation::interpolation_impl(std::vector<sf::Vector2f>& elems,
                                                std::function<int(int)> func,
                                                std::function<void(int,int)> push,
                                                int from,int to)
    {
        int num_of_elements=std::abs(to-from);
        while(num_of_elements>0)
        {
            int cur=func(from);
            push(from,cur);
            if(from>to)
                --from;
            else
                ++from;
            --num_of_elements;
        }
        elems.push_back(sf::Vector2f(x_end,y_end));
    }


    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animated_object
    //
    //
    ///////////////////////////////////////////////////////////////////

    animated_object::animated_object()
    {
        m_status=anim_obj_status::STATUS_NOT_READY;
    }

    void animated_object::repeat()
    {
        if(m_status==anim_obj_status::STATUS_COMPLETED)
        {
            m_current_position=0;
            m_status=anim_obj_status::STATUS_READY;
        }
    }

    anim_obj_status animated_object::stop()
    {
        if(m_status!=anim_obj_status::STATUS_NOT_READY&&
           m_status!=anim_obj_status::STATUS_FAULTY)
        {
            m_status=anim_obj_status::STATUS_STOPPED;
        }
        return m_status;
    }

    anim_obj_status animated_object::refresh()
    {
        if(m_status==anim_obj_status::STATUS_NOT_READY||
           m_status==anim_obj_status::STATUS_FAULTY)
        {
            return m_status;
        }
        if(m_status==anim_obj_status::STATUS_READY)
        {
            if(m_begin_position==m_end_position)
            {
                m_status=anim_obj_status::STATUS_COMPLETED;
            }
            else
            {
                moving_object_draw_impl();
            }
        }
        else if(m_status==anim_obj_status::STATUS_STOPPED)
        {
            //Just use the current position and change nothing
        }
        return m_status;
    }

    anim_obj_status animated_object::draw(sf::RenderWindow& p_rnd)
    {
#ifndef RUN_REGRESSION //Not very elegant, but works..
		if (m_status != anim_obj_status::STATUS_NOT_READY&&
			m_status != anim_obj_status::STATUS_FAULTY)
        {
            draw_impl(p_rnd);
        }
#endif
        return m_status;
    }

    sf::Vector2f animated_object::moving_object_draw_impl()
    {
        m_current_time+=m_single_time_increment;
        if(m_current_time>=m_new_position_threshold)
        {
			double l_diff = m_single_time_increment / m_new_position_threshold;
			do{
				m_cur_position = get_current_position();
			} while (--l_diff > 0);
			m_current_time = 0;
        }
        return m_cur_position;
    }

    sf::Vector2f animated_object::get_current_position()
    try{
        if(m_current_position>=object_positions.size())
        {
            if(m_status==anim_obj_status::STATUS_READY)
            {
                m_status=anim_obj_status::STATUS_COMPLETED;
            }
            return m_end_position;
        }
        //Look for the new position
        sf::Vector2f position;
        position=object_positions[m_current_position];
        ++m_current_position;
        return position;
    }catch(...)
    {
        m_current_position=0;
        m_status=anim_obj_status::STATUS_FAULTY;
        throw;
    }

    bool animated_object::set_animation_duration(float p_anim_duration)
    {
        if(p_anim_duration<=0)
            return false;
        m_animation_duration=p_anim_duration;
        return true;
    }

    float animated_object::get_animation_execution_time(int p_refresh_speed)
    {
        return (float)object_positions.size()/p_refresh_speed;
    }

    bool animated_object::set_refresh_frequency(int p_frequency)
    {
        if(m_animation_duration==0||p_frequency<=0)
        {
            return false;
        }
        m_refresh_frequency=p_frequency;
        calculate_drawing_data();
        return true;
    }

    void animated_object::calculate_drawing_data()
    {
        m_current_time=0;
        m_single_time_increment=(double)1/m_refresh_frequency;
        m_new_position_threshold=(double)(m_animation_duration)/object_positions.size();
    }

    animated_object::~animated_object(){}
}



































