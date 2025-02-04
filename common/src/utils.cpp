#include "utils.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

std::string Point::toString() {
  std::ostringstream oss;
  oss << "(" << x << ", " << y << ", " << z << ")";
  return oss.str();
}

bool fileExists(const std::string& filepath) {
  return std::filesystem::exists(filepath);
}

std::vector<Point> parseFile(std::string filepath) {
  std::string filename = filepath;
  std::vector<Point> points;

  size_t dotIndex = filename.find_last_of(".");
  if (dotIndex == std::string::npos) {
    std::cerr << "Error: Unable to determine file extension for " << filepath
              << std::endl;
    return points;
  }

  if (!fileExists(filepath)) {
    std::cerr << "Error: File does not exist: " << filepath << std::endl;
  }

  std::string extension = filename.substr(dotIndex + 1);
  if (extension == "obj") {
    std::cout << "Processing .obj file: " << filename << std::endl;
    points = parseOBJfile(filename);
  } else if (extension == "3d") {
    std::cout << "Processing .3d file: " << filename << std::endl;
    points = parse3Dfile(filename);
  } else {
    std::cerr << "Error: Unsupported file format for " << filename << std::endl;
  }

  return points;
}

void saveToFile(
    const std::vector<Point>& points,
    const char* filepath) {  // Changed parameter type to const char*

  std::ofstream file(filepath);

  if (file.is_open()) {
    for (const auto& point : points) {
      file << point.x << " " << point.y << " " << point.z << "\n";
    }
    file.close();
    std::cout << "File saved successfully.\n";
  } else {
    std::cerr << "Unable to open file: " << filepath << std::endl;
  }
}

std::vector<Point> parseOBJfile(std::string filename) {
  std::vector<Point> points;
  std::vector<Point> sorted_points;
  std::ifstream file(filename);

  std::string line;
  printf("Reading file\n");
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string type;
    iss >> type;
    if (type == "v") {
      Point point;
      iss >> point.x >> point.y >> point.z;
      points.push_back(point);
    } else if (type == "f") {
      std::string vertex;
      std::vector<int> vertex_indices;
      while (iss >> vertex) {
        std::string index = vertex.substr(0, vertex.find("/"));
        vertex_indices.push_back(std::stoi(index) - 1);
      }
      for (int i = 0; i < vertex_indices.size(); i++) {
        sorted_points.push_back(points[vertex_indices[i]]);
      }
    }
  }
  file.close();

  return sorted_points;
}

std::vector<Point> parse3Dfile(std::string filename) {
  std::vector<Point> points;
  std::ifstream file(filename);

  Point point;
  while (file >> point.x >> point.y >> point.z) {
    points.push_back(point);
  }

  file.close();

  return points;
}

Point Point::multiply(float value) {
  return Point(x * value, y * value, z * value);
}