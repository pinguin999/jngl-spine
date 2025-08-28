#pragma once

#include <jngl.hpp>

#include <spine/spine.h>

class SkeletonDrawable : public jngl::Drawable {
public:
	spine::Skeleton* skeleton;
	spine::AnimationState* state;
	float timeScale;

	explicit SkeletonDrawable (spine::SkeletonData* skeleton, spine::AnimationStateData* stateData = 0);
	~SkeletonDrawable () override;

	void step() override;

	void draw() const override;

	void setUsePremultipliedAlpha(bool usePMA) { usePremultipliedAlpha = usePMA; };
	bool getUsePremultipliedAlpha() { return usePremultipliedAlpha; };
private:
	bool ownsAnimationStateData;
	float* worldVertices;
	// spine::FloatArray* tempUvs;
	// spine::ColorArray* tempColors;
	spine::SkeletonClipping* clipper;
	bool usePremultipliedAlpha;
};

class TextureLoader : public spine::TextureLoader {
public:
	void load(spine::AtlasPage& page, const spine::String& path) override;

	void unload(void* texture) override;
};
