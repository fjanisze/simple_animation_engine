#include "animation.hpp"
#include <gmock/gmock.h>

class linear_interpolation_test_small : public ::testing::Test
{
public:
    int get_y(int x)
    {
        double m=1;
        return m*(x+2)-3;
    }
    animation_engine::linear_interpolation functions;
    sf::Vector2f begin_position{-2,-3};
    sf::Vector2f end_position{-5,-6};
    linear_interpolation_test_small()
    {
    }
};

TEST_F(linear_interpolation_test_small,first_test)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(4,functions.calculate_interpolation(positions,begin_position,end_position));
    //Check the limit values
    ASSERT_EQ(-2,positions[0].x);
    ASSERT_EQ(-3,positions[0].y);
    ASSERT_EQ(-5,positions.back().x);
    ASSERT_EQ(-6,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(-4,positions[1].y);
    ASSERT_EQ(-5,positions[2].y);
}


class linear_interpolation_tests : public ::testing::Test
{
public:
    int get_y(int x)
    {
        double m=-0.54705882352941176470588235294118;
        return m*(x-120)-60;
    }
    animation_engine::linear_interpolation functions;
    sf::Vector2f begin_position{-50,33};
    sf::Vector2f end_position{120,-60};
    sf::Vector2f begin_position2{0,0};
    sf::Vector2f end_position2{0,0};
    linear_interpolation_tests()
    {
    }
};

TEST_F(linear_interpolation_tests,first_test)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(171,functions.calculate_interpolation(positions,begin_position,end_position));
    //Check the limit values
    ASSERT_EQ(-50,positions[0].x);
    ASSERT_EQ(33,positions[0].y);
    ASSERT_EQ(120,positions.back().x);
    ASSERT_EQ(-60,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(get_y(positions[10].x),positions[10].y);
    ASSERT_EQ(get_y(positions[100].x),positions[100].y);
    ASSERT_EQ(get_y(positions[150].x),positions[150].y);
}

TEST_F(linear_interpolation_tests,try_zero_positions)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(1,functions.calculate_interpolation(positions,begin_position2,end_position2));
}


class linear_interpolation_tests_use_y : public ::testing::Test
{
public:
    animation_engine::linear_interpolation functions;
    sf::Vector2f begin_position{-4,7};
    sf::Vector2f end_position{-1,-6};
    sf::Vector2f begin_position2{0,50};
    sf::Vector2f end_position2{0,-50};
    linear_interpolation_tests_use_y()
    {
    }
};

TEST_F(linear_interpolation_tests_use_y,first_test)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(14,functions.calculate_interpolation(positions,begin_position,end_position));
    //Check the limit values
    ASSERT_EQ(-4,positions[0].x);
    ASSERT_EQ(7,positions[0].y);
    ASSERT_EQ(-1,positions.back().x);
    ASSERT_EQ(-6,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(-3,positions[2].x);
    ASSERT_EQ(-2,positions[5].x);
    ASSERT_EQ(-1,positions[10].x);
}

TEST_F(linear_interpolation_tests_use_y,second_test_x_is_zero)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(101,functions.calculate_interpolation(positions,begin_position2,end_position2));
    //Check the limit values
    ASSERT_EQ(0,positions[0].x);
    ASSERT_EQ(50,positions[0].y);
    ASSERT_EQ(0,positions.back().x);
    ASSERT_EQ(-50,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(40,positions[10].y);
    ASSERT_EQ(20,positions[30].y);
    ASSERT_EQ(-20,positions[70].y);
}

class linear_interpolation_tests_use_x : public ::testing::Test
{
public:
    animation_engine::linear_interpolation functions;
    sf::Vector2f begin_position{-4,0};
    sf::Vector2f end_position{7,0};
    sf::Vector2f begin_position2{50,0};
    sf::Vector2f end_position2{-50,0};
    linear_interpolation_tests_use_x()
    {
    }
};

TEST_F(linear_interpolation_tests_use_x,first_test)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(12,functions.calculate_interpolation(positions,begin_position,end_position));
    //Check the limit values
    ASSERT_EQ(-4,positions[0].x);
    ASSERT_EQ(0,positions[0].y);
    ASSERT_EQ(7,positions.back().x);
    ASSERT_EQ(0,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(-2,positions[2].x);
    ASSERT_EQ(1,positions[5].x);
    ASSERT_EQ(6,positions[10].x);
}

TEST_F(linear_interpolation_tests_use_x,second_test_x_is_zero)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(101,functions.calculate_interpolation(positions,begin_position2,end_position2));
    //Check the limit values
    ASSERT_EQ(50,positions[0].x);
    ASSERT_EQ(0,positions[0].y);
    ASSERT_EQ(-50,positions.back().x);
    ASSERT_EQ(0,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(40,positions[10].x);
    ASSERT_EQ(20,positions[30].x);
    ASSERT_EQ(-20,positions[70].x);
}

class animated_object_basic_testsuit : public ::testing::Test
{
public:
    sf::Sprite test_sprite;
    sf::Texture test_texture;
    sf::RenderWindow render_window;

    animated_object_basic_testsuit()
    {
        test_sprite.setTexture(test_texture);
    }
};

using namespace animation_engine;


TEST_F(animated_object_basic_testsuit,build_with_texture_size_0)
{
    anim_obj_ptr animated_object;
    try{
        animated_object=animated_texture::create(test_sprite);
    }catch(...)
    {
        FAIL();
    }
    sf::Vector2f end_position(53,-111);
    sf::Vector2f begin_position(-99,-45);
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,animated_object->prepare_to_render());//Texture size 0 and no interpolation points
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_end_position(end_position));
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_begin_position(begin_position));
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,animated_object->prepare_to_render());//Texture size 0
}

TEST_F(animated_object_basic_testsuit,build_and_set_proper_values)
{
    anim_obj_ptr animated_object;
    test_texture.create(10,10);
    test_sprite.setTexture(test_texture);
    try{
        animated_object=animated_texture::create(test_sprite);
    }catch(...)
    {
        FAIL();
    }
    ASSERT_EQ(anim_obj_status::STATUS_READY,animated_object->prepare_to_render());
}

TEST_F(animated_object_basic_testsuit,set_same_begind_and_end_coord_speedslow)
{
    anim_obj_ptr animated_object;
    test_texture.create(10,10);
    test_sprite.setTexture(test_texture);
    try{
        animated_object=animated_texture::create(test_sprite);
    }catch(...)
    {
        FAIL();
    }
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_end_position(sf::Vector2f(10,10)));
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_begin_position(sf::Vector2f(10,10)));
    ASSERT_EQ(anim_obj_status::STATUS_READY,animated_object->prepare_to_render());
    //Since begin position==end position, the first refresh should trigger the completion of the animation
    ASSERT_EQ(anim_obj_status::STATUS_COMPLETED,animated_object->refresh());
}

namespace helper_objects
{
    struct animation_engine_refresh_mock : public refresh_mechanism::I_animation_engine_refresh<anim_obj_status()>
    {
        using function_type=std::function<anim_obj_status()>;

        MOCK_METHOD1(set_refresh_internal_clock_rate,int(int));
        MOCK_METHOD0(get_refresh_internal_clock_rate,int());

        MOCK_METHOD0(start_internal_refresh_cycle,bool());
        MOCK_METHOD0(stop_internal_refresh_cycle,bool());
        MOCK_METHOD0(pause_internal_refresh_cycle,bool());
        MOCK_METHOD0(unpause_internal_refresh_cycle,void());

        MOCK_METHOD1(register_function,int(const function_type&));
        MOCK_METHOD1(unregister_function,int(const function_type&));
    };

    struct animated_object_mock : public animated_object
    {
        anim_obj_status m_status;

        MOCK_METHOD1(draw,animation_engine::anim_obj_status(sf::RenderWindow&));
        MOCK_METHOD0(prepare_to_render,anim_obj_status());
        MOCK_METHOD0(refresh,anim_obj_status());
        MOCK_METHOD1(set_refresh_frequency,bool(int));

        MOCK_METHOD1(draw_impl,void(sf::RenderWindow&));
        MOCK_METHOD0(get_position,sf::Vector2f());
        MOCK_METHOD1(set_begin_position,sf::Vector2f(const sf::Vector2f&));
        MOCK_METHOD1(set_end_position,sf::Vector2f(const sf::Vector2f&));

        MOCK_METHOD0(repeat,void());
        MOCK_METHOD0(stop,anim_obj_status());

        MOCK_METHOD1(set_animation_duration,bool(float));
        MOCK_METHOD1(get_animation_execution_time,float(int));
    };
}

using ::testing::_;
using ::testing::DefaultValue;
using ::testing::Return;

struct animation_engine_testsuit_common : public ::testing::Test
{
    typedef ::testing::StrictMock<helper_objects::animated_object_mock> anim_obj_strict_mock;
    typedef std::shared_ptr<anim_obj_strict_mock> anim_obj_mock_ptr;

    sf::RenderWindow render_window;
    anim_obj_mock_ptr anim_obj1,
                     anim_obj2;
    std::unique_ptr<animation_engine::animation_engine> engine;
    helper_objects::animation_engine_refresh_mock* raw_pointer_refresh_system;

    auto get_refresh_system_ptr()
    {
        return std::unique_ptr<refresh_mechanism::I_animation_engine_refresh<anim_obj_status()>>(raw_pointer_refresh_system);
    }

    animation_engine_testsuit_common()
    {
        raw_pointer_refresh_system = new helper_objects::animation_engine_refresh_mock();
    }
};

struct animation_engine_testsuit : public animation_engine_testsuit_common
{
    void set_common_expectation()
    {
        EXPECT_CALL(*anim_obj1,draw(_)).Times(1);
        EXPECT_CALL(*anim_obj2,draw(_)).Times(2);
        EXPECT_CALL(*anim_obj1,set_refresh_frequency(_)).Times(1);
        EXPECT_CALL(*anim_obj2,set_refresh_frequency(_)).Times(2);
        EXPECT_CALL(*raw_pointer_refresh_system,register_function(_)).Times(3);
        EXPECT_CALL(*raw_pointer_refresh_system,get_refresh_internal_clock_rate()).Times(3);
        EXPECT_CALL(*raw_pointer_refresh_system,stop_internal_refresh_cycle()).Times(1);
    }

    void full_construction_and_expectation()
    {
        //Set the expectation for the animation engine constructor
        EXPECT_CALL(*raw_pointer_refresh_system,set_refresh_internal_clock_rate(_)).Times(1);
        EXPECT_CALL(*raw_pointer_refresh_system,start_internal_refresh_cycle()).Times(1);
        ON_CALL(*raw_pointer_refresh_system,start_internal_refresh_cycle()).WillByDefault(Return(true));
        //Create the animation engine
        engine=std::unique_ptr<animation_engine::animation_engine>(new animation_engine::animation_engine(render_window,40,get_refresh_system_ptr()));

        anim_obj1=std::shared_ptr<anim_obj_strict_mock>(new anim_obj_strict_mock);
        anim_obj2=std::shared_ptr<anim_obj_strict_mock>(new anim_obj_strict_mock);

        //Default return value for draw
        DefaultValue<animation_engine::anim_obj_status>::Set(animation_engine::anim_obj_status::STATUS_READY);
        ON_CALL(*anim_obj1, set_refresh_frequency(_)).WillByDefault(Return(true));
        ON_CALL(*anim_obj2, set_refresh_frequency(_)).WillByDefault(Return(true));
        DefaultValue<sf::Vector2f>::Set(sf::Vector2f(0,0));

        set_common_expectation();
    }

    animation_engine_testsuit()
    {

    }
};

TEST_F(animation_engine_testsuit,create_2anim_obj_and_draw)
{
    full_construction_and_expectation();
	
    ASSERT_EQ(1,engine->register_object(anim_obj1));
    ASSERT_EQ(2,engine->register_object(anim_obj2));
    ASSERT_EQ(3,engine->register_object(anim_obj2));

    ASSERT_EQ(draw_return_status::STATUS_OK,engine->draw());
    ASSERT_FALSE(engine->check_if_all_completed());
}

TEST_F(animation_engine_testsuit,create_2anim_obj_and_check_repeat_action)
{
    full_construction_and_expectation();

    //draw need to return STATUS_COMPLETED in order to trigger perf_action_on_completed_animation
    DefaultValue<animation_engine::anim_obj_status>::Set(animation_engine::anim_obj_status::STATUS_COMPLETED);

    EXPECT_CALL(*anim_obj1,repeat()).Times(1);
    EXPECT_CALL(*anim_obj2,repeat()).Times(2);

    ASSERT_EQ(1,engine->register_object(anim_obj1,animated_obj_completion_opt::ACTION_REPEAT_ANIMATION));
    ASSERT_EQ(2,engine->register_object(anim_obj2,animated_obj_completion_opt::ACTION_REPEAT_ANIMATION));
    ASSERT_EQ(3,engine->register_object(anim_obj2,animated_obj_completion_opt::ACTION_REPEAT_ANIMATION));

    ASSERT_EQ(draw_return_status::STATUS_OK,engine->draw());
    ASSERT_FALSE(engine->check_if_all_completed());
}

TEST_F(animation_engine_testsuit,create_2anim_obj_and_check_delete_action)
{
    full_construction_and_expectation();

    //draw need to retrn STATUS_COMPLETED in order to trigger perf_action_on_completed_animation
    DefaultValue<animation_engine::anim_obj_status>::Set(animation_engine::anim_obj_status::STATUS_COMPLETED);

    ASSERT_EQ(1,engine->register_object(anim_obj1,animated_obj_completion_opt::ACTION_REMOVE_ANIMATED_OBJECT));
    ASSERT_EQ(2,engine->register_object(anim_obj2,animated_obj_completion_opt::ACTION_REMOVE_ANIMATED_OBJECT));
    ASSERT_EQ(3,engine->register_object(anim_obj2,animated_obj_completion_opt::ACTION_REMOVE_ANIMATED_OBJECT));

    ASSERT_EQ(draw_return_status::STATUS_CLEANUP_NEEDED,engine->draw());
    ASSERT_TRUE(engine->check_if_all_completed());
    ASSERT_EQ(0,engine->clean_up()); //Nothing should be left in the container
}

TEST_F(animation_engine_testsuit,create_2anim_obj_and_check_if_stop_work)
{
    full_construction_and_expectation();

    //draw need to retrn STATUS_COMPLETED in order to trigger perf_action_on_completed_animation
    DefaultValue<animation_engine::anim_obj_status>::Set(animation_engine::anim_obj_status::STATUS_COMPLETED);

    EXPECT_CALL(*anim_obj1,repeat()).Times(1);
    EXPECT_CALL(*anim_obj2,stop()).Times(2);

    ASSERT_EQ(1,engine->register_object(anim_obj1,animated_obj_completion_opt::ACTION_REPEAT_ANIMATION));
    ASSERT_EQ(2,engine->register_object(anim_obj2,animated_obj_completion_opt::ACTION_DONT_MOVE));
    ASSERT_EQ(3,engine->register_object(anim_obj2,animated_obj_completion_opt::ACTION_DONT_MOVE));

    ASSERT_EQ(draw_return_status::STATUS_OK,engine->draw());
    ASSERT_FALSE(engine->check_if_all_completed());
    ASSERT_FALSE(engine->check_if_all_completed_or_stopped());
}

TEST_F(animation_engine_testsuit, check_animation_engine_constructor)
{
    int frame_rate = 30;
    int refresh_rate = frame_rate * 50;
    EXPECT_CALL(*raw_pointer_refresh_system,set_refresh_internal_clock_rate(refresh_rate)).Times(1);
    EXPECT_CALL(*raw_pointer_refresh_system,start_internal_refresh_cycle()).Times(1);
    ON_CALL(*raw_pointer_refresh_system,start_internal_refresh_cycle()).WillByDefault(Return(true));

    //Create the animation engine
    animation_engine::animation_engine anim_engine(render_window,frame_rate,get_refresh_system_ptr());
}

struct animated_object_testsuit : public ::testing::Test
{
public:
    anim_obj_ptr anim_obj1,
                 anim_obj2,
                 anim_obj3;
    sf::Sprite sprite_1,
               sprite_2,
               sprite_3;
    sf::Texture texture_1,
                texture_2,
                texture_3;
    sf::RenderWindow render_window;
    animated_object_testsuit()
    {
        load_texture();
        anim_obj1=animated_texture::create(sprite_1);
        anim_obj2=animated_texture::create(sprite_2);
        anim_obj3=animated_texture::create(sprite_3);

        anim_obj1->set_begin_position(sf::Vector2f(10,10));
        anim_obj1->set_end_position(sf::Vector2f(12,10));

        anim_obj2->set_begin_position(sf::Vector2f(10,10));
        anim_obj2->set_end_position(sf::Vector2f(10,12));

        anim_obj3->set_begin_position(sf::Vector2f(2,2));
        anim_obj3->set_end_position(sf::Vector2f(0,0));
    }
    void load_texture()
    {
        texture_1.create(10,10);
        texture_2.create(20,20);
        texture_3.create(30,30);
        sprite_1.setTexture(texture_1);
        sprite_2.setTexture(texture_2);
        sprite_3.setTexture(texture_2);
    }
};

TEST_F(animated_object_testsuit,not_ready)
{
    //The prepare_to_render call is missing
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,anim_obj1->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,anim_obj2->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,anim_obj3->draw(render_window));
}

TEST_F(animated_object_testsuit,four_draw_and_complete)
{
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj1->prepare_to_render());
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj2->prepare_to_render());
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj3->prepare_to_render());

    for(short trigger_three_times=3;trigger_three_times>0;--trigger_three_times)
    {
        ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj1->refresh());
        ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj1->draw(render_window));
        ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj2->refresh());
        ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj2->draw(render_window));
        ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj3->refresh());
        ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj3->draw(render_window));
    }

    ASSERT_EQ(anim_obj_status::STATUS_COMPLETED,anim_obj1->refresh());
    ASSERT_EQ(anim_obj_status::STATUS_COMPLETED,anim_obj1->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_COMPLETED,anim_obj2->refresh());
    ASSERT_EQ(anim_obj_status::STATUS_COMPLETED,anim_obj2->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_COMPLETED,anim_obj3->refresh());
    ASSERT_EQ(anim_obj_status::STATUS_COMPLETED,anim_obj3->draw(render_window));
}

TEST_F(animated_object_testsuit,check_stopped_animation)
{
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj1->prepare_to_render());
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj2->prepare_to_render());
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj3->prepare_to_render());

    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj1->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj2->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj3->draw(render_window));

    ASSERT_EQ(anim_obj_status::STATUS_STOPPED,anim_obj1->stop());
    ASSERT_EQ(anim_obj_status::STATUS_STOPPED,anim_obj2->stop());
    ASSERT_EQ(anim_obj_status::STATUS_STOPPED,anim_obj3->stop());

    ASSERT_EQ(anim_obj_status::STATUS_STOPPED,anim_obj1->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_STOPPED,anim_obj2->draw(render_window));
    ASSERT_EQ(anim_obj_status::STATUS_STOPPED,anim_obj3->draw(render_window));
}

class animation_text_testsuit : public ::testing::Test
{
public:
    sf::Text m_text1;
    sf::Font m_font1;
    font_ptr_t test_font;
    anim_text_ptr anim_obj1;
    sf::Vector2f begin_position1{10,10};
    sf::Vector2f end_position1{20,20};
    animation_text_testsuit() : anim_obj1(animated_text::create(m_text1)),
                                test_font(std::make_shared<sf::Font>(m_font1))
    {
    }
};

TEST_F(animation_text_testsuit,basic_creation)
{
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,anim_obj1->prepare_to_render());
    ASSERT_EQ("",anim_obj1->set_text("Hello World"));
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,anim_obj1->prepare_to_render());
    //Add the font
    anim_obj1->set_font(test_font);
    ASSERT_EQ(anim_obj_status::STATUS_READY,anim_obj1->prepare_to_render());
    ASSERT_EQ("Hello World",anim_obj1->set_text(""));
}


class refresh_mechanism_testsuit : public ::testing::Test
{
public:
    using animated_object_mock=::testing::StrictMock<helper_objects::animated_object_mock>;
    using animated_object_mock_ptr=std::shared_ptr<animated_object_mock>;
    using ptr_type=anim_obj_status();

    animated_object_mock_ptr anim_obj1,
                             anim_obj2,
                             anim_obj3;

    refresh_mechanism::animation_engine_refresh<ptr_type> m_refresh;
    refresh_mechanism_testsuit()
    {
        anim_obj1=std::make_shared<animated_object_mock>();
        anim_obj2=std::make_shared<animated_object_mock>();
        anim_obj3=std::make_shared<animated_object_mock>();
    }
};

TEST_F(refresh_mechanism_testsuit,add_elements_refresh_remove)
{
    EXPECT_CALL(*anim_obj1,refresh()).Times(4);
    EXPECT_CALL(*anim_obj2,refresh()).Times(4);
    EXPECT_CALL(*anim_obj3,refresh()).Times(4);

    ASSERT_EQ(1000,m_refresh.set_refresh_internal_clock_rate(60));

    ASSERT_EQ(1,m_refresh.register_function(std::bind(&animated_object::refresh,anim_obj1)));
    ASSERT_EQ(2,m_refresh.register_function(std::bind(&animated_object::refresh,anim_obj2)));
    ASSERT_EQ(3,m_refresh.register_function(std::bind(&animated_object::refresh,anim_obj3)));

    ASSERT_TRUE(m_refresh.start_internal_refresh_cycle());
    //Wait for 50 ms, this should trigger the refresh 4 times for each anim_obj
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    //Stop the cycle
    ASSERT_TRUE(m_refresh.stop_internal_refresh_cycle());
    ASSERT_EQ(2,m_refresh.unregister_function(std::bind(&animated_object::refresh,anim_obj1)));
    ASSERT_EQ(1,m_refresh.unregister_function(std::bind(&animated_object::refresh,anim_obj3)));
}

TEST_F(refresh_mechanism_testsuit,remove_elements_while_refreshing)
{
    using ::testing::AtLeast;
    EXPECT_CALL(*anim_obj1,refresh()).Times(2);
    EXPECT_CALL(*anim_obj2,refresh()).Times(4);
    EXPECT_CALL(*anim_obj3,refresh()).Times(7);

    ASSERT_EQ(1000,m_refresh.set_refresh_internal_clock_rate(104));

    ASSERT_EQ(1,m_refresh.register_function(std::bind(&animated_object::refresh,anim_obj1)));
    ASSERT_EQ(2,m_refresh.register_function(std::bind(&animated_object::refresh,anim_obj2)));
    ASSERT_EQ(3,m_refresh.register_function(std::bind(&animated_object::refresh,anim_obj3)));

    ASSERT_TRUE(m_refresh.start_internal_refresh_cycle());
    //The erase operation on the container and the loop in refresh_all may overlap
    //If the data race is not properly handled
    std::this_thread::sleep_for(std::chrono::milliseconds{10});
    ASSERT_NO_THROW(m_refresh.unregister_function(std::bind(&animated_object::refresh,anim_obj1)));
    std::this_thread::sleep_for(std::chrono::milliseconds{20});
    ASSERT_NO_THROW(m_refresh.unregister_function(std::bind(&animated_object::refresh,anim_obj2)));
    std::this_thread::sleep_for(std::chrono::milliseconds{30});
    ASSERT_NO_THROW(m_refresh.unregister_function(std::bind(&animated_object::refresh,anim_obj3)));
    //Stop the cycle
    ASSERT_TRUE(m_refresh.stop_internal_refresh_cycle());
}





