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
        double x_beg,y_beg;
        double x_end,y_end;
        int m_keyframe_density;
    public:
        animation_functions(const sf::Vector2f& p_begin,
                            const sf::Vector2f& p_end);
        int calculate_interpolation(std::vector<sf::Vector2f>& object_positions);
    };

    /*
     * The object which can be animated on the screen,
     * the starting and ending position need to be provided,
     * and the moving speed. By default the object moves straight
     * from the initial position to the end position.
     */
    class animated_object
    {
        sprite_ptr_t m_sprite;
        sf::Vector2f m_end_position;//This is where this sprite is moving
        std::vector<sf::Vector2f> object_positions;
        int m_frame_rate{0};
        std::shared_ptr<animation_functions> functions;
        void calculate_frame_density();
    public:
        animated_object(sprite_ptr_t p_sprite);

        void frame_tick(sf::RenderWindow& p_rnd); //A new frame may be rendered
        void set_frame_rate(int p_frame_rate);

        const sf::Vector2f& get_position();
        const sf::Vector2f& set_current_position(const sf::Vector2f& position);
        const sf::Vector2f& set_end_position(const sf::Vector2f& new_position);

        sf::Sprite& get_sprite();
        static animated_object create(const sf::Sprite& p_sprite) throw(std::bad_alloc);
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
