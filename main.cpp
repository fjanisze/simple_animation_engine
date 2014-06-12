#include "animation.hpp"

#include <chrono>
#include <gmock.h>
#include <sstream>

#ifdef RUN_REGRESSION

int main(int argc,char** argv)
{
    ::testing::InitGoogleMock(&argc,argv);
    return RUN_ALL_TESTS();
}

#else

#define SPRITE

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

#ifdef SPRITE
    sf::Sprite sprite(texture);
    animation_engine::anim_texture_ptr object=animation_engine::animated_texture::create(sprite);
#else
    sf::Text text;
    text.setString("Hello World");
    text.setCharacterSize(20);
    text.setColor(sf::Color::Red);
    text.setStyle(sf::Text::Bold);
    sf::Font font;
    if(!font.loadFromFile("consola.ttf"))
    {
        return -1;
    }
    animation_engine::anim_text_ptr object=animation_engine::animated_text::create(text);
    object->set_font(std::make_shared<sf::Font>(font));
#endif
    animation_engine::animation_engine engine(app,30);

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
                        std::cout<<"Animation time: "<<object->get_animation_execution_time(30)<<std::endl;
                        static int multiplier = 1;
                        object->set_animation_duration((float)1/multiplier);
                        multiplier*=2;
                        engine.register_object(object,animation_engine::animated_obj_completion_opt::ACTION_REPEAT_ANIMATION);
                        //Create a new one
#ifdef SPRITE
                        sf::Sprite sprite(texture);
                        object=animation_engine::animated_texture::create(sprite);
#else
                        object=animation_engine::animated_text::create(text);
                        object->set_font(std::make_shared<sf::Font>(font));
#endif
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

















