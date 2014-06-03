#include "animation.hpp"

namespace animation_engine
{
    ///////////////////////////////////////////////////////////////////
    //
    //
    //	Follow the implementation for animated_text
    //
    //
    ///////////////////////////////////////////////////////////////////

    anim_text_ptr animated_text::create(const sf::Text& p_text)
    {
        text_ptr_t text=std::make_shared<sf::Text>(p_text);
        anim_text_ptr new_object(new animated_text(text));
        return new_object;
    }

    animated_text::animated_text(text_ptr_t p_text,
                                   std::shared_ptr<I_interpolation_algorithm> interpolation)
    {
        m_text=p_text;
        functions=interpolation;
    }

    anim_obj_status animated_text::prepare_to_render()
    {
        m_status=anim_obj_status::STATUS_READY;
        //Check if the sprite has a valid texture
        if(m_text->getFont()==nullptr||!m_text->getString().getSize())
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
        m_text->setPosition(m_begin_position);
        return m_status;
    }

    sf::Vector2f animated_text::get_position()
    {
        return m_text->getPosition();
    }

    //This will change the position of the sprite as well
    sf::Vector2f animated_text::set_begin_position(const sf::Vector2f& position)
    {
        sf::Vector2f last_begin_pos=m_text->getPosition();
        m_text->setPosition(position);
        m_begin_position=position;
        return last_begin_pos;
    }

    sf::Vector2f animated_text::set_end_position(const sf::Vector2f& new_position)
    {
        sf::Vector2f last_end_position=m_end_position;
        m_end_position=new_position;
        return last_end_position;
    }

    const_text_ptr_t animated_text::get_text()
    {
        return m_text;
    }

    const_font_ptr_t animated_text::get_font()
    {
        return m_font;
    }

    void animated_text::set_font(font_ptr_t p_font)
    {
        m_font=p_font;
        m_text->setFont(*m_font);
    }

    std::string animated_text::set_text(const std::string& p_str)
    {
        std::string old_str=m_text->getString();
        m_text->setString(p_str.c_str());
        return old_str;
    }

    void animated_text::draw_impl(sf::RenderWindow& p_rnd)
    {
        m_text->setPosition(m_cur_position);
        p_rnd.draw(*m_text);
    }

}//animation_engine
