#pragma once
#include "../Collider.h"

class CapsuleCollider : public Collider
{
public:

	// 開始位置
	VECTOR startOffset_{ 0.0f,110.0f,0.0f };

	// 終了位置
	VECTOR endOffset_{ 0.0f,30.0f,0.0f };

	// 半径
	float radius_ = 20.0f;

	// カプセル開始点取得
	VECTOR GetStart(void)const
	{
		return VAdd(transform_->pos_, startOffset_);
	}

	// カプセル終了点取得
	VECTOR GetEnd(void)const
	{
		return VAdd(transform_->pos_, endOffset_);
	}

	bool CheckCollision(Collider* other) override
	{
		// カプセル同士のみ
		auto cap = dynamic_cast<CapsuleCollider*>(other);
		if (!cap) return false;

		// カプセル衝突
		int hit = HitCheck_Capsule_Capsule(
			GetStart(), GetEnd(), radius_,
			cap->GetStart(), cap->GetEnd(), cap->radius_
		);

		return hit != 0;
	}
};
