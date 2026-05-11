#pragma once
#include <vector>
#include "../Component/Collider/Collider.h"

class CollisionManager
{
public:
	static CollisionManager& Instance();

	void Register(Collider* col);
	void CheckAll();
	void Clear();

private:
	std::vector<Collider*> colliders_;
};
