#pragma once

#include <string>

#include "Render.h"
#include "../Transform/Transform.h"

// 3D描画コンポーネント
class Render3D : public Render
{
public:
	// デストラクタで解放処理
	~Render3D(void) override { Release(); }

	void Init(void) override;		// 初期化
	void Update(void)override;		// 更新
	void Draw3D(void) override;		// 描画
	void Release(void) override;	// 解放

public:

	// 外部からモデルを設定
	void SetModel(std::string path);
	void SetModelHandles(std::string path);

	// 描画フラグを設定
	void SetIsDraw(bool flg) { isDraw_ = flg; }

	// ハンドルを返す
	int GetModel(void) const{ return handle_; }

	// 指定された番号のハンドルを渡す
	int GetHandles(int index)const;
	// ハンドル全てを渡す
	std::vector<int> GetAllHandles(void)const { return handles_; }

private:

	// Transform
	Transform* transform_ = nullptr;

private:

	// 描画フラグ
	bool isDraw_;
};
