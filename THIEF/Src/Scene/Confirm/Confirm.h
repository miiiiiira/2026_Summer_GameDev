#pragma once

#include "../SceneBase.h"
#include <DxLib.h>

class Confirm : public SceneBase
{
public:

	enum class SELECT
	{
		NONE,
		YES,
		NO,
	};

	Confirm(void);					// コンストラクタ
	~Confirm(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放



private:

	// 画像サイズ
	//static constexpr int IMAGE_SIZE_X = 200;
	//static constexpr int IMAGE_SIZE_Y = 50;

	// YES
	//static constexpr int YES_POS_X = 80;
	//static constexpr int YES_POS_Y = 200;

	// NO
	//static constexpr int NO_POS_X = 80;
	//static constexpr int NO_POS_Y = 200;

	void ChangeSelect(SELECT select);

	// メニュー選択時の処理
	void UpdateYes(void);
	void UpdateNo(void);


private:

	SELECT currentSelect_;		// 現在の選択肢
	VECTOR currentSelectPos_;	// 現在の選択肢の位置

	int handle_;				// 画像ハンドル
	int yesImg_;				// YES画像ハンドル
	int noImg_;					// NO画像ハンドル
};

