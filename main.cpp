#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
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
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGTH}), "SFML works!");

    window.setFramerateLimit(Frame_rate);
    std::vector<Particle> particles;
    particles.emplace_back(1,WIDTH/2,RADIOUS,false,10,sf::Vector2f(0,500));
    particles.emplace_back(2,WIDTH/2,(HEIGTH-20),false,40,sf::Vector2f(0,-300));
    int i = 0;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        i++;
        window.clear();

        for (const auto& particle: particles) {
            sf::CircleShape circle(particle.radius);
            circle.setFillColor(particle.color);
            circle.setOrigin(sf::Vector2f(particle.radius, particle.radius));
            circle.setPosition(particle.position);
            window.draw(circle);
        }
        
        for (size_t i = 0; i < particles.size(); ++i) {
            particles[i].update(1/ (float) Frame_rate);
            for (size_t j = i + 1; j < particles.size(); ++j) {
                particles[i].ParticleCollision(particles[j]);
            }
        }
        
        // Actualizar velocidades si usas next_velocity
        for (auto& particle : particles) {
            particle.updateVelocity();
        }
        
        // Dibujar (una sola vez)
       

        if (i%(Frame_rate / 5) == 0)
        particles.emplace_back(3,RADIOUS,5,false,10,sf::Vector2f(800,100));

        window.display();
    }
}