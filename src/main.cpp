#include <jngl.hpp>

#include <iostream>
#include <string.h>
#include "spineJNGLRenderer.hpp"

#include <stdio.h>
#include <stdlib.h>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

spine::SkeletonData* readSkeletonJsonData (const char* filename, spine::Atlas* atlas, float scale) {
	spine::SkeletonJson* json = new spine::SkeletonJson(atlas);
	json->setScale(scale);
	spine::SkeletonData* skeletonData = json->readSkeletonDataFile(filename);
	if (!skeletonData) {
		printf("%s\n", json->getError().buffer());
		exit(1);
	}
	delete json;
	return skeletonData;
}

spine::SkeletonData* readSkeletonBinaryData (const char* filename, spine::Atlas* atlas, float scale) {
	spine::SkeletonBinary* binary = new spine::SkeletonBinary(atlas);
	binary->setScale(scale);
	spine::SkeletonData* skeletonData = binary->readSkeletonDataFile(filename);
	if (!skeletonData) {
		printf("%s\n", binary->getError().buffer());
		exit(1);
	}
	delete binary;
	return skeletonData;
}

void testcase (void func(spine::SkeletonData* skeletonData, spine::Atlas* atlas),
		const char* jsonName, const char* binaryName, const char* atlasName,
		float scale) {
	jngl::cancelQuit();
	jngl::showWindow("Spine JNGL - vine", 640, 640, false, {1,1}, {1,1});
	jngl::setBackgroundColor(0x000000_rgb);
	spine::Atlas* atlas = new spine::Atlas(atlasName, nullptr);

	spine::SkeletonData* skeletonData = readSkeletonJsonData(jsonName, atlas, scale);
	func(skeletonData, atlas);
	delete skeletonData;

	jngl::cancelQuit();
	skeletonData = readSkeletonBinaryData(binaryName, atlas, scale);
	func(skeletonData, atlas);
	delete skeletonData;

	delete atlas;
}

// void coin (spSkeletonData* skeletonData, spAtlas* atlas) {
// 	SkeletonDrawable* drawable = new SkeletonDrawable(skeletonData);
// 	drawable->timeScale = 1;

// 	spSkeleton* skeleton = drawable->skeleton;
// 	skeleton->x = 320;
// 	skeleton->y = 590;
// 	spSkeleton_updateWorldTransform(skeleton);

// 	spAnimationState_setAnimationByName(drawable->state, 0, "animation", true);

// 	while (jngl::running()) {
// 		jngl::updateInput();

// 		drawable->step();
// 		drawable->draw();

// 		jngl::swapBuffers();
// 	}
// }

// void vine (spSkeletonData* skeletonData, spAtlas* atlas) {
// 	SkeletonDrawable* drawable = new SkeletonDrawable(skeletonData);
// 	drawable->timeScale = 1;

// 	spSkeleton* skeleton = drawable->skeleton;
// 	skeleton->x = 320;
// 	skeleton->y = 590;
// 	spSkeleton_updateWorldTransform(skeleton);

// 	spAnimationState_setAnimationByName(drawable->state, 0, "grow", true);

// 	jngl::setTitle("Spine JNGL - vine");
// 	while (jngl::running()) {
// 		jngl::updateInput();

// 		drawable->step();
// 		drawable->draw();

// 		jngl::swapBuffers();
// 	}
// }

// void tank (spSkeletonData* skeletonData, spAtlas* atlas) {
// 	SkeletonDrawable* drawable = new SkeletonDrawable(skeletonData);
// 	drawable->timeScale = 1;

// 	spSkeleton* skeleton = drawable->skeleton;
// 	skeleton->x = 500;
// 	skeleton->y = 590;
// 	spSkeleton_updateWorldTransform(skeleton);

// 	spAnimationState_setAnimationByName(drawable->state, 0, "drive", true);

// 	jngl::setTitle("Spine JNGL - tank");

// 	while (jngl::running()) {
// 		jngl::updateInput();
// 		drawable->step();
// 		drawable->draw();
// 		jngl::swapBuffers();
// 	}
// }

void pd (spine::SkeletonData* skeletonData, spine::Atlas* atlas) {
	SkeletonDrawable* drawable = new SkeletonDrawable(skeletonData);
	drawable->timeScale = 1;

	spine::Skeleton* skeleton = drawable->skeleton;
	skeleton->setX(900);
	skeleton->setY(410);
	skeleton->updateWorldTransform(spine::Physics_None);

	drawable->state->setAnimation(0, "idle", true);

	jngl::setTitle("Spine JNGL - Dog");

	while (jngl::running()) {
		jngl::updateInput();
		drawable->step();
		drawable->draw();
		jngl::swapBuffers();
	}
}

// /**
//  * Used for debugging purposes during runtime development
//  */
// void test (spSkeletonData* skeletonData, spAtlas* atlas) {
// 	spSkeleton* skeleton = spSkeleton_create(skeletonData);
// 	spAnimationStateData* animData = spAnimationStateData_create(skeletonData);
// 	spAnimationState* animState = spAnimationState_create(animData);
// 	spAnimationState_setAnimationByName(animState, 0, "drive", true);


// 	float d = 3;
// 	for (int i = 0; i < 1; i++) {
// 		spAnimationState_update(animState, d);
// 		spAnimationState_apply(animState, skeleton);
// 		spSkeleton_updateWorldTransform(skeleton);
// 		for (int ii = 0; ii < skeleton->bonesCount; ii++) {
// 			spBone* bone = skeleton->bones[ii];
// 			printf("%s %f %f %f %f %f %f\n", bone->data->name, bone->a, bone->b, bone->c, bone->d, bone->worldX, bone->worldY);
// 		}
// 		printf("========================================\n");
// 		d += 0.1f;
// 	}

// 	spSkeleton_dispose(skeleton);
// }

JNGL_MAIN_BEGIN {
	// testcase(test, "data/tank-pro.json", "data/tank-pro.skel", "data/tank.atlas", 1.0f);
	// testcase(coin, "data/coin-pro.json", "data/coin-pro.skel", "data/coin.atlas", 0.5f);
	// testcase(vine, "data/vine-pro.json", "data/vine-pro.skel", "data/vine.atlas", 0.5f);
	// testcase(tank, "data/tank-pro.json", "data/tank-pro.skel", "data/tank.atlas", 0.2f);
	testcase(pd, "data/dog_01_normal.json", "data/dog_01_normal.skel", "data/dog_01_normal.atlas", 1.1f);
} JNGL_MAIN_END
