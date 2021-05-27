#ifndef _SKELETON_H_
#define _SKELETON_H_
#include "core.h"
#include "Joint.h"
#include "Tokenizer.h"
class Skeleton {
private:
public:
	Joint* root;
	std::vector<Joint*> joints;
	bool load(const char* file);
	void update();
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	glm::mat4 getWorldMatrix(int jointIndex);
	Skeleton() {
		root = new Joint;
	};
	void print();

};
#endif