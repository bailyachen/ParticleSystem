#ifndef _ANIMATION_H_
#define _ANIMATION_H_
#include "core.h"
#include "Keyframe.h"
#include "Channel.h"
#include "Tokenizer.h"
class Animation {
private:
public:
	float time_start;
	float time_end;
	std::vector<Channel*> channels;
	Animation();
	void addChannel(Channel* channel);
	bool load(const char* file);
	void print();
	std::vector<glm::vec3> getPose(float time);
};

#endif
