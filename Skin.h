#include "core.h"
#include "Tokenizer.h"
#include "Skeleton.h"

#ifndef _SKIN_H_
#define _SKIN_H_
class Skin {
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::vec3 diffuseColor;
	glm::vec3 ambientColor;
	Skeleton* skeleton;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<std::vector<glm::vec2>> skinWeights;
	std::vector<unsigned int> triangles;
	std::vector<glm::mat4> bindings;
	std::vector<glm::mat4> skinningMatrices;

public:
	~Skin();
	bool load(const char* file, Skeleton* skeleton);
	void update();
	void draw(const glm::mat4& viewProjMtx, GLuint shader, bool ambient);
	void print();
};
#endif