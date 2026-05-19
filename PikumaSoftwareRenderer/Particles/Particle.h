#pragma once

#include "Vector3f.h"

namespace ParticleSpace
{
    class Particle
    {
    private:
        /* data */
    public:
        Maths::Vector3f position{};
        Maths::Vector3f acceleration{};
        Maths::Vector3f velocity{};
        float lifespan = 255.0;

        Particle(/* args */);
        ~Particle();

        void setAt(float x, float y, float z);

        bool isAlive()
        {
            return lifespan > 0;
        }

        void applyForce(Maths::Vector3f force);

        virtual void update(float deltaTime);

        virtual void draw()
        {
        }
    };

} // namespace Lights
