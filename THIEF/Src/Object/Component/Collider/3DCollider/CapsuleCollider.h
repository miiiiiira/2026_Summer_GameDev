#pragma once
#include "../Collider.h"
#include <vector>

class CapsuleCollider : public Collider
{
public:

	// カプセル情報
	struct Capsule
	{
		VECTOR startOffset;		// 開始位置
		VECTOR endOffset;		// 終了位置
		float radius;			// 半径
	};

	// カプセルの情報を登録
	void AddCapsule(
		const VECTOR& start,
		const VECTOR& end,
		float radius
	) 
	{
		capsules_.push_back({ start,end,radius });
	}

	// カプセルを取得
	const std::vector<Capsule>& GetCapsules(void) const
	{
		return capsules_;
	}

	// 開始位置取得
	VECTOR GetStart(int index = 0) const
	{
		return VAdd(transform_->pos_, capsules_[index].startOffset);
	}

	// 終了位置取得
	VECTOR GetEnd(int index = 0) const
	{
		return VAdd(transform_->pos_, capsules_[index].endOffset);
	}

	// 半径を取得
	float GetRadius(int index = 0) const
	{
		return capsules_[index].radius;
	}

	// 開始位置を設定
	void SetStartOffset(const VECTOR& start, int index = 0)
	{
		if (index < 0 || index >= static_cast<int>(capsules_.size())) return;
		capsules_[index].startOffset = start;
	}

	// 終了位置を設定
	void SetEndOffset(const VECTOR& end, int index = 0)
	{
		if (index < 0 || index >= static_cast<int>(capsules_.size())) return;
		capsules_[index].endOffset = end;
	}

	// 半径を設定
	void SetRadiusOffset(float radius, int index = 0)
	{
		if (index < 0 || index >= static_cast<int>(capsules_.size())) return;
		capsules_[index].radius = radius;
	}


	bool CheckCollision(Collider* other) override
	{
		auto cap = dynamic_cast<CapsuleCollider*>(other);

		if (!cap) return false;

		for (const auto& my : capsules_)
		{
			VECTOR myStart = VAdd(transform_->pos_, my.startOffset);

			VECTOR myEnd = VAdd(transform_->pos_, my.endOffset);

			for (const auto& enemy : cap->capsules_)
			{
				VECTOR enemyStart =
					VAdd(cap->transform_->pos_, enemy.startOffset);

				VECTOR enemyEnd =
					VAdd(cap->transform_->pos_, enemy.endOffset);

				if (HitCheck_Capsule_Capsule(
					myStart,
					myEnd,
					my.radius,
					enemyStart,
					enemyEnd,
					enemy.radius))
				{
					return true;
				}
			}
		}

		return false;
	}

private:

	// カプセルの当たり判定収納用
	std::vector<Capsule> capsules_;
};
