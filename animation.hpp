#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include <list>
#include <memory>

namespace animation_engine
{
    class animated_object;
    class animation_engine;

    using anim_obj_ptr=std::shared_ptr<animated_object>;
    using sprite_ptr_t=std::shared_ptr<sf::Sprite>;

    /*
     * Implement a set of function needed for the animation
     */
    class animation_functions
    {
        double x_beg{0},
               y_beg{0},
               x_end{0},
               y_end{0},
               m{0};
        float m_anim_speed{.1};
        void x_axis_interpolation(std::vector<sf::Vector2f>& elems);
        void y_axis_interpolation(std::vector<sf::Vector2f>& elems);
        void interpolation_impl(std::vector<sf::Vector2f>& elems,
                               std::function<int(int)> func,
                               std::function<void(int,int)> push,
                               int from,int to);
    public:
        animation_functions()=default;
        animation_functions(float p_anim_speed):m_anim_speed(p_anim_speed)
        {}
        int calculate_interpolation(std::vector<sf::Vector2f>& object_positions,
                                    const sf::Vector2f& p_begin,
                                    const sf::Vector2f& p_end);
    };

    //Possible state for the animated_object class
    enum anim_obj_status
    {
        READY,     //Ready to be drawn
        NOT_READY, //Not ready...
    };

    /*
     * The object which can be animated on the screen,
     * the starting and ending position need to be provided,
     * and the moving speed. By default the object moves straight
     * from the initial position to the end position.
     */
    class animated_object
    {
        anim_obj_status m_status;
        sprite_ptr_t m_sprite;
        sf::Vector2f m_begin_position;
        sf::Vector2f m_end_position;//This is where this sprite is moving

        std::vector<sf::Vector2f> object_positions;
        int m_current_position{0};

        int m_frame_rate{0};
        float m_anim_speed{0.1};
        std::shared_ptr<animation_functions> functions;
    public:
        animated_object(sprite_ptr_t p_sprite);

        void frame_tick(sf::RenderWindow& p_rnd); //A new frame may be rendered
        void set_frame_rate(int p_frame_rate);
        float set_animation_speed(float p_speed);

        sf::Vector2f get_position() throw(std::out_of_range);
        sf::Vector2f set_begin_position(const sf::Vector2f& position);
        sf::Vector2f set_end_position(const sf::Vector2f& new_position);
        anim_obj_status prepare_to_render();

        sprite_ptr_t get_sprite();
        static anim_obj_ptr create(const sf::Sprite& p_sprite) throw(std::bad_alloc);
    };

    /*
     * Container for one or more animated_objects,
     * each of which will be displayed on the screen
     */
    class animation_engine
    {
        std::list<anim_obj_ptr> object_container;
        int m_frame_rate{0};
        sf::RenderWindow& m_rnd_wnd;
    public:
        animation_engine(sf::RenderWindow& p_rnd_wnd, int p_frame_rate);
        int register_object(anim_obj_ptr obj);
        void draw();
    };
}

#endif
