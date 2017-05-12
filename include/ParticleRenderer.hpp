#ifndef PARTICLE_RENDERER_HPP
#define PARTICLE_RENDERER_HPP

#include <string>
#include "Texture.hpp"
#include "MovingCamera.hpp"
#include "ParticleSystem.hpp"

enum ParticleRendererType {
    POINT = 0,
    BILLBOARD,
    BILLBOARD_STRIP
};

class ParticleRenderer {
public:
    virtual ~ParticleRenderer() = default;
    ParticleRenderer(float particleSize) : particleSize_ { particleSize } {}
    float* refParticleSize() { return &particleSize_; };
    float getParticleSize() const { return particleSize_; }
    void setParticleSize(const float value) { particleSize_ = value; }
    virtual void draw(const ParticleSystem&, const MovingCamera&, const float) = 0;
    const ShaderProgram& getProgram() const { return shaderProgram_; }

protected:
    float particleSize_;
    ShaderProgram shaderProgram_;
};

class PointParticleRenderer : public ParticleRenderer {
public:
    PointParticleRenderer(const float pointSize);
    void draw(const ParticleSystem&, const MovingCamera&, const float) override;
};

class BillboardParticleRenderer : public ParticleRenderer {
public:
    void changeBillboardTexture(const std::string&);
    BillboardParticleRenderer(const std::string&, const float);
    void draw(const ParticleSystem&, const MovingCamera&, const float) override;
    std::string& getTexturePath() { return texturePath_; }
    Texture& getTexture() { return texture_; }

private:
    std::string texturePath_;
    Texture texture_;
};

#endif
