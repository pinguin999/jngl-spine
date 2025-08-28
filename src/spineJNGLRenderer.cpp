#include "spineJNGLRenderer.hpp"

#include "SkeletonRenderer.hpp"
#include <spine/Extension.h>

#include <jngl.hpp>

spine::SpineExtension* spine::getDefaultExtension() {
	return new spine::DefaultSpineExtension();
}

void TextureLoader::load(spine::AtlasPage& page, const spine::String& path) {
	jngl::Sprite* texture = new jngl::Sprite(path.buffer());
	// if (!texture->loadFromFile(path)) return;

	// TODO
	// if (self->magFilter == SP_ATLAS_LINEAR) texture->setSmooth(true);
	// if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT)
	// texture->setRepeated(true);

	page.texture = texture;
	// Vector2u size = texture->getSize();
	page.width = texture->getWidth();
	page.height = texture->getHeight();
}

void TextureLoader::unload(void* texture) {
	delete (jngl::Sprite*)texture;
}

char* _spUtil_readFile(const char* path, int* length) {
	const auto str = jngl::readAsset(path).str();
	if (length) {
		*length = static_cast<int>(str.length());
	}
	char* buf = static_cast<char*>(malloc(str.length() + 1));
	std::copy(str.begin(), str.end(), buf);
	buf[str.length()] = '\0';
	return buf;
}

SkeletonDrawable::SkeletonDrawable(spine::SkeletonData* skeletonData,
                                   spine::AnimationStateData* stateData)
: timeScale(1) {
	quadIndices.add(0);
	quadIndices.add(1);
	quadIndices.add(2);
	quadIndices.add(2);
	quadIndices.add(3);
	quadIndices.add(0);
	spine::Bone::setYDown(true);
	// worldVertices = MALLOC(float, SPINE_MESH_VERTEX_COUNT_MAX);
	skeleton = new spine::Skeleton(skeletonData);
	// tempUvs = spine::spFloatArray_create(16);
	// tempColors = spine::spColorArray_create(16);

	ownsAnimationStateData = stateData == 0;
	if (ownsAnimationStateData) stateData = new spine::AnimationStateData(skeletonData);

	state = new spine::AnimationState(stateData);
}

SkeletonDrawable::~SkeletonDrawable() {
	// delete vertexArray;
	// FREE(worldVertices);
	// if (ownsAnimationStateData) delete state->data;
	delete state;
	delete skeleton;
	// delete tempUvs;
	// delete tempColors;
}

void SkeletonDrawable::step() {
	const float deltaTime = 1.f / float(jngl::getStepsPerSecond());
	state->update(deltaTime * timeScale);
	state->apply(*skeleton);
	skeleton->updateWorldTransform(spine::Physics_None);
}

void SkeletonDrawable::draw() const {

	static auto* blancColor = new spine::Color();

	jngl::Sprite* texture = nullptr;
	for (int j = 0; j < skeleton->getSlots().size(); ++j) {
		spine::Slot& slot = *skeleton->getDrawOrder()[j];
		spine::Attachment* attachment = slot.getAttachment();
		if (!attachment) {
			continue;
		}

		spine::Vector<float>* vertices = &worldVertices;
		int verticesCount = 0;
		spine::Vector<float>* uvs = nullptr;
		spine::Vector<unsigned short>* indices = nullptr;
		int indicesCount = 0;
		spine::Color* attachmentColor = nullptr;

		if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti)) {
			auto* regionAttachment = reinterpret_cast<spine::RegionAttachment*>(attachment);

			vertices->setSize(8, 0);
			regionAttachment->computeWorldVertices(slot, *vertices, 0, 2);
			verticesCount = 4;
			uvs = &regionAttachment->getUVs();
			indices = &quadIndices;
			indicesCount = 6;
			texture =
			    reinterpret_cast<jngl::Sprite*>(regionAttachment->getRegion()->rendererObject);
			attachmentColor = &regionAttachment->getColor();

		} else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti)) {
			auto* mesh = reinterpret_cast<spine::MeshAttachment*>(attachment);
			attachmentColor = &mesh->getColor();

			// Early out if the slot color is 0
			if (attachmentColor->a == 0) {
				clipper.clipEnd(slot);
				continue;
			}

			texture = static_cast<jngl::Sprite*>(mesh->getRegion()->rendererObject);
			vertices->setSize(mesh->getWorldVerticesLength(), 0);
			mesh->computeWorldVertices(slot, 0, mesh->getWorldVerticesLength(), vertices->buffer(),
			                           0, 2);
			verticesCount = mesh->getWorldVerticesLength() >> 1;
			uvs = &mesh->getUVs();
			indices = &mesh->getTriangles();
			indicesCount = indices->size();
		} else if (attachment->getRTTI().isExactly(spine::ClippingAttachment::rtti)) {
			auto* clip = reinterpret_cast<spine::ClippingAttachment*>(slot.getAttachment());
			clipper.clipStart(slot, clip);
			continue;
		} else {
			continue;
		}

		if (attachmentColor == nullptr) {
			attachmentColor = blancColor;
		}
		const auto r = static_cast<uint8_t>(skeleton->getColor().r * slot.getColor().r *
		                                    attachmentColor->r * 255);
		const auto g = static_cast<uint8_t>(skeleton->getColor().g * slot.getColor().g *
		                                    attachmentColor->g * 255);
		const auto b = static_cast<uint8_t>(skeleton->getColor().b * slot.getColor().b *
		                                    attachmentColor->b * 255);
		const auto a = static_cast<uint8_t>(skeleton->getColor().a * slot.getColor().a *
		                                    attachmentColor->a * 255);

		if (clipper.isClipping()) {
			clipper.clipTriangles(*vertices, *indices, *uvs, 2);
			vertices = &clipper.getClippedVertices();
			verticesCount = clipper.getClippedVertices().size() >> 1;
			uvs = &clipper.getClippedUVs();
			indices = &clipper.getClippedTriangles();
			indicesCount = clipper.getClippedTriangles().size();
		}

		std::vector<jngl::Vertex> vertexArray;
		for (int i = 0; i < indicesCount; ++i) {
			int const index = (*indices)[i] << 1;
			vertexArray.push_back(jngl::Vertex{
			    (*vertices)[index], (*vertices)[index + 1],
			    (*uvs)[index],     // * size.x
			    (*uvs)[index + 1], // * size.y
			});
		}
		if (r < 250 || g < 250 || b < 250) {
			jngl::setSpriteColor(r, g, b);
		}
		if (texture) {
			jngl::setSpriteColor(r, g, b, a);
			texture->drawMesh(vertexArray);
			jngl::setSpriteColor(255, 255, 255, 255);
		}

		clipper.clipEnd(slot);
	}
	// target.draw(*vertexArray, states);
	clipper.clipEnd();

	jngl::setSpriteColor(255, 255, 255, 255);


	// Alternative simpler method:
	// spine::RenderCommand* command = SkeletonRenderer::handle().spine.render(*skeleton);
	// while (command) {
	// 	std::vector<jngl::Vertex> vertices;
	// 	float* positions = command->positions;
	// 	float* uvs = command->uvs;
	// 	uint32_t* colors = command->colors;
	// 	uint16_t* indices = command->indices;
	// 	jngl::Sprite* texture = (jngl::Sprite*)command->texture;
	// 	for (int i = 0, n = command->numIndices; i < n; ++i) {
	// 		int ii = indices[i];
	// 		int index = ii << 1;
	// 		jngl::Vertex vertex;
	// 		vertex.x = positions[index];
	// 		vertex.y = positions[index + 1];
	// 		vertex.u = uvs[index];
	// 		vertex.v = uvs[index + 1];
	// 		//  vertex.color = colors[i];
	// 		vertices.push_back(vertex);
	// 	}
	// 	//   BlendMode blendMode = command->blendMode; // Spine blend mode equals engine blend mode
	// 	texture->drawMesh(vertices);
	// 	command = command->next;
	// }
}
