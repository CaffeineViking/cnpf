#include "ParticleRenderer.hpp"

#include <string>
#include <vector>
#include <iostream>
#include "Shader.hpp"
#include "ShaderProgram.hpp"

PointRenderer::PointRenderer(const float pointSize)
    : pointSize_ { pointSize } {
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

void PointRenderer::draw(const ParticleSystem& particleSystem) {
}

BillboardRenderer::BillboardRenderer(const std::string& texturePath, const float billboardSize)
    : billboardSize_ { billboardSize } {
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

    std::vector<float> textureData;
    unsigned textureWidth, textureHeight;
    if(!OpenGLUtils::loadPNG(texturePath, textureWidth, textureHeight, textureData))
        std::cerr << "Failed to load '" << texturePath << "'!" << std::endl;
    texture_ = Texture {textureWidth, textureHeight, textureData.data()};
}

void BillboardRenderer::draw(const ParticleSystem& particleSystem) {
}
