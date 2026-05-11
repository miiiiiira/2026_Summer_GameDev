#pragma once
#include "../Collider.h"
#include <cmath>

// ‹…‘ÌƒRƒ‰ƒCƒ_[
class SphereCollider : public Collider
{
public:
	// ”¼Œa
	float radius_ = 10.0f;

	bool CheckCollision(Collider* other) override
	{
		// ‹…‘Ì“¯m‚Ì‚İ
		auto s = dynamic_cast<SphereCollider*>(other);
		if (!s) return false;

		// ‹——£ŒvZ
		float dx = transform_->pos_.x - s->transform_->pos_.x;
		float dy = transform_->pos_.y - s->transform_->pos_.y;
		float dz = transform_->pos_.z - s->transform_->pos_.z;

		// ‹——£‚ğ2æ
		float distSq = dx * dx + dy * dy + dz * dz;

		// ”¼Œa‡Œv
		float r = radius_ + s->radius_;

		// Õ“Ë”»’è
		return distSq <= r * r;
	}
};
