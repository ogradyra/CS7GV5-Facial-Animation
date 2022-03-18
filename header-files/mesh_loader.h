#pragma once

// Include Windows and Vector
#include <iostream>
#include <string> 
#include <fstream>
#include <functional>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

// Include Maths Functions
#include "maths_funcs.h"

// Eigen
#include <Eigen/Dense>
#include <Eigen/Sparse>

// Taken from https://github.com/SineadGalbraith/Facial-Animation/blob/main/Lab04/MeshLoader.h and 
// https://www.keithlantz.net/2011/10/a-preliminary-wavefront-obj-loader-in-c/

struct vertexData {
	std::vector<float> vertex;
	void normalize() {
		float magnitude = 0.0f;
		for (int i = 0; i < vertex.size(); i++)
			magnitude += pow(vertex[i], 2.0f);
		magnitude = sqrt(magnitude);
		for (int i = 0; i < vertex.size(); i++)
			vertex[i] /= magnitude;
	}
	vertexData operator-(vertexData v2) {
		vertexData v3;
		if (vertex.size() != v2.vertex.size()) {
			v3.vertex.push_back(0.0f);
			v3.vertex.push_back(0.0f);
			v3.vertex.push_back(0.0f);
		}
		else {
			for (int i = 0; i < vertex.size(); i++)
				v3.vertex.push_back(vertex[i] - v2.vertex[i]);
		}
		return v3;
	}
	vertexData cross(vertexData v2) {
		vertexData v3;
		if (vertex.size() != 3 || v2.vertex.size() != 3) {
			v3.vertex.push_back(0.0f);
			v3.vertex.push_back(0.0f);
			v3.vertex.push_back(0.0f);
		}
		else {
			v3.vertex.push_back(vertex[1] * v2.vertex[2] - vertex[2] * v2.vertex[1]);
			v3.vertex.push_back(vertex[2] * v2.vertex[0] - vertex[0] * v2.vertex[2]);
			v3.vertex.push_back(vertex[0] * v2.vertex[1] - vertex[1] * v2.vertex[0]);
		}
		return v3;
	}
};

struct face {
	std::vector<int> vertex;
	std::vector<int> texture;
	std::vector<int> normal;
};

class MeshLoader {
public:
	MeshLoader();
	MeshLoader(std::string filename);
	std::vector<float> meshVertices;
	std::vector<float> meshNormals;
	std::vector<float> meshTexCoords;
	int numVertices;
private:
	std::vector<vertexData> vertices;
	std::vector<vertexData> texCoords;
	std::vector<vertexData> normals;
	std::vector<face> faces;
};
