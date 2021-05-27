#include "Animation.h"
#include "Animation.h"
#include <iostream>

Animation::Animation() {
	time_start = 0;
	time_end = 0;
}


void Animation::addChannel(Channel* channel) {
	channels.push_back(channel);
}

bool Animation::load(const char* file) {
	Tokenizer token;
	if (token.Open(file)) {
		char temp[256];
		token.FindToken("animation");
		token.FindToken("{");
		token.FindToken("range");
		time_start = token.GetFloat();
		time_end = token.GetFloat();
		token.FindToken("numchannels");
		int numChannels = token.GetInt();
		for (int i = 0; i < numChannels; i++) {
			Channel* channel = new Channel();
			token.FindToken("channel");
			token.FindToken("{");
			token.FindToken("extrapolate");
			token.GetToken(temp);
			if (strcmp(temp, "linear") == 0) channel->extrap_in = Channel::ExtrapolationType::linear;
			else if (strcmp(temp, "constant") == 0) channel->extrap_in = Channel::ExtrapolationType::constant;
			else if (strcmp(temp, "cycle") == 0) channel->extrap_in = Channel::ExtrapolationType::cycle;
			else if (strcmp(temp, "cycle_offset") == 0) channel->extrap_in = Channel::ExtrapolationType::cycle_offset;
			else if (strcmp(temp, "bounce") == 0) channel->extrap_in = Channel::ExtrapolationType::bounce;
			token.GetToken(temp);
			if (strcmp(temp, "linear") == 0) channel->extrap_out = Channel::ExtrapolationType::linear;
			else if (strcmp(temp, "constant") == 0) channel->extrap_out = Channel::ExtrapolationType::constant;
			else if (strcmp(temp, "cycle") == 0) channel->extrap_out = Channel::ExtrapolationType::cycle;
			else if (strcmp(temp, "cycle_offset") == 0) channel->extrap_out = Channel::ExtrapolationType::cycle_offset;
			else if (strcmp(temp, "bounce") == 0) channel->extrap_out = Channel::ExtrapolationType::bounce;
			token.FindToken("keys");
			int keyCount = token.GetInt();
			token.FindToken("{");
			for (int j = 0; j < keyCount; j++) {
				float time = token.GetFloat();
				float value = token.GetFloat();
				Keyframe::TangentType tan_in = Keyframe::TangentType::linear;
				Keyframe::TangentType tan_out = Keyframe::TangentType::linear;
				token.GetToken(temp);
				if (strcmp(temp, "linear") == 0) tan_in = Keyframe::TangentType::linear;
				else if (strcmp(temp, "flat") == 0) tan_in = Keyframe::TangentType::flat;
				else if (strcmp(temp, "smooth") == 0) tan_in = Keyframe::TangentType::smooth;
				token.GetToken(temp);
				if (strcmp(temp, "linear") == 0) tan_out = Keyframe::TangentType::linear;
				else if (strcmp(temp, "flat") == 0) tan_out = Keyframe::TangentType::flat;
				else if (strcmp(temp, "smooth") == 0) tan_out = Keyframe::TangentType::smooth;
				Keyframe* keyframe = new Keyframe(time, value, tan_in, tan_out);
				channel->addKeyframe(keyframe);
			}
			token.FindToken("}");
			token.FindToken("}");
			addChannel(channel);
		}
		token.FindToken("}");
		token.Close();
		for(Channel * channel : channels) {
			channel->precompute();
		}
		return true;
	}
	return false;
}

void Animation::print()
{
	std::cout << "Animation" << std::endl;
	std::cout << "Range: " << time_start << " " << time_end << std::endl;
	std::cout << "Channel Count: " << channels.size() << std::endl;
	for (Channel* channel : channels) {
		std::cout << "Channel" << std::endl;
		std::cout << "Extrapolate: ";
		switch (channel->extrap_in) {
		case Channel::ExtrapolationType::linear: std::cout << "linear ";
			break;
			case Channel::ExtrapolationType::constant: std::cout << "constant ";
			break;
			case Channel::ExtrapolationType::cycle: std::cout << "cycle ";
			break;
			case Channel::ExtrapolationType::cycle_offset: std::cout << "cycle_offset ";
			break;
			case Channel::ExtrapolationType::bounce: std::cout << "bounce ";
			break;
		}
		switch (channel->extrap_out) {
		case Channel::ExtrapolationType::linear: std::cout << "linear" << std::endl;
			break;
		case Channel::ExtrapolationType::constant: std::cout << "constant" << std::endl;
			break;
		case Channel::ExtrapolationType::cycle: std::cout << "cycle" << std::endl;
			break;
		case Channel::ExtrapolationType::cycle_offset: std::cout << "cycle_offset" << std::endl;
			break;
		case Channel::ExtrapolationType::bounce: std::cout << "bounce" << std::endl;
			break;
		}
		std::cout << "Keyframe Count: " << channel->keyframes.size() << std::endl;
		for (Keyframe* keyframe : channel->keyframes) {
			std::cout << "Keyframe: " << keyframe->time << " " << keyframe->value << " ";
			switch (keyframe->rule_in) {
			case Keyframe::TangentType::flat: std::cout << "flat ";
				break;
			case Keyframe::TangentType::smooth: std::cout << "smooth ";
				break;
			case Keyframe::TangentType::linear: std::cout << "linear ";
				break;
			}
			switch (keyframe->rule_out) {
			case Keyframe::TangentType::flat: std::cout << "flat" << std::endl;
				break;
			case Keyframe::TangentType::smooth: std::cout << "smooth" << std::endl;
				break;
			case Keyframe::TangentType::linear: std::cout << "linear" << std::endl;
				break;
			}
		}

	}
}

std::vector<glm::vec3> Animation::getPose(float time) {
	std::vector<glm::vec3> pose;
	if (channels.size() % 3 != 0) {
		printf("Channels are not of the correct size\n");
		return pose;
	}
	for (unsigned int i = 0; i < channels.size(); i += 3) {
		glm::vec3 vector = glm::vec3(channels[i]->evaluate(time), channels[i + 1]->evaluate(time), channels[i + 2]->evaluate(time));
		pose.push_back(vector);
	}
	return pose;
}
