#include "animation.hpp"

namespace animation_engine
{
    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animated_texture
    //
    //
    ///////////////////////////////////////////////////////////////////

    //Create an empty sprite pointer
    anim_texture_ptr animated_texture::create(const sf::Sprite& p_sprite)
    {
        sprite_ptr_t sprite_ptr=std::make_shared<sf::Sprite>(p_sprite);
        anim_texture_ptr new_object(new animated_texture(sprite_ptr));
        return new_object;
    }


    animated_texture::animated_texture(sprite_ptr_t p_sprite,
                                     std::shared_ptr<I_interpolation_algorithm> interpolation)
    {
        functions=interpolation;
        m_sprite=p_sprite;
        m_texture=std::make_shared<sf::Texture>(*p_sprite->getTexture());
        m_sprite->setTexture(*m_texture);
        m_begin_position=m_sprite->getPosition();
    }

    anim_obj_status animated_texture::prepare_to_render()
    {
        m_status=anim_obj_status::STATUS_READY;
        //Check if the sprite has a valid texture
        sf::Vector2u texture_size=m_sprite->getTexture()->getSize();
        if(m_sprite->getTexture()==nullptr||!texture_size.x||!texture_size.y)
        {
            m_status=anim_obj_status::STATUS_NOT_READY;
            return m_status;
        }
        int num=functions->calculate_interpolation(object_positions,m_begin_position,
                                                    m_end_position);
        if(num==0)
        {
            m_status=anim_obj_status::STATUS_NOT_READY;
        }
        m_current_position=0;
        //Set begin position
        m_cur_position=m_begin_position;
        m_sprite->setPosition(m_begin_position);
        return m_status;
    }

    sf::Vector2f animated_texture::get_position()
    {
        return m_sprite->getPosition();
    }

    //This will change the position of the sprite as well
    sf::Vector2f animated_texture::set_begin_position(const sf::Vector2f& position)
    {
        sf::Vector2f last_begin_pos=m_sprite->getPosition();
        m_sprite->setPosition(position);
        m_begin_position=position;
        return last_begin_pos;
    }

    sf::Vector2f animated_texture::set_end_position(const sf::Vector2f& new_position)
    {
        sf::Vector2f last_end_position=m_end_position;
        m_end_position=new_position;
        return last_end_position;
    }

    void animated_texture::draw_impl(sf::RenderWindow& p_rnd)
    {
        m_sprite->setPosition(m_cur_position);
        p_rnd.draw(*m_sprite);
    }

    const_sprite_ptr_t animated_texture::get_sprite()
    {
        return m_sprite;
    }

    texture_ptr_t animated_texture::get_texture()
    {
        return m_texture;
    }
}//animation_engine
