#include "spineJNGLRenderer.hpp"
#include <jngl.hpp>


#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000
#endif

_SP_ARRAY_IMPLEMENT_TYPE(spColorArray, spColor)

void _AtlasPage_createTexture (AtlasPage* self, const char* path){
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

void _AtlasPage_disposeTexture (AtlasPage* self){
	delete (jngl::Sprite*)self->rendererObject;
}

char* _Util_readFile (const char* path, int* length){
	return _spReadFile(path, length);
}


namespace spine {

SkeletonDrawable::SkeletonDrawable (SkeletonData* skeletonData, AnimationStateData* stateData) :
		timeScale(1),
		// vertexArray(new VertexArray(Triangles, skeletonData->bonesCount * 4)),
		vertexEffect(0),
		worldVertices(0), clipper(0) {
	Bone_setYDown(true);
	worldVertices = MALLOC(float, SPINE_MESH_VERTEX_COUNT_MAX);
	skeleton = Skeleton_create(skeletonData);
	tempUvs = spFloatArray_create(16);
	tempColors = spColorArray_create(16);

	ownsAnimationStateData = stateData == 0;
	if (ownsAnimationStateData) stateData = AnimationStateData_create(skeletonData);

	state = AnimationState_create(stateData);

	clipper = spSkeletonClipping_create();
}

SkeletonDrawable::~SkeletonDrawable () {
	// delete vertexArray;
	FREE(worldVertices);
	if (ownsAnimationStateData) AnimationStateData_dispose(state->data);
	AnimationState_dispose(state);
	Skeleton_dispose(skeleton);
	spSkeletonClipping_dispose(clipper);
	spFloatArray_dispose(tempUvs);
	spColorArray_dispose(tempColors);
}

void SkeletonDrawable::step() {
	const float deltaTime = 1.f / float(jngl::getStepsPerSecond());
	Skeleton_update(skeleton, deltaTime);
	AnimationState_update(state, deltaTime * timeScale);
	AnimationState_apply(state, skeleton);
	Skeleton_updateWorldTransform(skeleton);
}

void SkeletonDrawable::draw() const {
	// vertexArray->clear();
	// states.texture = 0;
	unsigned short quadIndices[6] = { 0, 1, 2, 2, 3, 0 };

	if (vertexEffect != 0) vertexEffect->begin(vertexEffect, skeleton);

	std::vector<jngl::Vertex> vertexArray;
	jngl::Sprite* texture = 0;
	for (int i = 0; i < skeleton->slotsCount; ++i) {
		Slot* slot = skeleton->drawOrder[i];
		Attachment* attachment = slot->attachment;
		if (!attachment) continue;

		float* vertices = worldVertices;
		int verticesCount = 0;
		float* uvs = 0;
		unsigned short* indices = 0;
		int indicesCount = 0;
		spColor* attachmentColor;

		if (attachment->type == ATTACHMENT_REGION) {
			RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
			spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, vertices, 0, 2);
			verticesCount = 4;
			uvs = regionAttachment->uvs;
			indices = quadIndices;
			indicesCount = 6;
			texture = (jngl::Sprite*)((AtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
			attachmentColor = &regionAttachment->color;

		} else if (attachment->type == ATTACHMENT_MESH) {
			MeshAttachment* mesh = (MeshAttachment*)attachment;
			if (mesh->super.worldVerticesLength > SPINE_MESH_VERTEX_COUNT_MAX) continue;
			texture = (jngl::Sprite*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject;
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

		// Uint8 r = static_cast<Uint8>(skeleton->color.r * slot->color.r * attachmentColor->r * 255);
		// Uint8 g = static_cast<Uint8>(skeleton->color.g * slot->color.g * attachmentColor->g * 255);
		// Uint8 b = static_cast<Uint8>(skeleton->color.b * slot->color.b * attachmentColor->b * 255);
		// Uint8 a = static_cast<Uint8>(skeleton->color.a * slot->color.a * attachmentColor->a * 255);
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

		if (vertexEffect != 0) {
			spFloatArray_clear(tempUvs);
			spColorArray_clear(tempColors);
			for (int i = 0; i < verticesCount; i++) {
				// spColor vertexColor = light;
				spColor dark;
				dark.r = dark.g = dark.b = dark.a = 0;
				int index = i << 1;
				float x = vertices[index];
				float y = vertices[index + 1];
				float u = uvs[index];
				float v = uvs[index + 1];
				// vertexEffect->transform(vertexEffect, &x, &y, &u, &v, &vertexColor, &dark);
				vertices[index] = x;
				vertices[index + 1] = y;
				spFloatArray_add(tempUvs, u);
				spFloatArray_add(tempUvs, v);
				// spColorArray_add(tempColors, vertexColor);
			}

			for (int i = 0; i < indicesCount; ++i) {
				int index = indices[i] << 1;
				// vertex.position.x = vertices[index];
				// vertex.position.y = vertices[index + 1];
				// vertex.texCoords.x = uvs[index] * size.x;
				// vertex.texCoords.y = uvs[index + 1] * size.y;
				spColor vertexColor = tempColors->items[index >> 1];
				// vertex.color.r = static_cast<Uint8>(vertexColor.r * 255);
				// vertex.color.g = static_cast<Uint8>(vertexColor.g * 255);
				// vertex.color.b = static_cast<Uint8>(vertexColor.b * 255);
				// vertex.color.a = static_cast<Uint8>(vertexColor.a * 255);
				// vertexArray->append(vertex);
			}
		} else {
			for (int i = 0; i < indicesCount; ++i) {
				int index = indices[i] << 1;
				vertexArray.push_back(jngl::Vertex{
					vertices[index],
					vertices[index + 1],
					uvs[index], // * size.x
					uvs[index + 1], // * size.y
				});
			}
		}

		spSkeletonClipping_clipEnd(clipper, slot);
	}
	texture->drawMesh(vertexArray);
	// target.draw(*vertexArray, states);
	spSkeletonClipping_clipEnd2(clipper);

	if (vertexEffect != 0) vertexEffect->end(vertexEffect);
}

}
