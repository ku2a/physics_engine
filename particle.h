#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "constants.h"
class Particle{

    public:
    int ID;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f next_velocity;
    sf::Vector2f acceleration;
    sf::Color color;
    double mass;
    double radius;
    bool is_pinned;

    Particle(int ID,float x, float y, bool pinned = false, double radius = RADIOUS, sf::Vector2f velocity = sf::Vector2f(0,500), sf::Color color = sf::Color::White ): position(x,y), velocity(velocity), 
    acceleration(0,980), is_pinned(pinned) , color(color),radius(radius),mass(radius*radius),next_velocity(0,500), ID(ID) {}

    
    void update(float time_step) {
        // verlet intergration
        if (!is_pinned) {
            next_velocity = velocity + acceleration*time_step;
            position += velocity*time_step;
        }
        HandleCollisions();
        
    }
    void HandleCollisions(){
        if ( (position.x <= radius && velocity.x<0)){
            position.x = radius;
            next_velocity.x *= -RESTITUTION;
            
        }
        if ((position.x >= (WIDTH-radius) && velocity.x>0)){
            position.x = (WIDTH-radius);
            next_velocity.x *= -RESTITUTION;
            
        }
        if  (position.y <= radius && velocity.y<0){
            position.y = radius;
            next_velocity.y *= -RESTITUTION;
        }
        if  ((position.y >= (HEIGTH-radius) && velocity.y>0)){
            position.y = (HEIGTH-radius);
            next_velocity.y *= -RESTITUTION;
        }
    }
    void ParticleCollision(Particle& other) {
        // 1. Calcular vector de diferencia y distancia
        sf::Vector2f delta_pos = other.position - position;
        float distance_sq = delta_pos.x*delta_pos.x + delta_pos.y*delta_pos.y;
        float min_distance = radius + other.radius;
        
        // Salida temprana si no hay colisión
        if(distance_sq > min_distance*min_distance || distance_sq < 0.0001f)
            return;
        
        float distance = std::sqrt(distance_sq);
        sf::Vector2f collision_normal = delta_pos / distance;
    
        /* ========== CORRECCIÓN DE POSICIÓN (PRIMERO) ========== */
        float overlap = min_distance - distance;
        if(overlap > 0.0f) {
            float total_inv_mass = 1.0f/mass + 1.0f/other.mass;
            sf::Vector2f correction = collision_normal * (1 * overlap / total_inv_mass);
            
            position -= correction * static_cast<float>(1.0f/mass);
            other.position += correction * static_cast<float>(1.0f/other.mass);
            
            // Recalcular normal después de corrección
            delta_pos = other.position - position;
            distance = min_distance;
            collision_normal = delta_pos / distance;
        }
    
        /* ========== RESPUESTA DE COLISIÓN ========== */
        sf::Vector2f rel_velocity = other.velocity - velocity;
        float velocity_along_normal = rel_velocity.x*collision_normal.x + rel_velocity.y*collision_normal.y;
        
        // Caso especial: colisión frontal perfecta
        if(std::abs(velocity_along_normal) < 0.001f && distance < min_distance*1.01f) {
            // Solución para colisiones frontales:
            // 1. Detener ambas partículas en dirección normal
            float v1 = velocity.x*collision_normal.x + velocity.y*collision_normal.y;
            float v2 = other.velocity.x*collision_normal.x + other.velocity.y*collision_normal.y;
            
            // 2. Intercambiar velocidades normales con restitución
            float new_v1 = (mass*v1 + other.mass*v2 + other.mass*RESTITUTION*(v2-v1))/(mass+other.mass);
            float new_v2 = (mass*v1 + other.mass*v2 + mass*RESTITUTION*(v1-v2))/(mass+other.mass);
            
            // 3. Aplicar nuevas velocidades
            velocity += collision_normal * (new_v1 - v1);
            other.velocity += collision_normal * (new_v2 - v2);
            if(velocity_along_normal < 0.1f) { // Solo cuando la velocidad es pequeña
                velocity *= (0.9f);
                other.velocity *= (0.9f);
            }
            return;
        }
        
        // Colisión normal
        float j = -(1.0f + RESTITUTION) * velocity_along_normal;
        j /= (1.0f/mass + 1.0f/other.mass);
        
        sf::Vector2f impulse = collision_normal * j;
        velocity -= impulse * static_cast<float>(1.0f/mass);
        other.velocity += impulse * static_cast<float>(1.0f/other.mass);
        if(velocity_along_normal < 0.1f) { // Solo cuando la velocidad es pequeña
            velocity *= (0.9f);
            other.velocity *= (0.9f);
        }
    }  

    

    void updateVelocity(){
        velocity = next_velocity;
    }
    private:

    void mult (sf::Vector2f& vec1, float scalar){
        vec1.x *= scalar;
        vec1.y *= scalar;
        return;
    }
    static float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x * b.x + a.y * b.y;
    }
};

#endif
