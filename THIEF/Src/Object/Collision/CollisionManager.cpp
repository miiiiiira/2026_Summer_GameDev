#include "CollisionManager.h"
#include "../Component/Collider/Collider.h"
#include "../Object.h"

CollisionManager& CollisionManager::Instance()
{
	static CollisionManager instance;
	return instance;
}

void CollisionManager::Register(Collider* col)
{
	colliders_.push_back(col);
}

void CollisionManager::CheckAll()
{
	for (size_t i = 0; i < colliders_.size(); i++)
	{
		for (size_t j = i + 1; j < colliders_.size(); j++)
		{
			auto a = colliders_[i];
			auto b = colliders_[j];

			if (a->CheckCollision(b))
			{
				a->GetOwner()->SendCollision(b);
				b->GetOwner()->SendCollision(a);
			}
		}
	}
}

void CollisionManager::Clear()
{
	colliders_.clear();
}
