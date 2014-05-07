#include "animation.hpp"
#include <gmock.h>
#include <chrono>
#include <sstream>

#define RUN_REGRESSION

class animation_functions_test_small : public ::testing::Test
{
public:
    int get_y(int x)
    {
        double m=1;
        return m*(x+2)-3;
    }
    animation_engine::animation_functions functions;
    sf::Vector2f begin_position{-2,-3};
    sf::Vector2f end_position{-5,-6};
    animation_functions_test_small()
    {
    }
};

TEST_F(animation_functions_test_small,first_test)
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


class animation_functions_tests : public ::testing::Test
{
public:
    int get_y(int x)
    {
        double m=-0.54705882352941176470588235294118;
        return m*(x-120)-60;
    }
    animation_engine::animation_functions functions;
    sf::Vector2f begin_position{-50,33};
    sf::Vector2f end_position{120,-60};
    animation_functions_tests()
    {
    }
};

TEST_F(animation_functions_tests,first_test)
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
    ASSERT_THROW(animated_object->get_position(),std::out_of_range);//The container is empty
    sf::Vector2f end_position(53,-111);
    sf::Vector2f begin_position(-99,-45);
    ASSERT_EQ(anim_obj_status::NOT_READY,animated_object->prepare_to_render());//Texture missing
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_end_position(end_position));
    ASSERT_EQ(sf::Vector2f(0,0),animated_object->set_begin_position(begin_position));
    ASSERT_EQ(anim_obj_status::NOT_READY,animated_object->prepare_to_render());//Texture missing
    animated_object->get_sprite()->setTexture(test_texture);
    ASSERT_EQ(anim_obj_status::READY,animated_object->prepare_to_render());
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
    app.setFramerateLimit(150);

    sf::Texture texture;
    if(!texture.loadFromFile("texture.png"))
    {
        return -1;
    }

    sf::Sprite sprite(texture);
    animation_engine::anim_obj_ptr object=animation_engine::animated_object::create(sprite);

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
        object->frame_tick(app);
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

















