#pragma once
#include "../Component.h"

// 描画コンポーネントの基底
class Render : public Component
{
public:
	// デストラクタ
	virtual ~Render() override = default;

	// 描画
	virtual void Draw(void) override {};
	
	// 解放
	virtual void Release() {}

	// ハンドルを返す
	int GetHandle(void) const { return handle_; }

protected:
	int handle_ = -1;
};
