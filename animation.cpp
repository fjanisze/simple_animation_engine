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

    //Create an empty sprite pointer
    anim_obj_ptr animated_object::create(const sf::Sprite& p_sprite) throw(std::bad_alloc)
    {
        sprite_ptr_t sprite_ptr=std::make_shared<sf::Sprite>(p_sprite);
        anim_obj_ptr new_object(new animated_object(sprite_ptr));
        return new_object;
    }

    animated_object::animated_object(sprite_ptr_t p_sprite,
                                     std::shared_ptr<I_interpolation_algorithm> interpolation)
    {
        functions=interpolation;
        m_sprite=p_sprite;
        m_begin_position=m_sprite->getPosition();
        animation_speed_info=animation_speed_type::IS_NORMAL;

    }

    sf::Vector2f animated_object::get_position()
    {
        return m_sprite->getPosition();
    }

    sprite_ptr_t animated_object::get_sprite()
    {
        return m_sprite;
    }

    //This will change the position of the sprite as well
    sf::Vector2f animated_object::set_begin_position(const sf::Vector2f& position)
    {
        sf::Vector2f last_begin_pos=m_sprite->getPosition();
        m_sprite->setPosition(position);
        m_begin_position=position;
        return last_begin_pos;
    }

    sf::Vector2f animated_object::set_end_position(const sf::Vector2f& new_position)
    {
        sf::Vector2f last_end_position=m_end_position;
        m_end_position=new_position;
        return last_end_position;
    }

    void animated_object::repeat()
    {
        if(m_status==anim_obj_status::STATUS_COMPLETED)
        {
            m_current_position=0;
            m_status=anim_obj_status::STATUS_READY;
        }
    }

    anim_obj_status animated_object::frame_tick(sf::RenderWindow& p_rnd)
    {
        if(m_status!=anim_obj_status::STATUS_READY)
        {
            return m_status;
        }
        if(animation_speed_info==animation_speed_type::IS_SLOWER)
        {
            if(current_time>=expected_time_to_draw)//Reset the counter and do not change the position (skip the frame)
            {
                expected_time_to_draw+=single_time_increment;
                m_sprite->setPosition(get_current_position());
            }
            current_time+=single_frame_time_count;
        }
        else if(animation_speed_info==animation_speed_type::IS_FASTER)
        {
            while(current_time<expected_time_to_draw)
            {
                current_time+=single_time_increment;
                ++m_current_position;
            }
            expected_time_to_draw+=single_frame_time_count;
            --m_current_position;
            m_sprite->setPosition(get_current_position());
        }
        else //IS_NORMAL
        {
            m_sprite->setPosition(get_current_position());
        }
        //Draw the sprite
        p_rnd.draw(*m_sprite);
        return m_status;
    }

    void animated_object::set_animation_speed(float p_anim_duration,int p_frame_rate)
    {
        int amount_of_points=object_positions.size();
        if(amount_of_points>0)
        {
            //Time required to draw the current amount of points
            float time_needed_in_sec=get_animation_execution_time(p_frame_rate);
            if(p_anim_duration>time_needed_in_sec)
            {
                animation_speed_info=animation_speed_type::IS_SLOWER;
                single_time_increment=1/(amount_of_points/p_anim_duration);
            }
            else if(p_anim_duration<time_needed_in_sec)
            {
                animation_speed_info=animation_speed_type::IS_FASTER;
                single_time_increment=p_anim_duration/amount_of_points;
            }
            else
            {
                animation_speed_info=animation_speed_type::IS_NORMAL;
                single_time_increment=time_needed_in_sec/amount_of_points;
            }
            expected_time_to_draw=single_time_increment;
            single_frame_time_count=1.0/p_frame_rate;
        }

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

    anim_obj_status animated_object::prepare_to_render()
    {
        m_status=anim_obj_status::STATUS_READY;
        //Check if the sprite has a valid texture
        if(m_sprite->getTexture()==nullptr)
        {
            m_status=anim_obj_status::STATUS_NOT_READY;
        }
        int num=functions->calculate_interpolation(object_positions,m_begin_position,
                                                    m_end_position);
        if(num==0)
        {
            m_status=anim_obj_status::STATUS_NOT_READY;
        }
        m_current_position=0;
        //Set begin position
        m_sprite->setPosition(m_begin_position);
        return m_status;
    }

    float animated_object::get_animation_execution_time(int p_frame_rate)
    {
        return (float)object_positions.size()/p_frame_rate;
    }



    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animation_engine
    //
    //
    ///////////////////////////////////////////////////////////////////

    animation_engine::animation_engine(sf::RenderWindow& p_rnd_wnd,int p_frame_rate):
        m_frame_rate(p_frame_rate),m_rnd_wnd(p_rnd_wnd)
    {
    }

    int animation_engine::register_object(anim_obj_ptr p_obj,animated_obj_completion_opt p_action_when_completed)
    {
        anim_obj_container_entry new_entry;
        new_entry.m_anim_object=p_obj;
        new_entry.m_action_when_completed=p_action_when_completed;
        m_object_container.push_back(new_entry);
        return m_object_container.size();
    }

    draw_return_status animation_engine::draw()
    {
        draw_return_status status=draw_return_status::STATUS_OK;
        amount_of_obj_in_complete_state=0;
        anim_obj_status obj_status;
        for(auto& elem:m_object_container)
        {
            if(elem.m_to_be_removed)
            {
                ++amount_of_obj_in_complete_state;
                status=draw_return_status::STATUS_CLEANUP_NEEDED;
                continue; //Skip the objects which should be removed
            }
            obj_status=elem.m_anim_object->frame_tick(m_rnd_wnd);
            if(obj_status==anim_obj_status::STATUS_COMPLETED)
            {
                status=perf_action_on_completed_animation(elem);
                if(elem.m_action_when_completed!=animated_obj_completion_opt::ACTION_REPEAT_ANIMATION)
                {   //A repeating animation cannot be in the complete state, after the repeat() call
                    //is back in the 'ready' state
                    ++amount_of_obj_in_complete_state;
                }
            }
        }
        return status;
    }

    //The cleanup action is needed to remove from the container the object marked as m_to_be_removed{true}
    int animation_engine::clean_up()
    {
        std::list<anim_obj_container_entry>::iterator it=m_object_container.begin();
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
        }
        return draw_return_status::STATUS_OK;
    }

    //Return true if all the animated_object are in the STATUS_COMPLETE state
    bool animation_engine::check_if_all_completed()
    {
        return amount_of_obj_in_complete_state==m_object_container.size();
    }
}



































