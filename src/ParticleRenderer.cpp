#include "ParticleRenderer.hpp"

#include <string>
#include <vector>
#include <iostream>
#include "Shader.hpp"
#include "ShaderProgram.hpp"

PointParticleRenderer::PointParticleRenderer(const float pointSize)
    : ParticleRenderer { pointSize } {
    Shader vertexShader { "share/shaders/point.vert", GL_VERTEX_SHADER };
    Shader fragmentShader { "share/shaders/point.frag", GL_FRAGMENT_SHADER };

    // Compile our shaders.
    vertexShader.compile();
    fragmentShader.compile();

    // Links them together to a program.
    shaderProgram_.attach(vertexShader);
    shaderProgram_.attach(fragmentShader);
    // Shaders are automatically freed!!!
    // We only need the program.
}

void PointParticleRenderer::draw(const ParticleSystem& system,
                                 const MovingCamera& camera,
                                 const float time) {
    // Make sure we have bound an program.
    shaderProgram_.begin(); // Yea baby!!!
    // Fetch the MVP matrix of the camera.
    camera.update(shaderProgram_.getId());

    glPointSize(particleSize_);
    // Finally, render the particles. *PARTICLES*.
    int particles { system.getParticleCount(time)};
    glDrawArrays(GL_POINTS, 0, particles);
}

BillboardParticleRenderer::BillboardParticleRenderer(const std::string& texturePath, const float billboardSize)
    : ParticleRenderer { billboardSize }, texturePath_ { texturePath } {
    Shader vertexShader { "share/shaders/billboard.vert", GL_VERTEX_SHADER };
    Shader geometryShader { "share/shaders/billboard.geom", GL_GEOMETRY_SHADER };
    Shader fragmentShader { "share/shaders/billboard.frag", GL_FRAGMENT_SHADER };

    // Compile our shaders.
    vertexShader.compile();
    geometryShader.compile();
    fragmentShader.compile();

    // Links them together to a program.
    shaderProgram_.attach(vertexShader);
    shaderProgram_.attach(geometryShader);
    shaderProgram_.attach(fragmentShader);
    // Shaders are automatically freed!!!
    // We only need the program.

    // Load texture into GPU DDR memory.
    changeBillboardTexture(texturePath);

    // Set the location and all that kind of OpenGL stuff. Interesting thing to
    // note: if we do this later, it doesn't work. And I don't know why. WHY?!?
    GLint location { glGetUniformLocation(shaderProgram_.getId(), "diffuse") };
    glUniform1i(location, texture_.getId());
}

void BillboardParticleRenderer::changeBillboardTexture(const std::string& texturePath) {
    std::vector<float> textureData;
    unsigned textureWidth, textureHeight;
    if(!OpenGLUtils::loadPNG(texturePath, textureWidth, textureHeight, textureData))
        std::cerr << "Failed to load '" << texturePath << "'!" << std::endl;

    texturePath_ = texturePath; // For updating values in AntTwerkBark.
    texture_ = Texture { textureWidth, textureHeight, textureData.data() };
}

void BillboardParticleRenderer::draw(const ParticleSystem& system,
                                     const MovingCamera& camera,
                                     const float time) {
    // Make sure we have bound an program.
    shaderProgram_.begin(); // Yea baby!!!
    // Fetch the MVP matrix of the camera.
    camera.update(shaderProgram_.getId());
    texture_.begin(0); // Bind it to TU 0.

    // Also make the billboard size adjustable at runtime with this.
    glUniform1f(glGetUniformLocation(shaderProgram_.getId(), "size"),
                particleSize_);

    // Finally, render the particles. *PARTICLES*.
    int particles { system.getParticleCount(time)};
    glDrawArrays(GL_POINTS, 0, particles);
}

void TW_CALL setBillboardTextureCallback(const void* value, void* data) {
    ParticleRenderer* particleRenderer = static_cast<ParticleRenderer*>(data);
    BillboardTextureType textureType = *static_cast<const BillboardTextureType*>(value);
    if (auto br = dynamic_cast<BillboardParticleRenderer*>(particleRenderer)) {
        switch (textureType) {
        case FIRE: br->changeBillboardTexture("share/textures/fire.png"); break;
        case LINK: br->changeBillboardTexture("share/textures/link.png"); break;
        case SPHERE: br->changeBillboardTexture("share/textures/sphere.png"); break;
        case VERY_NICE_FISH: br->changeBillboardTexture("share/textures/fish.png"); break;
        case VECTOR: br->changeBillboardTexture("share/textures/arrow.png"); break;
        }
    }
}

void TW_CALL getBillboardTextureCallback(void* value, void* data) {
    ParticleRenderer* particleRenderer = static_cast<ParticleRenderer*>(data);
    BillboardTextureType* textureType = static_cast<BillboardTextureType*>(value);
    if (auto br = dynamic_cast<BillboardParticleRenderer*>(particleRenderer)) {
        std::string path { br->getTexturePath() };
        if (path == "share/textures/fire.png") *textureType = FIRE;
        else if (path == "share/textures/link.png") *textureType = LINK;
        else if (path == "share/textures/sphere.png") *textureType = SPHERE;
        else if (path == "share/textures/arrow.png") *textureType = VECTOR;
        else if (path == "share/textures/fish.png") *textureType = VERY_NICE_FISH;
    }
}

