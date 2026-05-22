#pragma once
#include "Render.h"
#include <string>

#include "../Transform/Transform.h"

// 3D描画コンポーネント
class Render3D : public Render
{
public:
	// デストラクタで解放処理
	~Render3D() override { Release(); }

	void Init(void) override;		// 初期化
	void Update(void)override;		// 更新
	void Draw(void) override;		// 描画
	void Release(void) override;	// 解放

	// 外部からモデルを設定
	void SetModel(std::string path);

	// 描画フラグを設定
	void SetIsDraw(bool flg);

	// ハンドルを返す
	int GetModel(void) const{ return handle_; }

private:
	Transform* transform_ = nullptr;

	bool isDraw_;
};
