#pragma once
#include "../SceneBase.h"
#include <vector>

class KeyConfigUI;

class OptionScene : public SceneBase
{
public:
	// 左側ナビゲーションのメニュー種別
	enum class NAVI_MENU
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
		NAVI_MENU type;
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
	static constexpr int NAVI_POS_X = 80;
	static constexpr int NAVI_INTERVAL_Y = 60;
	static constexpr int BACK_POS_Y = 600;

	// 左側ナビの選択更新
	void UpdateNaviSelect(void);
	void SelectNaviUp(void);
	void SelectNaviDown(void);
	void MouseNaviSelect(void);

	// 右側コンテンツの更新処理
	void UpdateSettingsContent(void);

private:
	// UIコンポーネント
	KeyConfigUI* keyConfigUI_;

	// 左側ナビボタン配列
	std::vector<NaviItem> naviButtons_;

	// 状態管理
	NAVI_MENU currentNavi_;		// 現在選択している左メニュー
	FOCUS_AREA focusArea_;		// 現在どこを操作中か
};
