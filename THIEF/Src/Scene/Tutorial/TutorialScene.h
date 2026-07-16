#pragma once
#include "../SceneBase.h"
#include "TutorialInfo.h"
#include <vector>

class TutorialScene : public SceneBase
{
public:

	// 状態
	enum STATE
	{
		MOVE = 1,
		JUMP,
		DUSH,
		CROUCH,
		SLIDING,
		LIGHT,
		GRAB,
		RANGE,
		CART,
		DELIVER,
		CLEAR,
		MAX,
	};

	TutorialScene(void);				// コンストラクタ
	~TutorialScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

	// 状態遷移
	void SetState(STATE newState);

	// 状態を返却
	STATE GetState() const { return currentState_; }

private:

	static constexpr float MAX_VALUE = 100.0f;

	std::vector<TutorialInfo> steps_;
	float currentStepValue_;
	int totalPlayCount_;
	int currentPlayCount_;

	// 状態関数型
	typedef void (TutorialScene::*StateFunction)(void);

	// 現在の状態	
	STATE currentState_;	// 現在のステート
	STATE nextState_;		// 次のステップのステート
	
	StateFunction stateTable_[MAX];

	void Move(void);
	void Jump(void);
	void Dush(void);
	void Crouch(void);
	void Sliding(void);
	void Light(void);
	void Grab(void);
	void Range(void);
	void Cart(void);
	void Deliver(void);
	void Clear(void);


	void LoadCsvData(void);
};

