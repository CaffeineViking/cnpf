#include "Texture.hpp"
#include "OpenGLUtils.hpp"

Texture::~Texture(){
    // See if have moved away the value.
    if (_valid) glDeleteTextures(1, &_id);
}

Texture::Texture(const unsigned width, const unsigned height, const float* data): _width{width},_height{height}{
    _id = OpenGLUtils::createTexture(width, height, data);
    _valid = true;
}

Texture::Texture(Texture&& otherTexture) {
    _valid = false;
    _id = otherTexture._id;
    _width = otherTexture._width;
    _height = otherTexture._height;
}

Texture& Texture::operator=(Texture&& otherTexture) {
    _valid = false;
    _id = otherTexture._id;
    _width = otherTexture._width;
    _height = otherTexture._height;
    return *this;
}


void Texture::begin(int textureUnit){
    glEnable(GL_TEXTURE_2D);

    // NOTE: texturing units are *not* the same as the
    // handle you are given by OpenGL! You GPU has only
    // a limited amount of texturing units (16 I think)
    // and shaders can thus only have 16 textures active
    // at a time. Therefore, we need to handle these manually
    // (or create some system that partitions these resources).
    // Since we have a small program this isn't really needed.
    //
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::end(){
    glBindTexture(GL_TEXTURE_2D, 0);
}
