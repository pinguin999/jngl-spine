#pragma once

#define SPINE_SHORT_NAMES
#include <spine/spine.h>
#include <spine/extension.h>

_SP_ARRAY_DECLARE_TYPE(spColorArray, spColor)

namespace spine {

class SkeletonDrawable{  //: public sf::Drawable {
public:
	Skeleton* skeleton;
	AnimationState* state;
	float timeScale;
	// sf::VertexArray* vertexArray;
	spVertexEffect* vertexEffect;

	SkeletonDrawable (SkeletonData* skeleton, AnimationStateData* stateData = 0);
	~SkeletonDrawable ();

	void update (float deltaTime);

	// virtual void draw (sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void draw (int target, int states) const;

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