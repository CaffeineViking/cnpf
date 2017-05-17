#ifndef PARTICLE_RENDERER_HPP
#define PARTICLE_RENDERER_HPP

#include <string>
#include "Texture.hpp"
#include "MovingCamera.hpp"
#include "ParticleSystem.hpp"

class ParticleRenderer {
public:
    virtual ~ParticleRenderer() = default;
    virtual void draw(const ParticleSystem&, const MovingCamera&, const float) = 0;
    const ShaderProgram& getProgram() const { return shaderProgram_; }

protected:
    ShaderProgram shaderProgram_;
};

class PointParticleRenderer : public ParticleRenderer {
public:
    PointParticleRenderer(const float pointSize);
    float getPointSize() const { return pointSize_; }
    void  setPointSize(const float value) { pointSize_ = value; }
    void draw(const ParticleSystem&, const MovingCamera&, const float) override;

private:
    float pointSize_;
};

class BillboardParticleRenderer : public ParticleRenderer {
public:
    void changeBillboardTexture(const std::string&);
    BillboardParticleRenderer(const std::string&, const float);
    float getBillboardSize() const { return billboardSize_; }
    void  setBillboardSize(const float value) { billboardSize_ = value; }
    void draw(const ParticleSystem&, const MovingCamera&, const float) override;
    Texture& getTexture() { return texture_; }

private:
    Texture texture_;
    float billboardSize_;
};

class SampledParticleRenderer : public ParticleRenderer {
public:
    void changeTexture(const std::string&);
    SampledParticleRenderer(const std::string&, const float);
    float getWidth() const { return width_; }
    void  setWidth(const float value) { width_ = value; }
    void draw(const ParticleSystem&, const MovingCamera&, const float) override;
    Texture& getTexture() { return texture_; }

private:
    Texture texture_;
    float width_;
};

#endif
