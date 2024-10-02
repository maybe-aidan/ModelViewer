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
	vertices.clear();
	normals.clear();
	texCoords.clear();
	GL_normals.clear();
	vertexIndices.clear();

	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "ERROR::MODEL::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		return false;
	}

	std::string line;
	// Parsing vertex, texture(uv), normal, and face data
	// As of right now, textures are not used, but I still parse them for future use.
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

	/* Debug *\
	std::cout << "Vertex Buffer Size: " << vertices.size() << std::endl;
	std::cout << "Normal Buffer Size: " << normals.size() << std::endl;
	std::cout << "GL_Normal Buffer Size: " << GL_normals.size() << std::endl;
	*/

	return true;
}

void Model::render(Shader shader) {
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	shader.use();

	glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_INT, 0);

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

	// Loading relevant data into the correct position per vertex.
 
	
	// Texture coordinates are probably broken right now, but I haven't test them yet so I can't say for sure.
	// Probably needs the same treament as the normals.
	if (!texCoords.empty()) {
		GLuint texVbo;
		glGenBuffers(1, &texVbo);
		glBindBuffer(GL_ARRAY_BUFFER, texVbo);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
	}
	if (!GL_normals.empty()) {
		GLuint normalVbo;
		glGenBuffers(1, &normalVbo);
		glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
		glBufferData(GL_ARRAY_BUFFER, GL_normals.size() * sizeof(glm::vec3), GL_normals.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
	}

	// Load the index buffer into the EBO.
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), vertexIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

// OBJ File Vertex format:
// v xCoord yCoord zCoord 
// Vertices can have optional extra paramters, but for now I only use x, y, z
void Model::parseVertex(const std::string& line) {
	std::istringstream s(line.substr(2));
	glm::vec3 vertex;
	s >> vertex.x >> vertex.y >> vertex.z;
	vertices.push_back(vertex);
}

// OBJ File Texture format:
// vt uCoord vCoord
// Similar to Vertices, texture coordinates can have extra parameters.
void Model::parseTexCoord(const std::string& line) {
	std::istringstream s(line.substr(3));
	glm::vec2 texCoord;
	s >> texCoord.x >> texCoord.y;
	texCoords.push_back(texCoord);
}

// OBJ File Normal format:
// vn normalX normalY normalZ
// Normals represent vectors orthogonal (perpendicular) to a surface point, usually a vertex.
void Model::parseNormal(const std::string& line) {
	std::istringstream s(line.substr(3));
	glm::vec3 normal;
	s >> normal.x >> normal.y >> normal.z;
	normals.push_back(normal);
}

// OBJ File Face format:
// f vertexIndex1/textureIndex1/normalIndex1 ... vertexIndexN/textureIndexN/normalIndexN
// faces can omit texture parameter, leaving the following format:
// f vertexIndex1//normalIndex1 ...
void Model::parseFace(const std::string& line) {
	std::stringstream s(line.substr(2));
	std::vector<unsigned int> vIndices;
	std::vector<unsigned int> tIndices; 
	std::vector<unsigned int> nIndices;
	std::string vertexData;

	// Get each vertex of a face.
	while (s >> vertexData) {
		std::istringstream vertexStream(vertexData);
		unsigned int vIndex, tIndex, nIndex;
		char slash;
		
		vertexStream >> vIndex; 
		if (vertexStream.peek() == '/') {
			vertexStream >> slash;
			if (vertexStream.peek() == '/') {
				vertexStream >> slash >> nIndex;
				tIndex = 0;
			}
			else {
				vertexStream >> tIndex >> slash >> nIndex;
			}
		}
		else {
			tIndex = 0;
			nIndex = 0;
		}
		
		vIndices.push_back(vIndex - 1);
		if(tIndex != 0) tIndices.push_back(tIndex - 1); // Store texture index
		/* Not a fan of how this has been working, decided to calculate my own normals.
		if (nIndex != 0) {
			nIndices.push_back(nIndex - 1); // Store normal index

			// If the current vertex index does not match the normal index
			if (vIndex != nIndex) {
				// Resize the GL_normals buffer accordingly.
				if (GL_normals.size() < vIndex) {
					GL_normals.resize(vIndex);
				}
				// Set the normal vector at the vertex index to be the correct normal
				GL_normals.at(vIndex - 1) =  normals.at(nIndex-1);
			}
			// This method works.. somewhat
			// Unfortunately some faces in obj file use multiple normals for the same vetex on different faces.
			// So this method almost certainly overwrites normals on some faces, causing strange patches of incorrect lighting.
			// Overall though, this method works MUCH better than the previous method of not rearranging normal buffer, essentially getting random surface normals.
		}
		*/
	}

	size_t count = vIndices.size();
	// Split the face into OpenGL readable triangles.
	if (count == 3) {
		// Triangle: directly add the indices
		vertexIndices.push_back(vIndices[0]);
		vertexIndices.push_back(vIndices[1]);
		vertexIndices.push_back(vIndices[2]);
		generateNormals(0, 1, 2, vIndices);
	}
	else if (count == 4) {
		// Quad: split into two triangles
		vertexIndices.push_back(vIndices[0]);
		vertexIndices.push_back(vIndices[1]);
		vertexIndices.push_back(vIndices[2]);
		generateNormals(0, 1, 2, vIndices);


		vertexIndices.push_back(vIndices[0]);
		vertexIndices.push_back(vIndices[2]);
		vertexIndices.push_back(vIndices[3]);
		generateNormals(0, 2, 3, vIndices);
	}
	else if (count > 4) {
		// Polygon: triangulate using fan triangulation
		for (size_t i = 1; i < count - 1; i++) {
			vertexIndices.push_back(vIndices[0]);
			vertexIndices.push_back(vIndices[i]);
			vertexIndices.push_back(vIndices[i+1]);
			generateNormals(0, i, i+1, vIndices);
		}
	}
}

// Takes more time for intial model load, but is considerably more reliable than the loading of normals from the file 
void Model::generateNormals(unsigned int aIndex, unsigned int bIndex, unsigned int cIndex, std::vector<unsigned int>& vIndices) {
	// Calculating for the normal at vertex A, we need to take the vectors of A to B and A to C. Thus, n_A = normalize(cross(B - A, C - A));
	unsigned int a = vIndices[aIndex];
	unsigned int b = vIndices[bIndex];
	unsigned int c = vIndices[cIndex];

	glm::vec3 nA = glm::normalize(glm::cross(vertices[b] - vertices[a], vertices[c] - vertices[a]));
	glm::vec3 nB = glm::normalize(glm::cross(vertices[c] - vertices[b], vertices[a] - vertices[b]));
	glm::vec3 nC = glm::normalize(glm::cross(vertices[a] - vertices[c], vertices[b] - vertices[c]));

	unsigned int maxIndex = std::max({ a, b, c });
	if (GL_normals.size() <= maxIndex) {
		GL_normals.resize(maxIndex + 1); // Ensure GL_normals can hold the largest index
	}
	GL_normals[a] = nA;
	GL_normals[b] = nB;
	GL_normals[c] = nC;
}