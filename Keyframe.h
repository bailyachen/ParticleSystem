#ifndef _KEYFRAME_H_
#define _KEYFRAME_H_
#include "core.h"

class Keyframe {

private:
public:
	enum class TangentType {
		linear, flat, smooth
	};
	float time;
	float value;
	float tangent_in;
	float tangent_out;
	TangentType rule_in;
	TangentType rule_out;
	glm::vec4 curve;
	Keyframe(float time, float value, TangentType tangent_in, TangentType tangent_out);
	void compute(Keyframe* prev, Keyframe* next);
	void computeCurve(Keyframe* next);
};
#endif
