#include "Texture.hpp"

Texture::~Texture(){
  glDeleteTextures(1, &_id);
}

Texture::Texture(){
	const GLfloat textureData[] = {
    	1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f,
        0.0f,0.0f,1.0f, 0.0f,0.0f,0.0f
    };

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, textureData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT );
}

void Texture::begin(){
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + _id);
        glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::end(){

}