#pragma once

// 前方宣言
class Object;
class Collider;

// コンポーネントの基底クラス
class Component
{
public:
	// 仮想デストラクタ
	// Componentを継承したクラスを安全に削除するため
	virtual ~Component() {};

	virtual void Init(void) {}		// 初期化
	virtual void Update(void) {}	// 更新
	virtual void PreDraw(void){}	// 描画前
	virtual void Draw2D(void) {}	// 2D描画
	virtual void Draw3D(void) {}	// 3D描画

	// 衝突イベント
	virtual void OnCollision(Collider* other) {}

	// このComponentを持っているObjectを設定
	void SetOwner(Object* owner) { owner_ = owner; }
	// このComponentのオーナーを渡す
	Object* GetOwner(void) { return owner_; }

protected:
	// このComponentを持っているObject
	Object* owner_ = nullptr;
};
