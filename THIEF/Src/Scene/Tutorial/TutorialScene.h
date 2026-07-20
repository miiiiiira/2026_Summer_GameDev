#pragma once
#include "../SceneBase.h"
#include "TutorialInfo.h"
#include <vector>

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

	// 状態遷移
	void SetState(Tutorial::STATE newState);

	// 状態を返却
	Tutorial::STATE GetState() const { return currentState_; }

private:

	static constexpr float MAX_VALUE = 100.0f;

	// 確認項目クリア時の最大カウント　「Good job!」出す時間
	static constexpr int MAX_CLEAR_COUNT =1;

	// データ
	std::vector<Tutorial::TutorialInfo> steps_;

	// 確認項目をクリアしたか
	bool isClearState_;

	// 確認項目クリア時のカウント
	int clearStateEndCount_;

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
	void Map(void);
	void Grab(void);
	void Range(void);
	void Cart(void);
	void Deliver(void);
	void Clear(void);

	void LoadCsvData(void);
};

