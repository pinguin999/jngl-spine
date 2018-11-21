#pragma once

#include <jngl.hpp>

#define SPINE_SHORT_NAMES
#include <spine/spine.h>
#include <spine/extension.h>

_SP_ARRAY_DECLARE_TYPE(spColorArray, spColor)

namespace spine {

class SkeletonDrawable : public jngl::Drawable {
public:
	Skeleton* skeleton;
	AnimationState* state;
	float timeScale;
	// sf::VertexArray* vertexArray;
	spVertexEffect* vertexEffect;

	SkeletonDrawable (SkeletonData* skeleton, AnimationStateData* stateData = 0);
	~SkeletonDrawable ();

	void step() override;

	void draw() const override;

	void setUsePremultipliedAlpha(bool usePMA) { usePremultipliedAlpha = usePMA; };
	bool getUsePremultipliedAlpha() { return usePremultipliedAlpha; };
private:
	bool ownsAnimationStateData;
	float* worldVertices;
	spFloatArray* tempUvs;
	spColorArray* tempColors;
	spSkeletonClipping* clipper;
	bool usePremultipliedAlpha;
};

}
