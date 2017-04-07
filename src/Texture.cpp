#include "Texture.hpp"
#include "OpenGLUtils.hpp"
#include "VectorField2D.hpp"
#include "VectorField3D.hpp"
#include <cmath>

Texture::~Texture(){
  glDeleteTextures(1, &_id);
}

Texture::Texture(const int width,const int height, const float* data): _width{width},_height{height}{
    _id = OpenGLUtils::createTexture(width, height, data);
}


void Texture::begin(){
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + _id);
        glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::end(){

}
