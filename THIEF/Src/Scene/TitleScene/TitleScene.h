#pragma once
#include "../SceneBase.h"
#include <memory>

class Confirm;

class TitleScene : public SceneBase
{
public:

	TitleScene(void);				// コンストラクタ
	~TitleScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// 確認画面
	std::shared_ptr<Confirm> confirm_ = nullptr;

	// タイトルロゴ
	int handle_ = -1;

	// PushAnyButton画像のハンドル
	int buttonHandle_ = -1;

	// アルファ値
	float alpha_ = 0.0f;

	// ボタンのアルファ値が増加しているかどうか
	bool isIncreasing_ = false;

	// サウンドを鳴らしたか
	bool isPlaySoundSE_ = false;

	// シェーダー初期化処理
	void ShaderInit(void);
};
