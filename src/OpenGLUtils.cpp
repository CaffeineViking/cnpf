#include "OpenGLUtils.hpp"
#include "foreign/lodepng.h"
#include <iostream>
#include <sstream>
GLuint OpenGLUtils::createBuffer(int size, const float* data, GLenum usage)
{
    GLuint ret_val = 0;
    glGenBuffers(1,&ret_val);
    glBindBuffer(GL_ARRAY_BUFFER,ret_val);
    glBufferData(GL_ARRAY_BUFFER,size*sizeof(float),data,usage);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    return ret_val;
}

GLuint OpenGLUtils::createTexture(unsigned width,unsigned height, const float* data ){
 	GLuint ret_val = 0;
    glGenTextures(1,&ret_val);
    glBindTexture(GL_TEXTURE_2D,ret_val);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_FLOAT,data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return ret_val;
}

GLuint OpenGLUtils::createTexture3D(unsigned width,unsigned height,unsigned depth, const float* data ){
  GLuint ret_val = 0;
    glGenTextures(1,&ret_val);
    glBindTexture(GL_TEXTURE_3D,ret_val);
    glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,width,height,depth,0,GL_RGBA,GL_FLOAT,data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return ret_val;
}

// Assumes RGB values interleaved in the data vector on the form 0.0f - 1.0f for each channel 
bool OpenGLUtils::writePNG(const std::string& filePath, const unsigned width, const unsigned height, const std::vector<float>& data){
    std::vector<unsigned char> image;
    for(unsigned i = 0; i < width*height; i++){
      image.push_back((unsigned char)(data.at(i*4 + 0)*255));
      image.push_back((unsigned char)(data.at(i*4 + 1)*255));
      image.push_back((unsigned char)(data.at(i*4 + 2)*255));
      image.push_back((unsigned char)255);
    }

   //Encode the image
  unsigned error = lodepng::encode(filePath, image, width, height);
  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
   return true;
}

bool OpenGLUtils::loadPNG(const std::string& filePath, unsigned& width, unsigned& height, std::vector<float>& data){

  std::vector<unsigned char> image; //the raw pixels
  int error = lodepng::decode(image, width, height, filePath);
  data.resize(width*height*4,0.0f);
  //if there's an error, display it
  if(error){
  	std::cerr << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
  	return false;
  } 

  for(unsigned i = 0 ; i < width*height*4; i++){
  	data[i] = ((float)image[i]/255.0f);
  }
  return true;
}

void OpenGLUtils::bufferToPNG_X(const std::string& name, const std::vector<float>& data, int width, int height, int depth){
      for(int x = 0; x < width; x++) {
      std::vector<float> crossSection(height * depth * 4);
      for(int y = 0; y < height; y++) {
         for(int z = 0; z < depth; z++) {
          int globalIndex = (x * height * depth) + (y * depth) + z;
          int subIndex = (y * depth) + z;
          crossSection[subIndex * 4 + 0] = data.at(globalIndex * 4 + 0);
          crossSection[subIndex * 4 + 1] = data.at(globalIndex * 4 + 1);
          crossSection[subIndex * 4 + 2] = data.at(globalIndex * 4 + 2);
        }
     }
       std::ostringstream oss;
       oss << name << (x + 1) << ".png";
       OpenGLUtils::writePNG(oss.str(), height, depth, crossSection);
   }
}
void OpenGLUtils::bufferToPNG_Y(const std::string& name, const std::vector<float>& data, int width, int height, int depth){
      for(int y = 0; y< height; y++) {
      std::vector<float> crossSection(width * depth * 4);
      for(int x = 0; x < width; x++) {
         for(int z = 0; z < depth; z++) {
          int globalIndex = (x * height * depth) + (y * depth) + z;
          int subIndex = (x * depth) + z;
          crossSection[subIndex * 4 + 0] = data.at(globalIndex * 4 + 0);
          crossSection[subIndex * 4 + 1] = data.at(globalIndex * 4 + 1);
          crossSection[subIndex * 4 + 2] = data.at(globalIndex * 4 + 2);
        }
     }
       std::ostringstream oss;
       oss << name << (y + 1) << ".png";
       OpenGLUtils::writePNG(oss.str(), width, depth, crossSection);
   }
}
