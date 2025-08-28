#include "spineJNGLRenderer.hpp"

#include "SkeletonRenderer.hpp"
#include <spine/Extension.h>

#include <jngl.hpp>


#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000
#endif


spine::SpineExtension *spine::getDefaultExtension() {
   return new spine::DefaultSpineExtension();
}

void TextureLoader::load(spine::AtlasPage& page, const spine::String& path) {
	jngl::Sprite* texture = new jngl::Sprite(path.buffer());
	// if (!texture->loadFromFile(path)) return;

	// TODO
	// if (self->magFilter == SP_ATLAS_LINEAR) texture->setSmooth(true);
	// if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT) texture->setRepeated(true);

	page.texture = texture;
	// Vector2u size = texture->getSize();
	page.width = texture->getWidth();
	page.height = texture->getHeight();
}

void TextureLoader::unload(void* texture) {
	delete (jngl::Sprite*)texture;
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


SkeletonDrawable::SkeletonDrawable(spine::SkeletonData* skeletonData, spine::AnimationStateData* stateData) :
		timeScale(1),
		worldVertices(0), clipper(0) {
	// spBone_setYDown(true);
	// worldVertices = MALLOC(float, SPINE_MESH_VERTEX_COUNT_MAX);
	skeleton = new spine::Skeleton(skeletonData);
	// tempUvs = spine::spFloatArray_create(16);
	// tempColors = spine::spColorArray_create(16);

	ownsAnimationStateData = stateData == 0;
	if (ownsAnimationStateData) stateData = new spine::AnimationStateData(skeletonData);

	state = new spine::AnimationState(stateData);

	clipper = new spine::SkeletonClipping();
}

SkeletonDrawable::~SkeletonDrawable () {
	// delete vertexArray;
	// FREE(worldVertices);
	// if (ownsAnimationStateData) delete state->data;
	delete state;
	delete skeleton;
	delete clipper;
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
   spine::RenderCommand *command = SkeletonRenderer::handle().spine.render(*skeleton);
   while (command) {
	  std::vector<jngl::Vertex> vertices;
      float *positions = command->positions;
      float *uvs = command->uvs;
      uint32_t *colors = command->colors;
      uint16_t *indices = command->indices;
      jngl::Sprite *texture = (jngl::Sprite *)command->texture;      
      for (int i = 0, j = 0, n = command->numVertices * 2; i < n; ++i, j += 2) {
      	 jngl::Vertex vertex;
         vertex.x = positions[j];
         vertex.y = positions[j + 1];
         vertex.u = uvs[j];
         vertex.v = uvs[j + 1];
        //  vertex.color = colors[i];
         vertices.push_back(vertex);
      }
    //   BlendMode blendMode = command->blendMode; // Spine blend mode equals engine blend mode
	  texture->drawMesh(vertices);
      command = command->next;
   }
}
