#pragma once
#include "../SceneBase.h"
#include <vector>

class KeyConfigUI;

class OptionScene : public SceneBase
{
public:
	// 左側ナビゲーションのメニュー種別
	enum class MENU
	{
		SETTINGS,
		CONTROLS,
		BACK,
		MAX
	};

	// 操作フォーカス（左側ナビを操作中か、右側コンテンツを操作中か）
	enum class FOCUS_AREA
	{
		LEFT_NAVI,
		RIGHT_CONTENT,
	};

	struct NaviItem
	{
		MENU type;
		int graphHandle;
		int x, y;
		int sizeX, sizeY;
	};

	OptionScene(void);
	~OptionScene(void) override;

	void Init(void)     override;
	void Load(void)     override;
	void LoadEnd(void)  override;
	void Update(void)   override;
	void Draw(void)     override;
	void Release(void)  override;

private:
	// 左側ナビの位置関連定数
	static constexpr int NAVI_POS_X = 50;
	static constexpr int NAVI_POS_Y = 350;
	static constexpr int BACK_POS_Y = 500;

	// 画像サイズ
	static constexpr int IMAGE_SIZE_X = 200;
	static constexpr int IMAGE_SIZE_Y = 50;

	// BACK画像サイズ
	static constexpr int BACK_IMAGE_SIZE_X = 154;
	static constexpr int BACK_IMAGE_SIZE_Y = 84;

	// 左側ナビの選択更新
	void UpdateNaviSelect(void);

	// 右側コンテンツの更新処理
	void UpdateSettingsContent(void);

private:
	// UIコンポーネント
	KeyConfigUI* keyConfigUI_;

	// 左側ナビボタン配列
	std::vector<NaviItem> naviButtons_;

	// 状態管理
	MENU currentNavi_;		// 現在選択している左メニュー
	FOCUS_AREA focusArea_;		// 現在どこを操作中か
};
