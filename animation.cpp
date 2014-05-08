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

    int animation_functions::calculate_interpolation(std::vector<sf::Vector2f>& object_positions,
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

    void animation_functions::interpolation_impl(std::vector<sf::Vector2f>& elems,
                                                std::function<int(int)> func,
                                                std::function<void(int,int)> push,
                                                int from,int to)
    {
        while(from!=to)
        {
            int cur=func(from);
            push(from,cur);
            if(from>to)
                --from;
            else ++from;
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

    animated_object::animated_object(sprite_ptr_t p_sprite)
    {
        functions=std::make_shared<animation_functions>();
        m_sprite=p_sprite;
        m_begin_position=m_sprite->getPosition();
    }

    void animated_object::set_frame_rate(int p_frame_rate)
    {
        m_frame_rate=p_frame_rate;
    }

    sf::Vector2f animated_object::get_position() throw(std::out_of_range)
    {
        return object_positions.at(m_current_position);
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

    void animated_object::frame_tick(sf::RenderWindow& p_rnd)
    {
        if(m_status==anim_obj_status::NOT_READY) return;
        if(m_current_position>=object_positions.size())
        {
            return;
        }
        m_sprite->setPosition(object_positions[m_current_position]);
        ++m_current_position;
        //Draw the sprite
        p_rnd.draw(*m_sprite);
    }

    anim_obj_status animated_object::prepare_to_render()
    {
        m_status=anim_obj_status::READY;
        //Check if the sprite has a valid texture
        if(m_sprite->getTexture()==nullptr)
        {
            m_status=anim_obj_status::NOT_READY;
        }
        int num=functions->calculate_interpolation(object_positions,m_begin_position,
                                                    m_end_position);
        if(num==0)
        {
            m_status=anim_obj_status::NOT_READY;
        }
        m_current_position=0;
        return m_status;
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

    int animation_engine::register_object(anim_obj_ptr obj)
    {
        obj->set_frame_rate(m_frame_rate);
        object_container.push_back(obj);
        return object_container.size();
    }

    void animation_engine::draw()
    {
        for(auto elem:object_container)
        {
            elem->frame_tick(m_rnd_wnd);
        }
    }
}

