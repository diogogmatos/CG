#include <GL/glew.h>
extern "C" {
#include <GL/gl.h>
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
}

#define _USE_MATH_DEFINES
#include <math.h>

#include <unordered_map>

#include "model.hpp"

unsigned int counter = 0;

std::vector<float> positionsFloats(std::vector<Vertex> points) {
  std::vector<float> floats;
  for (const Vertex& point : points) {
    floats.push_back(point.position.x);
    floats.push_back(point.position.y);
    floats.push_back(point.position.z);
  }
  return floats;
}

std::vector<float> normalFloats(std::vector<Vertex> points) {
  std::vector<float> floats;
  for (const Vertex& point : points) {
    floats.push_back(point.normal.x);
    floats.push_back(point.normal.y);
    floats.push_back(point.normal.z);
  }
  return floats;
}

std::vector<float> textureFloats(std::vector<Vertex> points) {
  std::vector<float> floats;
  for (const Vertex& point : points) {
    floats.push_back(point.texture.x);
    floats.push_back(point.texture.y);
  }
  return floats;
}

std::vector<Vertex> generateVBO(const std::vector<Vertex>& points) {
  std::vector<Vertex> vbo;
  std::unordered_map<Vertex, int, VertexHash> index_map;
  for (const Vertex& point : points) {
    if (index_map.find(point) == index_map.end()) {
      index_map[point] = vbo.size();
      vbo.push_back(point);
    }
  }
  return vbo;
}

std::vector<unsigned int> generateIBO(const std::vector<Vertex>& points,
                                      const std::vector<Vertex>& vbo) {
  std::vector<unsigned int> ibo;
  std::unordered_map<Vertex, int, VertexHash> index_map;
  for (size_t i = 0; i < vbo.size(); ++i) {
    index_map[vbo[i]] = i;
  }
  for (const Vertex& point : points) {
    ibo.push_back(index_map[point]);
  }
  return ibo;
}

Model::Model() {
  this->filename = "";
  this->id = -1;
  this->initialized = false;
  counter++;
}

Model::Model(std::string filename, std::vector<Vertex> vbo,
             std::vector<unsigned int> ibo, int id,
             std::vector<Vertex> points) {
  this->filename = filename;
  this->vbo = vbo;
  this->ibo = ibo;
  this->id = id;
  this->initialized = false;
  this->_points = points;
}

Model::Model(std::string filename, std::vector<Vertex> points) {
  this->filename = filename;
  this->id = counter;
  this->vbo = generateVBO(points);
  this->ibo = generateIBO(points, this->vbo);
  this->initialized = false;
  this->_points = points;
  counter++;
}

void Model::setupModel() {
  std::vector<float> points = positionsFloats(this->vbo);
  std::vector<float> normals = normalFloats(this->vbo);
  std::vector<float> textures = textureFloats(this->vbo);

  // Generate and bind vertex buffer
  std::cout << "Generating buffers" << std::endl;
  std::cout << "Points size: " << points.size() / 3 << std::endl;
  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(),
               GL_STATIC_DRAW);

  std::cout << "Normals size: " << normals.size() / 3 << std::endl;
  glGenBuffers(1, &this->_normals);
  glBindBuffer(GL_ARRAY_BUFFER, this->_normals);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(),
               GL_STATIC_DRAW);

  std::cout << "Textures size: " << textures.size() << std::endl;
  glGenBuffers(1, &this->_textures);
  glBindBuffer(GL_ARRAY_BUFFER, this->_textures);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textures.size(),
               textures.data(), GL_STATIC_DRAW);

  // Generate and bind index buffer
  glGenBuffers(1, &this->_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * this->ibo.size(),
               this->ibo.data(), GL_STATIC_DRAW);
}

void Model::drawModel() {
  if (!this->initialized) {
    this->initialized = true;
    setupModel();
  }
  std::cout << "Setting up material" << std::endl;

  setupMaterial(this->material);

  std::cout << "Drawing model " << this->id << std::endl;

  std::cout << "VBO size: " << this->vbo.size() << std::endl;
  glColor3f(1.0f, 1.0f, 1.0f);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexPointer(3, GL_FLOAT, 0, 0);

  std::cout << "Normals size: " << this->vbo.size() << std::endl;
  glBindBuffer(GL_ARRAY_BUFFER, this->_normals);
  glNormalPointer(GL_FLOAT, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ibo);
  glDrawElements(GL_TRIANGLES, this->ibo.size(), GL_UNSIGNED_INT, 0);
}

std::vector<Vertex> Model::getPoints() { return this->_points; }
