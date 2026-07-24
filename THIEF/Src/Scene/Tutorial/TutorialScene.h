#pragma once
#include <DxLib.h>
#include <vector>
#include <string>

#include "../../Object/Tag.h"
#include "../SceneBase.h"
#include "TutorialInfo.h"

class ObjectManager;
class EnemyManager;
class EnemyBase;
class Crosshair;
class TutorialWall;

class TutorialScene : public SceneBase
{
public:
	TutorialScene(void);				// コンストラクタ
	~TutorialScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

	void CameraCreate(void);		// カメラの作成
	void StageCreate(std::string path, std::string collPath = "NoData");			// ステージの作成
	void WispCreate(void);			// ライトの作成
	void PlayerCreate(void);		// プレイヤーの作成
	void CartCreate(void);			// カートの作成
	void ItemCreateTutorial(void);	// アイテムの作成

public:

	// 状態遷移
	void SetState(Tutorial::STATE newState);

private:

	static constexpr float MAX_VALUE = 100.0f;

	// 確認項目クリア時の最大カウント　「Good job!」出す時間
	static constexpr int MAX_CLEAR_COUNT = 120;

	// オブジェクトマネージャー
	ObjectManager* objectManger_;

	// クロスヘア
	Crosshair* crosshair_;

	// チュートリアル用の壁
	TutorialWall* tutorialWall_;

	// データ
	std::vector<Tutorial::TutorialInfo> steps_;

	// 確認項目をクリアしたか
	bool isClearState_;

	// 確認項目クリア時のカウント
	int clearStateEndCount_;

	// 達成率
	int achievementRate_;

	// 状態関数型
	typedef void (TutorialScene::*StateFunction)(void);

	// 現在の状態	
	Tutorial::STATE currentState_;	// 現在のステート
	
	StateFunction stateTable_[Tutorial::STATE::MAX];

	// ステート別Update処理
	void Move(void);
	void Jump(void);
	void Dash(void);
	void Crouch(void);
	void Sliding(void);
	void Light(void);
	void OpenMap(void);
	void Grab(void);
	void Range(void);
	void ItemInCart(void);
	void Deliver(void);
	void Clear(void);

	void LoadCsvData(void);

	// 達成率バーの処理
	void UpdateAchievementRate(void);

	// チュートリアルオブジェクトの初期化処理
	void TutorialCreate(void);

	// タグを使用し、アイテムを作る
	void ItemCreate(Tag tag, VECTOR pos);

	void WallCreate(void);

	// 1スロット目の入力名を取得する
	std::string GetSlot1KeyName(INPUT_INFO::ACTION action);

	// 文字列ないの特定のワードを置換する
	std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);

	// ％％タグをキーコンフィグ1スロット目の名称に置換する
	std::string ConvertTutorialTagToKeyName(std::string text);

	// タグ部分だけ色を変えて描画
	void DrawTutorialTextWithHighlight(int x, int y, const std::string& originalText, unsigned int normalColor, unsigned int highlightColor, int fontHandle);

	// パッドの割り当てがスティックか
	std::string GetPadMoveString(void);
};

