#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include "animation_refresh_mechanism.hpp"
#include <list>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>

//#define RUN_REGRESSION

namespace animation_engine
{

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

    using sprite_ptr_t=std::shared_ptr<sf::Sprite>;
    using text_ptr_t=std::shared_ptr<sf::Text>;
    using const_text_ptr_t=std::shared_ptr<const sf::Text>;
    using font_ptr_t=std::shared_ptr<sf::Font>;
    using const_font_ptr_t=std::shared_ptr<const sf::Font>;
    using const_sprite_ptr_t=std::shared_ptr<const sf::Sprite>;
    using texture_ptr_t=std::shared_ptr<sf::Texture>;

    //Possible state for the animated_texture class
    enum class anim_obj_status
    {
        STATUS_READY,     //Ready to be drawn
        STATUS_NOT_READY, //Not ready...
        STATUS_FAULTY,    //Something bad happened
        STATUS_COMPLETED, //The animation is finished
        STATUS_STOPPED,   //Do not move the object from the current position
    };


    /*
     * The object which can be animated on the screen,
     * the starting and ending position need to be provided,
     * and the moving speed. By default the object moves straight
     * from the initial position to the end position.
     */
    class animated_object
    {
    protected:
        anim_obj_status m_status;
        sf::Vector2f m_begin_position;
        sf::Vector2f m_end_position;//This is where this sprite is moving
        sf::Vector2f m_cur_position;

        std::vector<sf::Vector2f> object_positions;
        int m_current_position{0};
        sf::Vector2f get_current_position();

        std::shared_ptr<I_interpolation_algorithm> functions;

        float m_animation_duration{0};
        double m_single_time_increment{0};
        double m_current_time{0};
        double m_new_position_threshold{0};
        int m_refresh_frequency{0};
        void calculate_drawing_data();

        sf::Vector2f moving_object_draw_impl();
        virtual void draw_impl(sf::RenderWindow& p_rnd)=0;
    public:
        animated_object();

        virtual anim_obj_status draw(sf::RenderWindow& p_rnd);
        virtual anim_obj_status refresh();
        virtual bool set_refresh_frequency(int p_frequency);
        virtual anim_obj_status prepare_to_render()=0;

        virtual sf::Vector2f get_position()=0;
        virtual sf::Vector2f set_begin_position(const sf::Vector2f& position)=0;
        virtual sf::Vector2f set_end_position(const sf::Vector2f& new_position)=0;
        virtual void repeat();
        virtual anim_obj_status stop();

        virtual bool set_animation_duration(float p_anim_duration);
        virtual float get_animation_execution_time(int p_refresh_speed);

        virtual ~animated_object()=0;
    };

    class animated_texture;
    class animated_text;
    class animation_engine;

    using anim_obj_ptr=std::shared_ptr<animated_object>;
    using anim_texture_ptr=std::shared_ptr<animated_texture>;
    using anim_text_ptr=std::shared_ptr<animated_text>;

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

    class animated_texture : public animated_object
    {
        texture_ptr_t m_texture;
        sprite_ptr_t m_sprite;

        animated_texture(sprite_ptr_t p_sprite,
                        std::shared_ptr<I_interpolation_algorithm> interpolation=std::shared_ptr<linear_interpolation>(new linear_interpolation));
        void draw_impl(sf::RenderWindow& p_rnd);
    public:
        sf::Vector2f get_position();
        sf::Vector2f set_begin_position(const sf::Vector2f& position);
        sf::Vector2f set_end_position(const sf::Vector2f& new_position);
        anim_obj_status prepare_to_render();

        const_sprite_ptr_t get_sprite();
        texture_ptr_t get_texture();
        static anim_texture_ptr create(const sf::Sprite& p_sprite);
        ~animated_texture(){}
    };

    class animated_text : public animated_object
    {
        text_ptr_t m_text;
        font_ptr_t m_font;

        animated_text(text_ptr_t p_text,
                       std::shared_ptr<I_interpolation_algorithm> interpolation=std::shared_ptr<linear_interpolation>(new linear_interpolation));
        void draw_impl(sf::RenderWindow& p_rnd);
    public:
        static anim_text_ptr create(const sf::Text& p_text);

        anim_obj_status prepare_to_render();

        sf::Vector2f get_position();
        sf::Vector2f set_begin_position(const sf::Vector2f& position);
        sf::Vector2f set_end_position(const sf::Vector2f& new_position);

        const_text_ptr_t get_text();
        const_font_ptr_t get_font();
        std::string set_text(const std::string& p_str);
        void set_font(font_ptr_t p_font);
    };

    //Options that can be provided with the animated_texture in order to specify its behavior
    enum animated_obj_completion_opt
    {
        ACTION_DEFAULT,//The sprite will be removed
        ACTION_REPEAT_ANIMATION, //Move back to be begin position and repeat the animation
        ACTION_DONT_MOVE,//Simply do not move the object
        ACTION_REMOVE_ANIMATED_OBJECT, //Remove the object from the animation_engine container
    };

    //Return status from the draw function
    enum draw_return_status
    {
        STATUS_OK,
        STATUS_CLEANUP_NEEDED,
    };

    /*
     * Container for one or more animated_textures,
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
        using refresh_callback_ptr = anim_obj_status();
        using refresh_sys_ptr = std::unique_ptr<refresh_mechanism::I_animation_engine_refresh<refresh_callback_ptr>>;

        static refresh_sys_ptr get_default_refresh_sys()
        {
            return std::unique_ptr<refresh_mechanism::animation_engine_refresh<refresh_callback_ptr>>
                                    (new refresh_mechanism::animation_engine_refresh<refresh_callback_ptr>());
        }
        refresh_sys_ptr m_refresh;
        std::list<anim_obj_container_entry> m_object_container;
        int m_frame_rate{0};
        sf::RenderWindow& m_rnd_wnd;
        draw_return_status perf_action_on_completed_animation(anim_obj_container_entry& p_obj);
        int amount_of_obj_in_complete_state{0};
        int amount_of_obj_in_stop_state{0};
    public:
        animation_engine(sf::RenderWindow& p_rnd_wnd, int p_frame_rate,
                         refresh_sys_ptr p_refresh_mechanism = get_default_refresh_sys());
        ~animation_engine();
        int register_object(anim_obj_ptr p_obj,
                            animated_obj_completion_opt p_action_when_completed=animated_obj_completion_opt::ACTION_DEFAULT);
        draw_return_status draw();
        int clean_up(bool p_force_clean_all=false);
        bool check_if_all_completed();
        bool check_if_all_completed_or_stopped();
    };
}

#endif
