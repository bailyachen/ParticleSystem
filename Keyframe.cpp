#include "Keyframe.h"

Keyframe::Keyframe(float time, float value, TangentType rule_in, TangentType rule_out) {
	this->time = time;
	this->value = value;
	this->rule_in = rule_in;
	this->rule_out = rule_out;
	tangent_in = 0;
	tangent_out = 0;
	curve = glm::vec4(0);
}

void Keyframe::compute(Keyframe* prev, Keyframe* next) {
	switch (rule_in) {
	case TangentType::flat: tangent_in = 0;
		break;
	case TangentType::smooth:
		if (prev == nullptr && next != nullptr) {
			tangent_in = (next->value - value) / (next->time - time);
		}
		else if (prev != nullptr && next == nullptr) {
			tangent_in = (value - prev->value) / (time - prev->time);
		}
		else if (prev == nullptr && next == nullptr) {
			tangent_in == 0;
		}
		else tangent_in = (prev->value - next->value) / (prev->time - next->time);
		break;
	case TangentType::linear:
		if (prev == nullptr) {
			tangent_in = value / time;
		}
		else tangent_in = (value - prev->value) / (time - prev->time);
		break;
	}
	switch (rule_out) {
	case TangentType::flat: tangent_out = 0;
		break;
	case TangentType::smooth:
		if (prev == nullptr && next != nullptr) {
			tangent_out = (next->value - value) / (next->time - time);
		}
		else if (prev != nullptr && next == nullptr) {
			tangent_out = (value - prev->value) / (time - prev->time);
		}
		else if (prev == nullptr && next == nullptr) {
			tangent_out == 0;
		}
		else tangent_out = (prev->value - next->value) / (prev->time - next->time);
		break;
	case TangentType::linear:
		if (next == nullptr) {
			tangent_out = value / time;
		}
		else tangent_out = (next->value - value) / (next->time - time);
		break;
	}
}

void Keyframe::computeCurve(Keyframe* next) {
	if (next != nullptr) {
		glm::mat4 B = glm::mat4(0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 1, 1, 1, 1, 0, 0);
		glm::vec4 g = glm::vec4(value, next->value, (time - next->time) * tangent_out, (time - next->time) * next->tangent_in);
		curve = glm::inverse(B) * g;
	}
	else curve = glm::vec4(0);
}
