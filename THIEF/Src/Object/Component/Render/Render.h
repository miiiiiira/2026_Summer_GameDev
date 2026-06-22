#pragma once
#include "../Component.h"
#include <vector>

// 描画コンポーネントの基底
class Render : public Component
{
public:
	// デストラクタ
	virtual ~Render() override = default;

	// 描画
	virtual void Draw2D(void) override {};
	virtual void Draw3D(void) override {};
	
	// 解放
	virtual void Release() {};

	// ハンドルを返す
	int GetHandle(void) const { return handle_; }

protected:
	int handle_ = -1;

	std::vector<int> handles_;
};
