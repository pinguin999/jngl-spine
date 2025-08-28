#pragma once

#include <jngl.hpp>
#include <spine/spine.h>

class SkeletonRenderer : public jngl::Singleton<SkeletonRenderer> {
public:
    spine::SkeletonRenderer spine;
};
