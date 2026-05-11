#pragma once
#include "../Collider.h"
#include <cmath>

// ‰~ƒRƒ‰ƒCƒ_[(2D)
class CircleCollider : public Collider
{
public:
	// ”¼Œa
	float radius_ = 10.0f;

	bool CheckCollision(Collider* other)override
	{
		// Circle“¯m‚Ì‚İ
		auto c = dynamic_cast<CircleCollider*>(other);
		
		if (!c) return false;

		// XY‹——£
		float dx = transform_->pos_.x - c->transform_->pos_.x;
		float dy = transform_->pos_.y - c->transform_->pos_.y;

		// ‹——£‚Ì2æ
		float distSq = dx * dx + dy * dy;

		// ”¼Œa‡Œv
		float r = radius_ + c->radius_;

		// Õ“Ë”»’è
		return distSq <= r * r;
	}
};
