#pragma once
#include "../Collider.h"
#include <cmath>

// AABBコライダー
class AABBCollider : public Collider
{
public:
	// サイズ
	VECTOR size_{ 10,10,10 };

	bool CheckCollision(Collider* other) override
	{
		// AABB同士のみ
		auto aabb = dynamic_cast<AABBCollider*>(other);
		if (!aabb) return false;

		auto a = transform_->pos_;
		auto b = aabb->transform_->pos_;

		// XYZ軸それぞれで重なり判定
		return (
			abs(a.x - b.x) <= (size_.x + aabb->size_.x) * 0.5f &&
			abs(a.y - b.y) <= (size_.y + aabb->size_.y) * 0.5f &&
			abs(a.z - b.z) <= (size_.z + aabb->size_.z) * 0.5f
			);
	}
};
