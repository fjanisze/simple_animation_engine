#include "animation.hpp"
#include <gmock.h>
#include <chrono>
#include <sstream>

#define RUN_REGRESSION

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

class animated_object_basic : public ::testing::Test
{
public:
    sf::Sprite test_sprite;
    sf::Texture test_texture;
    animated_object_basic()
    {
    }
};

using namespace animation_engine;


TEST_F(animated_object_basic,build_and_set_proper_values)
{
    anim_obj_ptr animated_object;
    try{
        animated_object=animated_object::create(test_sprite);
    }catch(...)
    {
        FAIL();
    }
    sf::Vector2f end_position(53,-111);
    sf::Vector2f begin_position(-99,-45);
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,animated_object->prepare_to_render());//Texture missing
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_end_position(end_position));
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_begin_position(begin_position));
    ASSERT_EQ(anim_obj_status::STATUS_NOT_READY,animated_object->prepare_to_render());//Texture missing
    animated_object->get_sprite()->setTexture(test_texture);
    ASSERT_EQ(anim_obj_status::STATUS_READY,animated_object->prepare_to_render());
    ASSERT_EQ(animated_object->get_position(),begin_position);
}

namespace helper_objects
{
    struct animated_object_mock : public I_animate_object
    {
        anim_obj_status m_status;

        MOCK_METHOD1(frame_tick,animation_engine::anim_obj_status(sf::RenderWindow&));
        MOCK_METHOD0(prepare_to_render,anim_obj_status());
        MOCK_METHOD1(set_animation_speed,int(int));

        MOCK_METHOD0(get_position,sf::Vector2f());
        MOCK_METHOD1(set_begin_position,sf::Vector2f(const sf::Vector2f&));
        MOCK_METHOD1(set_end_position,sf::Vector2f(const sf::Vector2f&));
        MOCK_METHOD0(repeat,void());

        MOCK_METHOD2(set_animation_speed,void(float,int));
        MOCK_METHOD1(get_animation_execution_time,float(int));

        MOCK_METHOD0(get_sprite,sprite_ptr_t());
    };
}

using ::testing::_;
using ::testing::DefaultValue;

struct animation_engine_testsuit : public ::testing::Test
{
    typedef ::testing::StrictMock<helper_objects::animated_object_mock> anim_obj_strict_mock;
    typedef std::shared_ptr<anim_obj_strict_mock> anim_obj_mock_ptr;

    sf::RenderWindow render_window;
    anim_obj_mock_ptr anim_obj1,
                     anim_obj2;
    animation_engine_testsuit()
    {
        anim_obj1=std::shared_ptr<anim_obj_strict_mock>(new anim_obj_strict_mock);
        anim_obj2=std::shared_ptr<anim_obj_strict_mock>(new anim_obj_strict_mock);
        //Default return value for frame_tick
        DefaultValue<animation_engine::anim_obj_status>::Set(animation_engine::anim_obj_status::STATUS_READY);
        DefaultValue<sf::Vector2f>::Set(sf::Vector2f(0,0));
    }
};

TEST_F(animation_engine_testsuit,create_2anim_obj_and_draw)
{
    animation_engine::animation_engine engine(render_window,40);
    ASSERT_EQ(1,engine.register_object(anim_obj1));
    ASSERT_EQ(2,engine.register_object(anim_obj2));
    ASSERT_EQ(3,engine.register_object(anim_obj2));

    EXPECT_CALL(*anim_obj1,frame_tick(_)).Times(1);
    EXPECT_CALL(*anim_obj2,frame_tick(_)).Times(2);

    ASSERT_EQ(draw_return_status::STATUS_OK,engine.draw());
}

TEST_F(animation_engine_testsuit,create_2anim_obj_and_check_repeat_action)
{
    animation_engine::animation_engine engine(render_window,40);

    //frame_tick need to retrn STATUS_COMPLETED in order to trigger perf_action_on_completed_animation
    DefaultValue<animation_engine::anim_obj_status>::Set(animation_engine::anim_obj_status::STATUS_COMPLETED);

    ASSERT_EQ(1,engine.register_object(anim_obj1,animated_obj_completion_opt::ACTION_REPEAT_ANIMATION));
    ASSERT_EQ(2,engine.register_object(anim_obj2,animated_obj_completion_opt::ACTION_REPEAT_ANIMATION));
    ASSERT_EQ(3,engine.register_object(anim_obj2,animated_obj_completion_opt::ACTION_REPEAT_ANIMATION));

    EXPECT_CALL(*anim_obj1,frame_tick(_)).Times(1);
    EXPECT_CALL(*anim_obj2,frame_tick(_)).Times(2);
    EXPECT_CALL(*anim_obj1,repeat()).Times(1);
    EXPECT_CALL(*anim_obj2,repeat()).Times(2);

    ASSERT_EQ(draw_return_status::STATUS_OK,engine.draw());
}

#ifdef RUN_REGRESSION

int main(int argc,char** argv)
{
    ::testing::InitGoogleMock(&argc,argv);
    return RUN_ALL_TESTS();
}

#else

int main()
{
    sf::RenderWindow app;
    app.create(sf::VideoMode(800, 600), "animation");
    app.setFramerateLimit(60);

    sf::Texture texture;
    if(!texture.loadFromFile("texture.png"))
    {
        return -1;
    }


    sf::Sprite sprite(texture);
    animation_engine::anim_obj_ptr object=animation_engine::animated_object::create(sprite);

    animation_engine::animation_engine engine(app,20);

    bool begin_point_ready=false;

    auto begin_time=std::chrono::high_resolution_clock::now();
    long fps_counter=0;

    while (app.isOpen())
    {
        // Process the events
        sf::Event event;
        while (app.pollEvent(event))
        {
            //Generic events
            switch(event.type)
            {
            case sf::Event::Closed:
                app.close();
                break;
            case sf::Event::MouseButtonPressed:
                {
                    if(!begin_point_ready)
                    {
                        object->set_begin_position(sf::Vector2f(event.mouseButton.x,event.mouseButton.y));
                        begin_point_ready=true;
                    }
                    else
                    {
                        object->set_end_position(sf::Vector2f(event.mouseButton.x,event.mouseButton.y));
                        object->prepare_to_render();
                        std::cout<<"Animation time: "<<object->get_animation_execution_time(60)<<std::endl;
                        object->set_animation_speed(.5,60);
                        engine.register_object(object,animation_engine::animated_obj_completion_opt::ACTION_REPEAT_ANIMATION);
                        //Create a new one
                        sf::Sprite sprite(texture);
                        object=animation_engine::animated_object::create(sprite);
                        begin_point_ready=false;
                    }
                }
                break;
            default:
                //Ignored
                ;
            };
        }
        app.clear(sf::Color::Black);
        auto time_now=std::chrono::high_resolution_clock::now();
        engine.draw();
        if(std::chrono::duration_cast<std::chrono::milliseconds>(time_now-begin_time)>=std::chrono::milliseconds{1000})
        {
            std::cout<<fps_counter<<std::endl;
            std::stringstream ss;
            ss<<"fsp: "<<fps_counter;
            fps_counter=0;
            begin_time=time_now;
        }
        ++fps_counter;
        app.display();

    };
    return 0;
}

#endif

















