#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <vector>
#include <string>

#include "glm/glm/glm.hpp"

#include "shader.h"

class Model
{
public:
	Model();
	~Model();

	bool loadOBJ(const std::string& path);

	std::vector<glm::vec3> getVertices() const { return vertices; }
	// std::vector<glm::vec3> getNormals() const { return normals; }
	std::vector<glm::vec2> getTexCoords() const { return texCoords; }

	void render(Shader shader);

private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> GL_normals;
	std::vector<unsigned int> normal_count;
	// std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> vertexIndices; // Faces

	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	void parseVertex(const std::string& line);
	void parseTexCoord(const std::string& line);
	void parseNormal(const std::string& line);
	void parseFace(const std::string& line);

	void setupBuffers();

	void generateNormals(unsigned int a, unsigned int b, unsigned int c, std::vector<unsigned int>& vIndices);
};

#endif

