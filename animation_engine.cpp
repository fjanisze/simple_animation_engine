#include "animation.hpp"
#include <cmath>

namespace animation_engine
{
    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animation_engine
    //
    //
    ///////////////////////////////////////////////////////////////////

    animation_engine::animation_engine(sf::RenderWindow& p_rnd_wnd,int p_frame_rate,
                                       refresh_sys_ptr p_refresh_mechanism):
        m_frame_rate(p_frame_rate),m_rnd_wnd(p_rnd_wnd),
        m_refresh(std::move(p_refresh_mechanism))
    {
        if(p_frame_rate<=0)
        {
            throw std::runtime_error("p_frame_rate<=0");
        }
        m_refresh->set_refresh_internal_clock_rate(1000);
        if(!m_refresh->start_internal_refresh_cycle())
        {
            throw std::runtime_error("Unable to start the internal refresh cycle");
        }
    }

    animation_engine::~animation_engine()
    {
        m_refresh->stop_internal_refresh_cycle();
    }

    int animation_engine::register_object(anim_obj_ptr p_obj,animated_obj_completion_opt p_action_when_completed)
    {
        anim_obj_container_entry new_entry;
        new_entry.m_anim_object=p_obj;
        new_entry.m_action_when_completed=p_action_when_completed;
        if(!new_entry.m_anim_object->set_refresh_frequency(m_refresh->get_refresh_internal_clock_rate()))
        {
            //Most probably the duration of the animation was not provided
            return -1;
        }
        m_object_container.push_back(new_entry);
        m_refresh->register_function(std::bind(&animated_object::refresh,new_entry.m_anim_object));
        return m_object_container.size();
    }

    draw_return_status animation_engine::draw()
    {
        draw_return_status status=draw_return_status::STATUS_OK;
        amount_of_obj_in_complete_state=0;
        amount_of_obj_in_stop_state=0;
        anim_obj_status obj_status;
        for(auto& elem:m_object_container)
        {
            if(elem.m_to_be_removed)
            {
                ++amount_of_obj_in_complete_state;
                status=draw_return_status::STATUS_CLEANUP_NEEDED;
                continue; //Skip the objects which should be removed
            }
            obj_status=elem.m_anim_object->draw(m_rnd_wnd);
            if(obj_status==anim_obj_status::STATUS_COMPLETED)
            {
                status=perf_action_on_completed_animation(elem);
                if(elem.m_action_when_completed==animated_obj_completion_opt::ACTION_REMOVE_ANIMATED_OBJECT)
                {
                    ++amount_of_obj_in_complete_state;
                }
            }
            else if(obj_status==anim_obj_status::STATUS_STOPPED)
            {
                ++amount_of_obj_in_stop_state;
            }
        }
        return status;
    }

    //The cleanup action is needed to remove from the container the object marked as m_to_be_removed{true}
    int animation_engine::clean_up(bool p_force_clean_all)
    {
        if(p_force_clean_all)
        {
            m_object_container.clear();
            return 0;
        }
        auto it=m_object_container.begin();
        while(it!=m_object_container.end())
        {
            if(it->m_to_be_removed)
            {
                m_object_container.erase(it++);
            }
            else ++it;
        }
        return m_object_container.size();
    }

    draw_return_status animation_engine::perf_action_on_completed_animation(anim_obj_container_entry& p_obj)
    {
        switch(p_obj.m_action_when_completed)
        {
        case animated_obj_completion_opt::ACTION_DEFAULT: //Default action: remove the object
        case animated_obj_completion_opt::ACTION_REMOVE_ANIMATED_OBJECT:
            p_obj.m_to_be_removed=true;
            return draw_return_status::STATUS_CLEANUP_NEEDED;
            break;
        case animated_obj_completion_opt::ACTION_REPEAT_ANIMATION:
            {
                p_obj.m_anim_object->repeat();
            }
            break;
        case animated_obj_completion_opt::ACTION_DONT_MOVE:
            {
                p_obj.m_anim_object->stop();
                ++amount_of_obj_in_stop_state;
            }
            break;
        }
        return draw_return_status::STATUS_OK;
    }

    //Return true if all the animated_texture are in the STATUS_COMPLETE state
    bool animation_engine::check_if_all_completed()
    {
        return amount_of_obj_in_complete_state==m_object_container.size();
    }

    bool animation_engine::check_if_all_completed_or_stopped()
    {
        return (amount_of_obj_in_complete_state+amount_of_obj_in_stop_state)==m_object_container.size();
    }
}//animation_engine




























