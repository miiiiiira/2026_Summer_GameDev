#include "MouseCursor.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Application.h"

MouseCursor* MouseCursor::instance_ = nullptr;

MouseCursor::MouseCursor(void)
{
}

void MouseCursor::Load(void)
{
	// マウス画像の読み込み
	mouseImg_ = LoadGraph("Data/Image/Common/cursor.png");
}

void MouseCursor::Init(void)
{
	// マウス座標の初期化
	mousePos_ = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };

	// マウス描画フラグの初期化
	mouseDrawFlg_ = true;
}

void MouseCursor::Update(void)
{
	// マウス画像が読み込まれていない場合処理を行わない
	if (mouseImg_ == -1)return;

	// デバイスがパッドであれば処理を行わない
	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::PAD)
	{
		mousePos_ = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };
		return;
	}

	// 描画フラグが立っていなかったら処理を行わない
	if (!mouseDrawFlg_)return;

	// マウスの場所を取得
	mousePos_ = InputManager::GetInstance()->GetMousePos();
}

void MouseCursor::Draw(void)
{
	// マウス画像が読み込まれていない場合処理を行わない
	if (mouseImg_ == -1)return;

	// デバイスがパッドであればマウスを表示しない
	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::PAD) return;

	// 描画フラグが立っていなかったらマウスを表示しない
	if (!mouseDrawFlg_)return;

	// マウス画像の描画
	DrawRotaGraphF(mousePos_.x, mousePos_.y, 0.2, 0.0, mouseImg_, true);

#ifdef _DEBUG
	
	// デバッグ表示
	DebugDraw();

#endif // _DEBUG

}

void MouseCursor::Destroy(void)
{
	// 画像解放
	DeleteGraph(mouseImg_);

	// インスタンス解放
	DeleteInstance();
}

void MouseCursor::SetMouseDraw(bool flg)
{
	// 指定された描画フラグへ変更
	mouseDrawFlg_ = flg;
}

void MouseCursor::DebugDraw(void)
{
	int posX = static_cast<int>(mousePos_.x);
	int posy = static_cast<int>(mousePos_.y);

	// 当たり判定描画
	DrawBox(posX - MOUSE_IMG_SIZE_WID / 2,
		posy - MOUSE_IMG_SIZE_HIG / 2,
		posX + MOUSE_IMG_SIZE_WID / 2,
		posy + MOUSE_IMG_SIZE_HIG / 2,
		0xff0000, false);
}
