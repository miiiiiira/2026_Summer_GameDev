#pragma once
#include "../Component.h"
#include "../Transform/Transform.h"

// コライダー基底クラス
class Collider : public Component
{
public:

	// 仮想デストラクタ
	virtual ~Collider() {}

	// 当たり判定(派生クラス側で実装)
	virtual bool CheckCollision(Collider* other) = 0;

	virtual void Init() override;		// 初期化
	virtual void Update() override;		// 更新

	// 所有オブジェクト取得
	Object* GetOwner() { return owner_; }

protected:
	
	// Transform参照
	Transform* transform_ = nullptr;
};
