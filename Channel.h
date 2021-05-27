#ifndef _CHANNEL_H_
#define _CHANNEL_H_
#include "core.h"
#include "Keyframe.h"
class Channel {
private:
public:
	enum class ExtrapolationType {
		linear, constant, cycle, cycle_offset, bounce
	};
	ExtrapolationType extrap_in;
	ExtrapolationType extrap_out;
	std::vector<Keyframe*> keyframes;
	Channel();
	void addKeyframe(Keyframe* keyframe);
	void precompute();
	float evaluate(float time);
	float evaluateCubic(float time);
};
#endif
