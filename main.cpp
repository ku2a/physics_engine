#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <random>
#include "particle.h"
#include "constants.h"

sf::Color getRandomColor() {
    return sf::Color(
        std::rand() % 256,  // Red
        std::rand() % 256,  // Green
        std::rand() % 256   // Blue
    );
}

int main()
{
    //random init
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-10, 10);
    std::uniform_int_distribution<> distrib2(1, WIDTH);
    std::uniform_int_distribution<> distrib3(1, HEIGTH);

    //Create Window
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGTH}), "Physics engine");
    window.setFramerateLimit(Frame_rate);

    //Create particles
    std::vector<Particle> particles;

    sf::CircleShape fondo(BACK_RAD);
    fondo.setPointCount(500);
    fondo.setFillColor(sf::Color::Black);
    fondo.setOrigin(sf::Vector2f(BACK_RAD, BACK_RAD));
    fondo.setPosition(sf::Vector2f(WIDTH/ 2.f, HEIGTH/ 2.f));
    for (int i = 0; i<500; i++){
        
        float vel_x = distrib(gen);
        float vel_y = distrib(gen);
        
        float rand_width = distrib2(gen);
        
        float rand_height = distrib3(gen);
        particles.emplace_back(1,rand_width,rand_height,false,13,sf::Vector2f(vel_x, vel_y));
    }

    bool can_press = true;
    while (window.isOpen())
    {
        
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            can_press = true;
        }else if (can_press && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            can_press = false;
            sf::Vector2i localPosition = sf::Mouse::getPosition(window); 
            particles.emplace_back(1,localPosition.x,localPosition.y,false,13,sf::Vector2f(0,0));
        }
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        
        window.clear(sf::Color(50,50,50));
        window.draw(fondo);
        uint32_t sub_steps = 15;
        for(int k= 0; k<sub_steps;k++){
            for (int i = 0; i< particles.size(); i++){
                
                
                particles[i].EdgeCollisions();
                for (int j = i+1; j< particles.size();j++){
                    particles[i].ParticleCollision(particles[j]);  
                }
                particles[i].update(1/ (float )sub_steps);
            }
        }

        for (const auto& particle: particles) {
            sf::CircleShape circle(particle.radius);
            circle.setFillColor(particle.color);
            circle.setOrigin(sf::Vector2f(particle.radius, particle.radius));
            circle.setPosition(particle.position);
            window.draw(circle);
        }
        
       
        
        

        window.display();
    }
}