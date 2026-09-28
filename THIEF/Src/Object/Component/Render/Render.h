#pragma once

#include <vector>

#include "../Component.h"

// 描画コンポーネントの基底
class Render : public Component
{
public:

	// デストラクタ
	virtual ~Render(void) override = default;

	virtual void Draw2D(void) override {};	// 2D描画
	virtual void Draw3D(void) override {};	// 3D描画
	virtual void Release(void) {};			// 解放

public:

	// ハンドルを返す
	int GetHandle(void) const { return handle_; }

protected:

	int handle_ = -1;

	std::vector<int> handles_;
};
