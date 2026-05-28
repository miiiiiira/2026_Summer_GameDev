#pragma once
#include "Render.h"
#include <string>

#include "../Transform/Transform2D.h"

// 2D描画コンポーネント
class Render2D : public Render
{
public:
	// デストラクタで解放処理
	~Render2D() override { Release(); }

	void Init(void) override;		// 初期化
	void Draw2D(void)	override;		// 描画
	void Release(void) override;	// 解放

	// 外部からモデルを設定
	void SetModel(std::string path);

	// ハンドルを返す
	int GetModel(void) const { return handle_; }

private:
	Transform2D* transform_ = nullptr;
};
