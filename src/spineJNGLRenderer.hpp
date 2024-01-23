#pragma once

#include <jngl.hpp>

#include <spine/spine.h>
#include <spine/extension.h>

_SP_ARRAY_DECLARE_TYPE(spColorArray, spColor)

namespace spine {

class SkeletonDrawable : public jngl::Drawable {
public:
	spSkeleton* skeleton;
	spAnimationState* state;
	float timeScale;

	explicit SkeletonDrawable (spSkeletonData* skeleton, spAnimationStateData* stateData = 0);
	~SkeletonDrawable () override;

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
