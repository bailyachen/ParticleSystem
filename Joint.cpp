#include "Joint.h"
#include "Cube.h"

void Joint::update(glm::mat4& parent) {
	// Compute local matrix L
	glm::mat4 T = glm::translate(glm::mat4(1.0f), Offset);
	glm::mat4 zR = glm::rotate(z->GetValue(), glm::vec3(0, 0, 1));
	glm::mat4 yR = glm::rotate(y->GetValue(), glm::vec3(0, 1, 0));
	glm::mat4 xR = glm::rotate(x->GetValue(), glm::vec3(1, 0, 0));
	L = T * zR * yR * xR;
	// Compute world matrix W
	W = parent * L;
	
	// Recursively call Update() on children
	for(Joint * jnt : children) {
		jnt->update(W);
	}
}
void Joint::draw(const glm::mat4& viewProjMtx, GLuint shader) {
	// Draw oriented box with OpenGL
	Cube cube = Cube(boxMin, boxMax);
	cube.draw(viewProjMtx * W, shader);

	// Recursively call Draw() on children
	for (Joint* jnt : children) {
		jnt->draw(viewProjMtx, shader);
	}
}

void Joint::print()
{
	printf("Joint: %s\n", name.c_str());
	printMatrix();
	/*for (Joint* child : children) {
		child->print();
	}*/
}

void Joint::printMatrix() {
	printf("Joint Matrix:\n");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", L[i][j]);
		}
		printf("\n");
	}
}

bool Joint::load(Tokenizer& token, std::vector<Joint*>& joints) {
	joints.push_back(this);
	token.SkipWhitespace();
	while (token.CheckChar() != '{') {
		name += token.GetChar();
	}
	token.FindToken("{");
	while (1) {
		char temp[256];
		token.GetToken(temp);
		if (strcmp(temp, "offset") == 0) {
			Offset.x = token.GetFloat();
			Offset.y = token.GetFloat();
			Offset.z = token.GetFloat();
		}
		//else // Check for other tokens
		else if (strcmp(temp, "boxmin") == 0) {
			boxMin.x = token.GetFloat();
			boxMin.y = token.GetFloat();
			boxMin.z = token.GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0) {
			boxMax.x = token.GetFloat();
			boxMax.y = token.GetFloat();
			boxMax.z = token.GetFloat();
		}
		else if (strcmp(temp, "rotxlimit") == 0) {
			float minX = token.GetFloat();
			float maxX = token.GetFloat();
			x->SetMinMax(minX, maxX);
		}
		else if (strcmp(temp, "rotylimit") == 0) {
			float minY = token.GetFloat();
			float maxY = token.GetFloat();
			y->SetMinMax(minY, maxY);
		}
		else if (strcmp(temp, "rotzlimit") == 0) {
			float minZ = token.GetFloat();
			float maxZ = token.GetFloat();
			z->SetMinMax(minZ, maxZ);
		}
		else if (strcmp(temp, "pose") == 0) {
			float xVal = token.GetFloat();
			float yVal = token.GetFloat();
			float zVal = token.GetFloat();
			x->SetValue(xVal);
			y->SetValue(yVal);
			z->SetValue(zVal);
		}
		else if (strcmp(temp, "balljoint") == 0) {
			Joint* jnt = new Joint();
			jnt->load(token, joints);
			addchild(*jnt);
		}
		else if (strcmp(temp, "}") == 0) return true;
		else token.SkipLine(); // Unrecognized token
	}
}

glm::mat4 Joint::getWorldMatrix() {
	return W;
}

void Joint::addchild(Joint& jnt)
{
	children.push_back(&jnt);
}
