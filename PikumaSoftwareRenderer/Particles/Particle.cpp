#include "Particle.h"

namespace ParticleSpace
{
    Particle::Particle(/* args */)
    {
    }

    Particle::~Particle()
    {
    }

    void Particle::setAt(float x, float y, float z)
    {
        position.set(x, y, z);
    }

    void Particle::applyForce(Maths::Vector3f force)
    {
        acceleration.add(force);
    }

    void Particle::update(float deltaTime)
    {
        velocity.add(acceleration);
        position.add(velocity);
        acceleration.zero();

        lifespan -= 2.0;
    }

} // namespace Lights
