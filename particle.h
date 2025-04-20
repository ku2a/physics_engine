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
    sf::Vector2f acceleration;
    sf::Color color;
    float mass;
    float radius;
    bool is_pinned;

    Particle(int ID,float x, float y, bool pinned = false, double radius = 10, sf::Vector2f velocity = sf::Vector2f(0,500), sf::Color color = getRandomColor() ): position(x,y), velocity(velocity), 
    acceleration(0,GRAVITY), is_pinned(pinned) , color(color),radius(radius),mass(radius*radius), ID(ID) {}

    
    void update(float dt){
        velocity += acceleration*dt;
        
        position += velocity*dt;
        //acceleration *= 0.0f;
    }

    void EdgeCollisions(){

        const sf::Vector2f v    = sf::Vector2f(WIDTH/2,HEIGTH/2) - position;
        const float dist = sqrt(v.x * v.x + v.y * v.y);
        if (dist > (BACK_RAD - radius)) {
            const sf::Vector2f n = v / dist;
            position = sf::Vector2f(WIDTH/2,HEIGTH/2) - n * (BACK_RAD - radius);
            velocity = -Simmetry(velocity,v) * BOX_RESTITUTION;
        }
        /*if (position.x < radius){ 
            velocity.x*=-BOX_RESTITUTION;
            position.x = radius;
        }else if (position.x >(WIDTH-radius)){

            velocity.x*=-BOX_RESTITUTION;
            position.x = (WIDTH-radius);
        }else if (position.y < radius){

            velocity.y*=-BOX_RESTITUTION;
            position.y = radius;
        }else if(position.y > (HEIGTH-radius)){

            velocity.y*=-BOX_RESTITUTION;
            position.y = (HEIGTH-radius);
        }*/
    }

    void ParticleCollision(Particle& particle){
        sf::Vector2f diff = particle.position - position;
        float dist = std::hypot(diff.x,diff.y);

        if (dist == 0.0f) {
            float angle = 0;
            diff = sf::Vector2f(std::cos(angle), std::sin(angle)) * 0.01f; 
            dist = std::hypot(diff.x, diff.y);
        }
        if (dist < (radius+particle.radius)){
            sf::Vector2f normal = diff / dist;
            //Fix overlap
            float overlap  = radius + particle.radius - dist + 0.5;
            position += -(normal)*overlap/2.0f;
            particle.position += (normal)*overlap/2.0f;

            

            float dist = radius + particle.radius;
            diff = particle.position - position;
            //Change direction and velocity
            float mSum = mass + particle.mass;
            sf::Vector2f vDiff = particle.velocity - velocity;
            //Particle 1
            float num = dot(vDiff,diff);
            float den = mSum*dist*dist;
            sf::Vector2f DeltaVA = diff;
            velocity += ((1 + PART_RESTITUTION)*particle.mass*num/den) * diff;
            //Particle 2
            particle.velocity+= (-(1 + PART_RESTITUTION) * mass *num/den)*diff;
        }
    }

    void applyForce(sf::Vector2f force){
        acceleration = force/ mass;
    }
    


    private:

    float dot(sf::Vector2f vec1, sf::Vector2f vec2){
        return vec1.x*vec2.x + vec1.y*vec2.y;
    }
    static sf::Color getRandomColor() {
        return sf::Color(rand() % 256, rand() % 256, rand() % 256);
    }

    sf::Vector2f Simmetry(sf::Vector2f vec1, sf::Vector2f dir){
        float mod_dir = std::hypot(dir.x,dir.y);
        sf::Vector2f proy = dir * (dot(vec1,dir)/(mod_dir*mod_dir));
        return (2.0f*proy - vec1);
    }
};

#endif
