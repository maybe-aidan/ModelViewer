#include "model.h"
#include <iostream>
#include <fstream>
#include <sstream>

Model::Model() { }

Model::~Model() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

bool Model::loadOBJ(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "ERROR::MODEL::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.substr(0, 2) == "v ") {
			parseVertex(line);
		}
		else if (line.substr(0, 3) == "vt ") {
			parseTexCoord(line);
		}
		else if (line.substr(0, 3) == "vn ") {
			parseNormal(line);
		}
		else if (line.substr(0, 2) == "f ") {
			parseFace(line);
		}
	}

	file.close();

	setupBuffers();

	return true;
}

void Model::render(Shader shader) {
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	shader.use();

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Model::setupBuffers() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	if (!texCoords.empty()) {
		GLuint texVbo;
		glGenBuffers(1, &texVbo);
		glBindBuffer(GL_ARRAY_BUFFER, texVbo);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
	}
	if (!normals.empty()) {
		GLuint normalVbo;
		glGenBuffers(1, &normalVbo);
		glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
	}

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Model::parseVertex(const std::string& line) {
	std::istringstream s(line.substr(2));
	glm::vec3 vertex;
	s >> vertex.x >> vertex.y >> vertex.z;
	vertices.push_back(vertex);
}
void Model::parseTexCoord(const std::string& line) {
	std::istringstream s(line.substr(3));
	glm::vec2 texCoord;
	s >> texCoord.x >> texCoord.y;
	texCoords.push_back(texCoord);
}
void Model::parseNormal(const std::string& line) {
	std::istringstream s(line.substr(3));
	glm::vec3 normal;
	s >> normal.x >> normal.y >> normal.z;
	normals.push_back(normal);
}
void Model::parseFace(const std::string& line) {
	std::stringstream s(line.substr(2));
	std::vector<unsigned int> vIndices;
	std::vector<unsigned int> tIndices; 
	std::vector<unsigned int> nIndices;
	std::string vertexData;

	while (s >> vertexData) {
		std::istringstream vertexStream(vertexData);
		unsigned int vIndex, tIndex, nIndex;
		char slash;
		
		vertexStream >> vIndex >> slash >> tIndex >> slash >> nIndex;
		vIndices.push_back(vIndex - 1);
		tIndices.push_back(tIndex - 1); // Store texture index
		nIndices.push_back(nIndex - 1); // Store normal index
	}

	size_t count = vIndices.size();
	if (count == 3) {
		// Triangle: directly add the indices
		indices.push_back(vIndices[0]);
		indices.push_back(vIndices[1]);
		indices.push_back(vIndices[2]);

	}
	else if (count == 4) {
		// Quad: split into two triangles
		indices.push_back(vIndices[0]); // First vertex
		indices.push_back(vIndices[1]); // Second vertex
		indices.push_back(vIndices[2]); // Third vertex


		indices.push_back(vIndices[0]); // First vertex
		indices.push_back(vIndices[2]); // Third vertex
		indices.push_back(vIndices[3]); // Fourth vertex
	}
	else if (count > 4) {
		// Polygon: triangulate using fan triangulation
		for (size_t i = 1; i < count - 1; i++) {
			indices.push_back(vIndices[0]);     // First vertex (fixed for fan triangulation)
			indices.push_back(vIndices[i]);     // Current vertex
			indices.push_back(vIndices[i + 1]); // Next vertex
		}
	}
}