#include "Animator.h"

Animator::Animator(Skeleton* skeleton, Animation* animation) {
	this->skeleton = skeleton;
	this->animation = animation;
}

void Animator::play(float time) {
	std::vector<glm::vec3> pose = animation->getPose(time);
	if (pose.size() == 0) {
		printf("Ending Playback\n");
		return;
	}
	if (pose.size() != skeleton->joints.size() + 1) {
		printf("Joint Count: %d Channel Count: %d\n", skeleton->joints.size(), pose.size());
		return;
	}
	skeleton->root->SetOffset(pose[0]);
	//printf("X: %f, Y: %f, Z: %f\n", pose[10].x, pose[10].y, pose[10].z);
	for (unsigned int i = 0; i < skeleton->joints.size(); i++) {
		skeleton->joints[i]->SetPose(pose[i + 1]);
	}

}
