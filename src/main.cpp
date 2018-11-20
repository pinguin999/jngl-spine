#include <jngl.hpp>

#include <iostream>
#include <string.h>
#include "spineJNGLRenderer.hpp"

using namespace std;
using namespace spine;
#include <stdio.h>
#include <stdlib.h>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

void callback (AnimationState* state, EventType type, TrackEntry* entry, Event* event) {
	const char* animationName = (entry && entry->animation) ? entry->animation->name : 0;

	switch (type) {
	case ANIMATION_START:
		printf("%d start: %s\n", entry->trackIndex, animationName);
		break;
	case ANIMATION_INTERRUPT:
		printf("%d interrupt: %s\n", entry->trackIndex, animationName);
		break;
	case ANIMATION_END:
		printf("%d end: %s\n", entry->trackIndex, animationName);
		break;
	case ANIMATION_COMPLETE:
		printf("%d complete: %s\n", entry->trackIndex, animationName);
		break;
	case ANIMATION_DISPOSE:
		printf("%d dispose: %s\n", entry->trackIndex, animationName);
		break;
	case ANIMATION_EVENT:
		printf("%d event: %s, %s: %d, %f, %s\n", entry->trackIndex, animationName, event->data->name, event->intValue, event->floatValue,
				event->stringValue);
		break;
	}
	fflush(stdout);
}

SkeletonData* readSkeletonJsonData (const char* filename, Atlas* atlas, float scale) {
	SkeletonJson* json = SkeletonJson_create(atlas);
	json->scale = scale;
	SkeletonData* skeletonData = SkeletonJson_readSkeletonDataFile(json, filename);
	if (!skeletonData) {
		printf("%s\n", json->error);
		exit(0);
	}
	SkeletonJson_dispose(json);
	return skeletonData;
}

SkeletonData* readSkeletonBinaryData (const char* filename, Atlas* atlas, float scale) {
	SkeletonBinary* binary = SkeletonBinary_create(atlas);
	binary->scale = scale;
	SkeletonData *skeletonData = SkeletonBinary_readSkeletonDataFile(binary, filename);
	if (!skeletonData) {
		printf("%s\n", binary->error);
		exit(0);
	}
	SkeletonBinary_dispose(binary);
	return skeletonData;
}

void testcase (void func(SkeletonData* skeletonData, Atlas* atlas),
		const char* jsonName, const char* binaryName, const char* atlasName,
		float scale) {
	Atlas* atlas = Atlas_createFromFile(atlasName, 0);

	SkeletonData* skeletonData = readSkeletonJsonData(jsonName, atlas, scale);
	func(skeletonData, atlas);
	SkeletonData_dispose(skeletonData);

	skeletonData = readSkeletonBinaryData(binaryName, atlas, scale);
	func(skeletonData, atlas);
	SkeletonData_dispose(skeletonData);

	Atlas_dispose(atlas);
}

void coin (SkeletonData* skeletonData, Atlas* atlas) {
	SkeletonDrawable* drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;

	Skeleton* skeleton = drawable->skeleton;
	skeleton->x = 320;
	skeleton->y = 590;
	Skeleton_updateWorldTransform(skeleton);

	AnimationState_setAnimationByName(drawable->state, 0, "rotate", true);

	// sf::RenderWindow window(sf::VideoMode(640, 640), "Spine SFML - vine");
	// window.setFramerateLimit(60);
	// sf::Event event;
	// sf::Clock deltaClock;
	float swirlTime = 0;
	while (true) {
	// while (window.isOpen()) {
	// 	while (window.pollEvent(event))
	// 		if (event.type == sf::Event::Closed) window.close();

		// float delta = deltaClock.getElapsedTime().asSeconds();
		// deltaClock.restart();
		float delta = 25.0;

		jngl::getTime();
		drawable->update(delta);

		// window.clear();
		// window.draw(*drawable);  //TODO das brauch ich irgendwie noch
		// window.display();
	}
}

/**
 * Used for debugging purposes during runtime development
 */
void test (SkeletonData* skeletonData, Atlas* atlas) {
	spSkeleton* skeleton = Skeleton_create(skeletonData);
	spAnimationStateData* animData = spAnimationStateData_create(skeletonData);
	spAnimationState* animState = spAnimationState_create(animData);
	spAnimationState_setAnimationByName(animState, 0, "drive", true);


	float d = 3;
	for (int i = 0; i < 1; i++) {
		spSkeleton_update(skeleton, d);
		spAnimationState_update(animState, d);
		spAnimationState_apply(animState, skeleton);
		spSkeleton_updateWorldTransform(skeleton);
		for (int ii = 0; ii < skeleton->bonesCount; ii++) {
			spBone* bone = skeleton->bones[ii];
			printf("%s %f %f %f %f %f %f\n", bone->data->name, bone->a, bone->b, bone->c, bone->d, bone->worldX, bone->worldY);
		}
		printf("========================================\n");
		d += 0.1f;
	}

	Skeleton_dispose(skeleton);
}


class Main : public jngl::Work {
public:
	Main() {
		// Aus der main.cpp sfml
		// testcase(test, "data/tank-pro.json", "data/tank-pro.skel", "data/tank.atlas", 1.0f);
		// testcase(coin, "data/coin-pro.json", "data/coin-pro.skel", "data/coin.atlas", 0.5f);


		jngl::setBackgroundColor(0, 0, 0);
	}

	~Main() {
	}

	void step() override {
		const auto mouse = jngl::getMousePos() +
		                   jngl::Vec2(jngl::getScreenWidth() / 2, jngl::getScreenHeight() / 2);
	}

	void draw() const override {
		jngl::translate(-jngl::getScreenWidth() / 2, - jngl::getScreenHeight() / 2);
		jngl::setFontColor(255, 255, 255);
		jngl::print("Hello World from JNGL!", 100, 100);
	}

private:
};

JNGL_MAIN_BEGIN {
	jngl::showWindow("JNGL+Spine", WINDOW_WIDTH, WINDOW_HEIGHT, false);
	jngl::setWork(std::make_shared<Main>());
	jngl::mainLoop();
} JNGL_MAIN_END
