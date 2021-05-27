#include "core.h"
#include "Skeleton.h"
#include "Animation.h"

#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_
class Animator {
private:
	Skeleton* skeleton;
	Animation* animation;
public:
	Animator(Skeleton* skeleton, Animation* animation);
	void play(float time);
};
#endif