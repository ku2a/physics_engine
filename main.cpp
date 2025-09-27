#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <random>
#include <unordered_map>
#include <vector>

#include "particle.h"
#include "constants.h"

// Seed para rand()
static bool rand_seeded = ([]{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    return true;
})();

sf::Color getRandomColor() {
    return sf::Color(
        std::rand() % 256,  // Red
        std::rand() % 256,  // Green
        std::rand() % 256   // Blue
    );
}

int calculate_number(float x, float y){
    // Calcula el key del cuadrado de área diameter*diameter en el que se encuentra el punto (x,y)
    int diameter = 2 * RADIUS;
    // número de casillas en ancho y alto
    int width_squares = (WIDTH + diameter - 1) / diameter;
    int height_squares = (HEIGTH + diameter - 1) / diameter;

    // índice de la casilla (1-based)
    int x_sqr = static_cast<int>(x) / diameter + 1;
    int y_sqr = static_cast<int>(y) / diameter + 1;

    int pos = (y_sqr - 1) * width_squares + x_sqr;
    return pos;
}

std::unordered_map<int, std::vector<Particle*>> Grid(std::vector<Particle>& particles){
    // Crea hashmap grid
    std::unordered_map<int, std::vector<Particle*>> grid;
    for (auto& particle : particles) {
        int pos = calculate_number(particle.position.x, particle.position.y);
        auto it = grid.find(pos);
        if (it == grid.end()) {
            grid[pos] = std::vector<Particle*>{ &particle };
        } else {
            it->second.emplace_back(&particle);
        }
    }
    return grid;
}

void HanddleCollisions(std::unordered_map<int, std::vector<Particle*>>& grid){
    // Maneja las colisiones de las partículas entre ellas 
    int diameter = 2 * RADIUS;
    int width_squares = (WIDTH + diameter - 1) / diameter;
    const std::vector<int> neighborOffsets = {
        0, -1, 1,
        -width_squares, -width_squares - 1, -width_squares + 1,
        width_squares, width_squares - 1, width_squares + 1
    };

    for (auto& pair : grid) {

        for (auto& particle : pair.second) {
            for (const auto& offset : neighborOffsets) {
                int pos = pair.first + offset;
                auto it = grid.find(pos);
                if (it != grid.end()) {
                    for (auto* part : it->second) {
                        // Evitamos procesar la misma pareja dos veces comparando punteros
                        if (particle < part) {
                            particle->ParticleCollision(*part);
                        }
                    }
                }
            }
        }
    }
}

int main()
{
    // Create Window (SFML 2.5)
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGTH), "Physics engine");
    window.setFramerateLimit(Frame_rate);

    sf::CircleShape fondo(BACK_RAD);
    fondo.setPointCount(500);
    fondo.setFillColor(sf::Color::Black);
    fondo.setOrigin(sf::Vector2f(BACK_RAD, BACK_RAD));
    fondo.setPosition(sf::Vector2f(WIDTH / 2.f, HEIGTH / 2.f));

    // Vector particulas
    std::vector<Particle> particles;

    // Si quieres iniciar aleatoriamente, descomenta y ajusta:
    /*
    int N = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-10, 10);
    std::uniform_int_distribution<> distrib2(1, WIDTH);
    std::uniform_int_distribution<> distrib3(1, HEIGTH);
    
    for (int i = 0; i < N; i++){
        float vel_x = distrib(gen);
        float vel_y = distrib(gen);
        float rand_width = distrib2(gen);
        float rand_height = distrib3(gen);
        particles.emplace_back(i, rand_width, rand_height, false, RADIUS, sf::Vector2f(vel_x, vel_y));
    }
    */

    int i = 0;
    bool can_press = true;

    while (window.isOpen())
    {
        // Manejo de eventos (SFML 2.5)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Crear particula al pulsar ratón (control de "debounce")
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            can_press = true;
        } else if (can_press && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            can_press = false;
            sf::Vector2i localPosition = sf::Mouse::getPosition(window); 
            particles.emplace_back(i, localPosition.x, localPosition.y, false, RADIUS, sf::Vector2f(0.f, 0.f));
            i++;
        }

        window.clear(sf::Color(50, 50, 50));
        window.draw(fondo);

        // Procesador de movimiento
        uint32_t sub_steps = 15;
        for (int k = 0; k < static_cast<int>(sub_steps); k++) {
            std::unordered_map<int, std::vector<Particle*>> grid = Grid(particles);
            HanddleCollisions(grid);
            for (size_t idx = 0; idx < particles.size(); idx++) {
                particles[idx].EdgeCollisions();
                particles[idx].update(1.0f / static_cast<float>(sub_steps));
            }
        }

        // Dibujar partículas
        for (const auto& particle : particles) {
            sf::CircleShape circle(particle.radius);
            circle.setFillColor(particle.color);
            circle.setOrigin(sf::Vector2f(particle.radius, particle.radius));
            circle.setPosition(particle.position);
            window.draw(circle);
        }

        window.display();
    }

    return 0;
}
