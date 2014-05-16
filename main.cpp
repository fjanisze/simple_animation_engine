#include "animation.hpp"

#include <chrono>
#include <gmock.h>
#include <sstream>

#define RUN_REGRESSION

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

















