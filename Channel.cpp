#include "Channel.h"

Channel::Channel() {
	extrap_in = ExtrapolationType::constant;
	extrap_out = ExtrapolationType::constant;
}

void Channel::addKeyframe(Keyframe* keyframe) {
	keyframes.push_back(keyframe);
}

void Channel::precompute() {
	if (keyframes.size() == 0) return;
	else if (keyframes.size() == 1) {
		keyframes[0]->compute(nullptr, nullptr);
		return;
	}
	for (unsigned int i = 0; i < keyframes.size(); i++) {
		if (i == 0) {
			keyframes[i]->compute(nullptr, keyframes[i + 1]);
		}
		else if (i = keyframes.size() - 1) {
			keyframes[i]->compute(keyframes[i - 1], nullptr);
		}
		else {
			keyframes[i]->compute(keyframes[i - 1], keyframes[i + 1]);
		}
	}
	for (unsigned int i = 0; i < keyframes.size() - 1; i++) {
		keyframes[i]->computeCurve(keyframes[i + 1]);
	}
}

float Channel::evaluate(float time) {
	if (keyframes.size() == 0) return 0;
	else if (keyframes.size() == 1) return keyframes.front()->value;
	else if (time < keyframes.front()->time) {
		float valueSpan = keyframes.back()->value - keyframes.front()->value;
		float span = keyframes.back()->time - keyframes.front()->time;
		int count = 0;
		// Before the first key
		switch (extrap_in) {
		case ExtrapolationType::linear:
			return keyframes.front()->tangent_out * time;
		case ExtrapolationType::constant:
			return keyframes.front()->value;
		case ExtrapolationType::cycle:
			while (time < keyframes.front()->time) time += span;
			return evaluateCubic(time);
		case ExtrapolationType::cycle_offset:
			while (time < keyframes.front()->time) {
				time += span;
				count++;
			}
			return -count * valueSpan + evaluateCubic(time);
		case ExtrapolationType::bounce:
			while (time < keyframes.front()->time) {
				time += span;
				count++;
			}
			if (count % 2 == 1) return -evaluateCubic(time);
			else return evaluateCubic(time);
		}
	}
	else if (time > keyframes.back()->time) {
		float valueSpan = keyframes.back()->value - keyframes.front()->value;
		float span = keyframes.back()->time - keyframes.front()->time;
		int count = 0;
		// After the last key
		switch (extrap_out) {
		case ExtrapolationType::linear:
			return keyframes.back()->tangent_in * time;
		case ExtrapolationType::constant:
			return keyframes.back()->value;
		case ExtrapolationType::cycle:
			while (time > keyframes.back()->time) time -= span;
			return evaluateCubic(time);
		case ExtrapolationType::cycle_offset:
			while (time > keyframes.back()->time) {
				time -= span;
				count++;
			}
			return count * valueSpan + evaluateCubic(time);
		case ExtrapolationType::bounce:
			while (time > keyframes.back()->time) {
				time -= span;
				count++;
			}
			if (count % 2 == 1) return -evaluateCubic(time);
			else return evaluateCubic(time);
		}
	}
	else return evaluateCubic(time);
}

float Channel::evaluateCubic(float time) {
	unsigned int prev = 0;
	unsigned int next = keyframes.size() - 1;
	// Find Span
	for (unsigned int i = 0; i < keyframes.size(); i++) {
		if (time > keyframes[i]->time) prev = i;
		else if (time == keyframes[i]->time) return keyframes[i]->value;
		else if (time < keyframes[i]->time) {
			next = i;
			float u = (time - keyframes[prev]->time) / (keyframes[next]->time - keyframes[prev]->time);
			// d + u(c + u(b + u(a)))
			return (((((u * keyframes[prev]->curve[0]) + keyframes[prev]->curve[1]) * u) + keyframes[prev]->curve[2]) * u) + keyframes[prev]->curve[3];
		}
	}
}
