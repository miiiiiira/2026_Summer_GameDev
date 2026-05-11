#include "Collider.h"
#include "../../Object.h"
#include "../../Collision/CollisionManager.h"

// ‰Šú‰»
void Collider::Init()
{
	// Transformæ“¾
	transform_ = owner_->GetComponent<Transform>();
}

// XV
void Collider::Update()
{
	// CollisionManager‚Ö©“®“o˜^
	// –ˆƒtƒŒ[ƒ€“o˜^‚µ‚ÄÕ“Ë”»’è‘ÎÛ‚É‚·‚é
	CollisionManager::Instance().Register(this);
}
