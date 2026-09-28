#pragma once

#include <DxLib.h>

#include "../Component.h"
#include "../Transform/Transform.h"

class Transform;
class PlayerController;

class Camera : public Component
{
public:

	enum class MODE
	{
		NONE,
		FIXED,
		FREE,
		FOLLOW,
	};

public:

	// カメラ初期角度
	static constexpr VECTOR CAMERA_ANGLE_STAGE_1 = { 0.0f, 90.0f * DX_PI_F / 180.0f, 0.0f };	// ステージ1
	static constexpr VECTOR CAMERA_ANGLE_STAGE_2 = { 0.0f,0.0f,0.0f };							// ステージ2
	static constexpr VECTOR CAMERA_ANGLE_STAGE_3 = { 0.0f, 90.0f * DX_PI_F / 180.0f, 0.0f };	// ステージ3

	// マウス感度
	static constexpr float MOUSE_SENSITIVITY = 0.003f;

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
	static constexpr float MOVE_COUNT_MAX = 100.0f;

	// 移動時のカメラ揺らしの幅
	static constexpr float SHAKE_SIZE = 10.0f;

	// プレイヤーの速度から角度を求める際の微調整係数
	static constexpr float SHAKE_ADJUST = 0.4f;

public:

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void PreDraw(void) override;	// 描画前

public:

	// 描画前のカメラ設定
	void SetBeforeDraw(void);

	// モード変更
	void ChangeMode(MODE mode) { mode_ = mode; }

	// 追従対象
	void SetTarget(Transform* target) { target_ = target; }

	// プレイヤーの情報をもらう　※プレイヤーの場合
	void SetPlayerController(PlayerController* playerController) { playerController_ = playerController; }

	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

private:

	// デッドゾーン
	static constexpr float DEAD_ZONE = 0.2f; 

	// 回転速度
	static constexpr float START_DEG = 5.0f;	// 最小
	static constexpr float END_DEG = 100.0f;	// 最大

	// スティック正規化用
	static constexpr float STICK_AXIS_MAX = 1000.0f;

private:

	// Transform
	Transform* transform_ = nullptr;

	// 追従対象Transform
	Transform* target_ = nullptr;

	// プレイヤー
	PlayerController* playerController_ = nullptr;

private:

	// カメラのモード
	MODE mode_ = MODE::FOLLOW;

	// 追従対象の座標
	VECTOR targetPos_{};

	// マウス画像
	int mousePosX_ = 0;
	int mousePosY_ = 0;

	// 移動時の上下揺らし用カウント
	float angleMoveCount = 0.0f;

private:

	// フォローモードの更新処理
	void UpdateFollow(void);

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

	// 注視点の移動
	void TargetPosUpdate(MATRIX mat);
};
