#include "spineJNGLRenderer.hpp"
#include <jngl.hpp>


#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000
#endif

bool operator==(const spColor& lhs, const spColor& rhs)
{
    return lhs.a == rhs.a && lhs.r == rhs.r &&lhs.g == rhs.g &&lhs.b == rhs.b;
}

_SP_ARRAY_IMPLEMENT_TYPE(spColorArray, spColor)

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path){
	jngl::Sprite* texture = new jngl::Sprite(path);
	// if (!texture->loadFromFile(path)) return;

	// TODO
	// if (self->magFilter == SP_ATLAS_LINEAR) texture->setSmooth(true);
	// if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT) texture->setRepeated(true);

	self->rendererObject = texture;
	// Vector2u size = texture->getSize();
	self->width = texture->getWidth();
	self->height = texture->getHeight();
}

void _spAtlasPage_disposeTexture (spAtlasPage* self){
	delete (jngl::Sprite*)self->rendererObject;
}

char* _spUtil_readFile (const char* path, int* length){
	const auto str = jngl::readAsset(path).str();
	if (length) {
		*length = static_cast<int>(str.length());
	}
	char* buf = static_cast<char*>(malloc(str.length() + 1));
	std::copy(str.begin(), str.end(), buf);
	buf[str.length()] = '\0';
	return buf;
}


namespace spine {

SkeletonDrawable::SkeletonDrawable (spSkeletonData* skeletonData, spAnimationStateData* stateData) :
		timeScale(1),
		worldVertices(0), clipper(0) {
	spBone_setYDown(true);
	worldVertices = MALLOC(float, SPINE_MESH_VERTEX_COUNT_MAX);
	skeleton = spSkeleton_create(skeletonData);
	tempUvs = spFloatArray_create(16);
	tempColors = spColorArray_create(16);

	ownsAnimationStateData = stateData == 0;
	if (ownsAnimationStateData) stateData = spAnimationStateData_create(skeletonData);

	state = spAnimationState_create(stateData);

	clipper = spSkeletonClipping_create();
}

SkeletonDrawable::~SkeletonDrawable () {
	// delete vertexArray;
	FREE(worldVertices);
	if (ownsAnimationStateData) spAnimationStateData_dispose(state->data);
	spAnimationState_dispose(state);
	spSkeleton_dispose(skeleton);
	spSkeletonClipping_dispose(clipper);
	spFloatArray_dispose(tempUvs);
	spColorArray_dispose(tempColors);
}

void SkeletonDrawable::step() {
	const float deltaTime = 1.f / float(jngl::getStepsPerSecond());
	spAnimationState_update(state, deltaTime * timeScale);
	spAnimationState_apply(state, skeleton);
	spSkeleton_updateWorldTransform(skeleton);
}

void SkeletonDrawable::draw() const {
	// vertexArray->clear();
	// states.texture = 0;
	unsigned short quadIndices[6] = { 0, 1, 2, 2, 3, 0 };

	jngl::Sprite* texture = 0;
	for (int i = 0; i < skeleton->slotsCount; ++i) {
		spSlot* slot = skeleton->drawOrder[i];
		spAttachment* attachment = slot->attachment;
		if (!attachment) continue;

		float* vertices = worldVertices;
		int verticesCount = 0;
		float* uvs = 0;
		unsigned short* indices = 0;
		int indicesCount = 0;
		spColor* attachmentColor;

		if (attachment->type == SP_ATTACHMENT_REGION) {
			spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;
			spRegionAttachment_computeWorldVertices(regionAttachment, slot, vertices, 0, 2);
			verticesCount = 4;
			uvs = regionAttachment->uvs;
			indices = quadIndices;
			indicesCount = 6;
			texture = (jngl::Sprite*)((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
			attachmentColor = &regionAttachment->color;

		} else if (attachment->type == SP_ATTACHMENT_MESH) {
			spMeshAttachment* mesh = (spMeshAttachment*)attachment;
			if (mesh->super.worldVerticesLength > SPINE_MESH_VERTEX_COUNT_MAX) continue;
			texture = (jngl::Sprite*)((spAtlasRegion*)mesh->rendererObject)->page->rendererObject;
			spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, worldVertices, 0, 2);
			verticesCount = mesh->super.worldVerticesLength >> 1;
			uvs = mesh->uvs;
			indices = mesh->triangles;
			indicesCount = mesh->trianglesCount;
			attachmentColor = &mesh->color;
		} else if (attachment->type == SP_ATTACHMENT_CLIPPING) {
			spClippingAttachment* clip = (spClippingAttachment*)slot->attachment;
			spSkeletonClipping_clipStart(clipper, slot, clip);
			continue;
		} else continue;

		 const auto r =
		 	static_cast<uint8_t>(skeleton->color.r * slot->color.r * attachmentColor->r * 255);
		 const auto g =
		 	static_cast<uint8_t>(skeleton->color.g * slot->color.g * attachmentColor->g * 255);
		 const auto b =
		 	static_cast<uint8_t>(skeleton->color.b * slot->color.b * attachmentColor->b * 255);
		 const auto a =
		 	static_cast<uint8_t>(skeleton->color.a * slot->color.a * attachmentColor->a * 255);
		// vertex.color.r = r;
		// vertex.color.g = g;
		// vertex.color.b = b;
		// vertex.color.a = a;

		// spColor light;
		// light.r = r / 255.0f;
		// light.g = g / 255.0f;
		// light.b = b / 255.0f;
		// light.a = a / 255.0f;

		// TODO wieder rein
		// sf::BlendMode blend;
		// if (!usePremultipliedAlpha) {
		// 	switch (slot->data->blendMode) {
		// 		case BLEND_MODE_NORMAL:
		// 			blend = normal;
		// 			break;
		// 		case BLEND_MODE_ADDITIVE:
		// 			blend = additive;
		// 			break;
		// 		case BLEND_MODE_MULTIPLY:
		// 			blend = multiply;
		// 			break;
		// 		case BLEND_MODE_SCREEN:
		// 			blend = screen;
		// 			break;
		// 		default:
		// 			blend = normal;
		// 	}
		// } else {
		// 	switch (slot->data->blendMode) {
		// 		case BLEND_MODE_NORMAL:
		// 			blend = normalPma;
		// 			break;
		// 		case BLEND_MODE_ADDITIVE:
		// 			blend = additivePma;
		// 			break;
		// 		case BLEND_MODE_MULTIPLY:
		// 			blend = multiplyPma;
		// 			break;
		// 		case BLEND_MODE_SCREEN:
		// 			blend = screenPma;
		// 			break;
		// 		default:
		// 			blend = normalPma;
		// 	}
		// }

		// if (states.texture == 0) states.texture = texture;

		// if (states.blendMode != blend || states.texture != texture) {
		// target.draw(*vertexArray, states);
		// vertexArray->clear();
		// 	states.blendMode = blend;
		// 	states.texture = texture;
		// }

		// if (spSkeletonClipping_isClipping(clipper)) {
		// 	spSkeletonClipping_clipTriangles(clipper, vertices, verticesCount << 1, indices, indicesCount, uvs, 2);
		// 	vertices = clipper->clippedVertices->items;
		// 	verticesCount = clipper->clippedVertices->size >> 1;
		// 	uvs = clipper->clippedUVs->items;
		// 	indices = clipper->clippedTriangles->items;
		// 	indicesCount = clipper->clippedTriangles->size;
		// }

		// Vector2u size = texture->getSize();

		std::vector<jngl::Vertex> vertexArray;
		for (int i = 0; i < indicesCount; ++i) {
			int index = indices[i] << 1;
			vertexArray.push_back(jngl::Vertex{
				vertices[index],
				vertices[index + 1],
				uvs[index], // * size.x
				uvs[index + 1], // * size.y
			});
		}
		if (r < 250 || g < 250 || b < 250) {
			jngl::setSpriteColor(r, g, b);
		}
		if (texture)
		{
			jngl::setSpriteColor(r, g, b, a);
			texture->drawMesh(vertexArray);
			jngl::setSpriteColor(255, 255, 255, 255);
		}

		spSkeletonClipping_clipEnd(clipper, slot);
	}
	// target.draw(*vertexArray, states);
	spSkeletonClipping_clipEnd2(clipper);

	jngl::setSpriteColor(255, 255, 255, 255);
}

}
