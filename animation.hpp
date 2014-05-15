#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include <list>
#include <memory>
#include <chrono>

namespace animation_engine
{
    using sprite_ptr_t=std::shared_ptr<sf::Sprite>;

    //Possible state for the animated_object class
    enum anim_obj_status
    {
        STATUS_READY,     //Ready to be drawn
        STATUS_NOT_READY, //Not ready...
        STATUS_FAULTY,    //Something bad happened
        STATUS_COMPLETED, //The animation is finished
    };

    /*
     * The object which can be animated on the screen,
     * the starting and ending position need to be provided,
     * and the moving speed. By default the object moves straight
     * from the initial position to the end position.
     */
    class I_animate_object
    {
    public:
        virtual anim_obj_status frame_tick(sf::RenderWindow& p_rnd)=0;
        virtual anim_obj_status prepare_to_render()=0;

        virtual sf::Vector2f get_position()=0;
        virtual sf::Vector2f set_begin_position(const sf::Vector2f& position)=0;
        virtual sf::Vector2f set_end_position(const sf::Vector2f& new_position)=0;
        virtual void repeat()=0;

        virtual void set_animation_speed(float p_anim_duration,int p_frame_rate)=0;
        virtual float get_animation_execution_time(int p_frame_rate)=0;

        virtual sprite_ptr_t get_sprite()=0;
    };

    class animated_object;
    class animation_engine;

    using anim_obj_ptr=std::shared_ptr<I_animate_object>;

    /*
     * Interface for the class containing the interpolation algorithms
     */
    class I_interpolation_algorithm
    {
    public:
        virtual int calculate_interpolation(std::vector<sf::Vector2f>& object_positions,
                                            const sf::Vector2f& p_begin,
                                            const sf::Vector2f& p_end) = 0;
    };

    /*
     * Implement a set of function needed for the animation
     */
    class linear_interpolation : public I_interpolation_algorithm
    {
        double x_beg{0},
               y_beg{0},
               x_end{0},
               y_end{0},
               m{0};
        void interpolation_impl(std::vector<sf::Vector2f>& elems,
                               std::function<int(int)> func,
                               std::function<void(int,int)> push,
                               int from,int to);

    public:
        int calculate_interpolation(std::vector<sf::Vector2f>& object_positions,
                                    const sf::Vector2f& p_begin,
                                    const sf::Vector2f& p_end);
    };

    class animated_object : public I_animate_object
    {
        anim_obj_status m_status;
        sprite_ptr_t m_sprite;
        sf::Vector2f m_begin_position;
        sf::Vector2f m_end_position;//This is where this sprite is moving

        std::vector<sf::Vector2f> object_positions;
        int m_current_position{0};
        sf::Vector2f get_current_position();

        std::shared_ptr<I_interpolation_algorithm> functions;

        enum animation_speed_type
        {
            IS_NORMAL=0,
            IS_SLOWER,
            IS_FASTER
        }animation_speed_info;
        double single_frame_time_count{0};
        double single_time_increment{0};
        double current_time{0};
        double expected_time_to_draw{0};

    public:
        animated_object(sprite_ptr_t p_sprite,
                        std::shared_ptr<I_interpolation_algorithm> interpolation=std::shared_ptr<linear_interpolation>(new linear_interpolation));

        anim_obj_status frame_tick(sf::RenderWindow& p_rnd); //A new frame may be rendered

        sf::Vector2f get_position();
        sf::Vector2f set_begin_position(const sf::Vector2f& position);
        sf::Vector2f set_end_position(const sf::Vector2f& new_position);
        void repeat();

        anim_obj_status prepare_to_render();
        void set_animation_speed(float p_anim_duration,int p_frame_rate);
        float get_animation_execution_time(int p_frame_rate);

        sprite_ptr_t get_sprite();
        static anim_obj_ptr create(const sf::Sprite& p_sprite) throw(std::bad_alloc);
    };

    //Options that can be provided with the animated_object in order to specify its behavior
    enum animated_obj_completion_opt
    {
        ACTION_DEFAULT,//The sprite will remain in the end position
        ACTION_REPEAT_ANIMATION, //Move back to be begin position and repeat the animation
        ACTION_REMOVE_ANIMATED_OBJECT, //Remove the object from the animation_engine container
    };

    /*
     * Container for one or more animated_objects,
     * each of which will be displayed on the screen
     */
    struct anim_obj_container_entry
    {
        anim_obj_ptr m_anim_object;
        animated_obj_completion_opt m_action_when_completed;
        bool m_to_be_removed{false};
    };
    class animation_engine
    {
        std::list<anim_obj_container_entry> m_object_container;
        int m_frame_rate{0};
        sf::RenderWindow& m_rnd_wnd;
        void perf_action_on_completed_animation(anim_obj_container_entry& p_obj);
    public:
        animation_engine(sf::RenderWindow& p_rnd_wnd, int p_frame_rate);
        int register_object(anim_obj_ptr p_obj,
                            animated_obj_completion_opt p_action_when_completed=animated_obj_completion_opt::ACTION_DEFAULT);
        void draw();
    };
}

#endif
