#pragma once
#include "../../Component.h"
#include "../../Transform/Transform.h"
#include "../3DCollider/CapsuleCollider.h"

// チュートリアル壁コンポーネント
class TutorialWallCollider : public Component
{
	// 定数
private:

	// 1フレーム中に複数回衝突する場合に対応するための最大反復回数
	static constexpr int MAX_BOUNCE = 5;

	static constexpr float RADIUS_OFFSET = 10.0f;

public:
	// 初期化
	void Init(void) override;
	// 更新処理
	void Update(void) override;

	// 壁に当たっているか
	void WallColl(void);

private:

	// Z軸の制限値
	float limitDistance_;

	// Transform
	Transform* transform_ = nullptr;
};

