#include "core.h"
#include "Cube.h"
#include "DOF.h"
#include "Tokenizer.h"
#include <string>
#include "imgui-master/imgui.h"

#ifndef _JOINT_H_
#define _JOINT_H_

class Joint {
private:
	std::string name;
	glm::mat4 L;
	glm::mat4 W;
	glm::vec3 Offset;
	glm::vec3 boxMin;
	glm::vec3 boxMax;
	DOF* x;
	DOF* y;
	DOF* z;
	
public:
	
	std::vector<Joint*> children;
	void update(glm::mat4& parent);
	bool load(Tokenizer& token, std::vector<Joint*>& joints);
	void addchild(Joint& jnt);
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	glm::mat4 getWorldMatrix();
	void print();
	void printMatrix();
	Joint() {
		name = "";
		L = glm::mat4(1.0f);
		W = glm::mat4(1.0f);
		Offset = glm::vec3(0.0f);
		boxMin = glm::vec3(-0.1f);
		boxMax = glm::vec3(0.1f);
		x = new DOF();
		y = new DOF();
		z = new DOF();
	};
	void SetSliders() {
		ImGui::Text("Joint: %s", name.c_str());
		std::string xName = "X_" + name;
		std::string yName = "Y_" + name;
		std::string zName = "Z_" + name;
		ImGui::SliderFloat(xName.c_str(), &x->value, x->GetMin(), x->GetMax());
		ImGui::SliderFloat(yName.c_str(), &y->value, y->GetMin(), y->GetMax());
		ImGui::SliderFloat(zName.c_str(), &z->value, z->GetMin(), z->GetMax());
		for (Joint* child : children) {
			child->SetSliders();
		}
	}
	void SetPose(glm::vec3 d) {
		x->SetValue(d.x);
		y->SetValue(d.y);
		z->SetValue(d.z);
	}
	void SetOffset(glm::vec3 offset) {
		Offset = offset;
	}
};
#endif
