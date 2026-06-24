#pragma once
#include "../Component.h"
#include <DxLib.h>

class Transform;
class PlayerController;

class Camera : public Component
{
public:

	// カメラの初期座標
	static constexpr VECTOR DERFAULT_POS = { 0.0f, 0.0f, 0.0f };

	// カメラの初期角度
	static constexpr VECTOR DERFAULT_ANGLES = {	0.0f, 0.0f, 0.0f };

	// 追従対象から注視点への相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 0.0f, 300.0f };

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 20.0f;
	static constexpr float VIEW_FAR = 5000.0f;

	// カメラのX回転上限度角
	static constexpr float LIMIT_X_UP_RAD = -80.0f * (DX_PI_F / 180.0f);
	static constexpr float LIMIT_X_DW_RAD = 70.0f * (DX_PI_F / 180.0f);

	// 視点のしきい値(マウス)
	static constexpr float THRESHOLD = 1.5f;

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.09f;

	// 移動カウント最高値(カメラの揺らしタイミングに使用)
	static constexpr float MOVE_COUNT_MAX = 32.0f;

public:
	enum class MODE
	{
		NONE,
		FIXED,
		FREE,
		FOLLOW,
	};

	void Init(void) override;
	void Update(void) override;
	void PreDraw(void) override;

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// モード変更
	void ChangeMode(MODE mode);

	// 追従対象
	void SetTarget(Transform* target);

	// プレイヤーの情報をもらう　※プレイヤーの場合
	void SetPlayerController(PlayerController* playerController);

	// 向きの取得
	VECTOR& GetAngle(void) { return angle_; }

	// Transformを返す
	Transform* GetTransform();

private:
	// 描画前のカメラ設定
	void SetBeforeDrawFixedPoint();
	void SetBeforeDrawFree();
	void SetBeforeDrawFollow();

	// カメラ操作		true = 視点操作(上下)上限有り / false = 視点操作(上下)上限なし
	void ProcessRot(bool isLimit);

	// カメラ回転(キーボード)
	void RotKeyboard(bool isLimit);

	// カメラ回転(ゲームパッド)
	void RotGamePad(bool isLimit);

	// カメラ回転(マウス)
	void RotMouse(bool isLimit);

private:
	Transform* transform_ = nullptr;
	Transform* target_ = nullptr;

	PlayerController* playerController_ = nullptr;

	MODE mode_ = MODE::FOLLOW;

	VECTOR angle_{};
	VECTOR targetPos_{};

	int mousePosX_ = 0;
	int mousePosY_ = 0;

	float moveCount;
};
