#include "Skeleton.h"

bool Skeleton::load(const char* file) {
	Tokenizer token;
	if (token.Open(file)) {
		token.FindToken("balljoint");
		// Parse tree
		root = new Joint;
		root->load(token, joints);
		// Finish
		token.Close();
		return true;
	}
	return false;
}

void Skeleton::update()
{
	root->update(glm::mat4(1.0f));
}

void Skeleton::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	root->draw(viewProjMtx, shader);
}

void Skeleton::print()
{
	printf("Joint Count: %d\n", joints.size());
	for (Joint* joint : joints) {
		joint->print();
	}
}

glm::mat4 Skeleton::getWorldMatrix(int jointIndex) {
	return joints[jointIndex]->getWorldMatrix();
}
