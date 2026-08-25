#include "Collider.h"
#include "../../Object.h"

// ‰Šú‰»
void Collider::Init()
{
	// TransformŽæ“¾
	transform_ = owner_->GetComponent<Transform>();
}
