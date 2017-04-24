#ifndef PARTICLE_RENDERER_HPP
#define PARTICLE_RENDERER_HPP

#include <string>
#include "Texture.hpp"
#include "ParticleSystem.hpp"

class ParticleRenderer {
public:
    virtual ~ParticleRenderer() = default;
    virtual void draw(const ParticleSystem&) = 0;

protected:
    ShaderProgram shaderProgram_;
};

class PointRenderer : public ParticleRenderer {
public:
    PointRenderer(const float pointSize);
    void draw(const ParticleSystem& particleSystem) override;

private:
    float pointSize_;
};

class BillboardRenderer : public ParticleRenderer {
public:
    BillboardRenderer(const std::string&, const float);
    void draw(const ParticleSystem& particleSystem) override;

private:
    Texture texture_;
    float billboardSize_;
};

#endif
