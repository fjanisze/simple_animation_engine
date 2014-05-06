#include "animation.hpp"
#include <functional>

namespace animation_engine
{

    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animation_functions
    //
    //
    ///////////////////////////////////////////////////////////////////

    animation_functions::animation_functions(const sf::Vector2f& p_begin,
                                             const sf::Vector2f& p_end)
    {
        x_beg=p_begin.x;
        y_beg=p_begin.y;
        x_end=p_end.x;
        y_end=p_end.y;
    }

    int animation_functions::calculate_interpolation(std::vector<sf::Vector2f>& object_positions)
    {
        //y=mx+q
        using std::abs;
        double m=(y_end-y_beg)/
                 (x_end-x_beg);
        auto func=[&](int x)
        {
            return m*(x-x_end)+y_end;
        };
        //Include both the begin and end position
        int cur_x=x_beg;
        while(cur_x!=x_end)
        {
            int cur_y=func(cur_x);
            object_positions.push_back(sf::Vector2f(cur_x,cur_y));
            if(cur_x>x_end)
                --cur_x;
            else ++cur_x;
        }
        object_positions.push_back(sf::Vector2f(x_end,y_end));
        return object_positions.size();
    }

    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animated_object
    //
    //
    ///////////////////////////////////////////////////////////////////

    //Create an empty sprite pointer
    animated_object animated_object::create(const sf::Sprite& p_sprite) throw(std::bad_alloc)
    {
        sprite_ptr_t sprite_ptr=std::make_shared<sf::Sprite>(p_sprite);
        animated_object new_object(sprite_ptr);
        return new_object;
    }

    animated_object::animated_object(sprite_ptr_t p_sprite)
    {
        m_sprite=p_sprite;
    }

    void animated_object::set_frame_rate(int p_frame_rate)
    {
        m_frame_rate=p_frame_rate;
        functions->calculate_interpolation(object_positions);
    }

    void animated_object::frame_tick(sf::RenderWindow& p_rnd)
    {

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

