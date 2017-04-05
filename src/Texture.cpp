#include "Texture.hpp"
#include "VectorField2D.hpp"
#include "VectorField3D.hpp"
#include <cmath>

Texture::~Texture(){
  glDeleteTextures(1, &_id);
}

Texture::Texture(){
  
  VectorField2D vectorField(glm::vec2(0,0), glm::vec2(100,100));
  //VectorField3D vectorField(glm::vec3(0,0,0), glm::vec3(100,100,100));

  std::vector<glm::vec2> field = vectorField.curl().getField();
  //std::vector<glm::vec3> field = vectorField.curl().getField();
  int width = vectorField.getWidth();
  int height = vectorField.getHeight();
  //  float scl = 0.001;

  GLfloat textureData[width*height*3];

  for(int i = 0; i < width*height*3; i += 3){
    glm::vec2 p = glm::normalize(field.at(i/3));
    //glm::vec3 p = glm::normalize(field.at(i/3));
    textureData[i] = std::abs(p.x);
    textureData[i+1] = std::abs(p.y);
    textureData[i+2] = 0.5f; //std::abs(p.z);
  }

  // const GLfloat textureData[] = {
  //   	  1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,0.0f,
  //   	  0.0f,0.0f,1.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f,
  //   	  1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,0.0f
  //   };

  // const GLfloat textureData[] = {
  //   	  1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f,
  //   	  0.0f,0.0f,1.0f, 0.0f,0.0f,0.0f
  //   };

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    //glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 3, 3, 0, GL_RGB, GL_FLOAT, textureData);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, textureData);

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
