#include "Camera.h"
#include "../Transform/Transform.h"
#include "../../Object.h"
#include "../../../Common/Math/Math.h"
#include "../../../Input/InputManager.h"
#include "../../../System/SystemManager.h"
#include "../../../Application.h"

#include "../PlayerController/PlayerController.h"

void Camera::Init(void)
{
	transform_ = owner_->GetComponent<Transform>();

	transform_->pos_ = DERFAULT_POS;
	angle_ = DERFAULT_ANGLES;
}

void Camera::Update(void)
{
	ProcessRot(true);
}

void Camera::PreDraw(void)
{
	SetBeforeDraw();
}

void Camera::SetBeforeDraw(void)
{
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	switch (mode_)
	{
	case Camera::MODE::FIXED:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	case Camera::MODE::NONE:
	default:
		SetBeforeDrawFree();
		break;
	}
}

void Camera::SetBeforeDrawFixedPoint()
{
	SetCameraPositionAndAngle(
		transform_->pos_,
		angle_.x,
		angle_.y,
		angle_.z
	);
}

void Camera::SetBeforeDrawFree()
{
	SetCameraPositionAndAngle(
		transform_->pos_,
		angle_.x,
		angle_.y,
		angle_.z
	);
}

void Camera::SetBeforeDrawFollow()
{
	if (!target_) return;

	// カメラの回転行列を作成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angle_.x));
	mat = MMult(mat, MGetRotY(angle_.y));

	// 追従対象の座標
	VECTOR followPos = target_->pos_;

	// カメラY軸座標を保持しておく
	float prePosY = transform_->pos_.y;

	// 相対座標からワールド座標に直して、カメラ座標とする
	// しゃがみ状態かスライディング状態であれば、カメラの位置を下げる
	if (playerController_->state_ == PlayerState::CROUCHING
		|| playerController_->state_ == PlayerState::SLIDING)
	{
		// 相対座標をカメラの回転を反映
		transform_->pos_ = VAdd(followPos, FOLLOW_CAMERA_LOCAL_POS_CROUCHING);
	}
	// しゃがみ状態でなければ、カメラの位置は立ち状態のまま
	else
	{
		transform_->pos_ = VAdd(followPos, FOLLOW_CAMERA_LOCAL_POS_STANDING);
	}

	// 線形補間で滑らかにする
	transform_->pos_.y = Math::Lerp(prePosY, transform_->pos_.y, COEFFICIENT);

	// 注視点の移動
	// 回転させた相対座標
	VECTOR targetLocalRotPos = VTransform(FOLLOW_TARGET_LOCAL_POS, mat);

	// カメラ座標との高さを一致させるためカメラ座標から回転させた相対座標を足す
	targetPos_ = VAdd(transform_->pos_, targetLocalRotPos);

	// カメラの上方向を計算
	VECTOR up = VTransform(Math::DIR_U, mat);

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_->pos_,
		targetPos_,
		up
	);
}

void Camera::ChangeMode(MODE mode)
{
	mode_ = mode;
}

void Camera::SetTarget(Transform* target)
{
	target_ = target;
}

void Camera::SetPlayerController(PlayerController* playerController)
{
	playerController_ = playerController;
}

void Camera::ProcessRot(bool isLimit)
{
	if (SystemManager::GetInstance().GetIsDevice())
	{
		// 方向回転によるXYZの移動(キーボード)
		RotKeyboard(isLimit);

		// 方向回転によるXYZの移動(マウス)
		RotMouse(isLimit);
	}
	else
	{
		// 方向回転によるXYZの移動(ゲームパッド)
		RotGamePad(isLimit);
	}
}

void Camera::RotKeyboard(bool isLimit)
{
	// 矢印キーでカメラの角度を変える
	float rotPow = 1.0f * DX_PI_F / 180.0f;

	// isLimitがtrueだった場合カメラの視点操作(上下)に上限を付ける
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_DOWN))
	{
		angle_.x += rotPow;

		if (isLimit && angle_.x > LIMIT_X_DW_RAD)
		{
			angle_.x = LIMIT_X_DW_RAD;
		}
	}

	if (InputManager::GetInstance()->IsNew(KEY_INPUT_UP))
	{
		angle_.x -= rotPow;

		if (isLimit && angle_.x < LIMIT_X_UP_RAD)
		{
			angle_.x = LIMIT_X_UP_RAD;
		}
	}

	// 視点操作(左右)
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_RIGHT)) { angle_.y += rotPow; }
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_LEFT)) { angle_.y -= rotPow; }
}

void Camera::RotGamePad(bool isLimit)
{
	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		InputManager::GetInstance()->GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	VECTOR dir = Math::VECTOR_ZERO;


	const float ROT_POW_DEG = 2.0f;
	const float rotPow = ROT_POW_DEG * DX_PI_F / 180.0f;

	// 右スティックの傾き
	dir = InputManager::GetInstance()->GetDirectionXZAKey(padState.AKeyRX, padState.AKeyRY);

	// 右スティック左右の傾き
	angle_.y += dir.x * rotPow;

	// 右スティック上下の傾き
	angle_.x += dir.z * rotPow;

	// 角度制限
	if (!isLimit)return;

	if (angle_.x > LIMIT_X_DW_RAD)
	{
		angle_.x = LIMIT_X_DW_RAD;
	}
	if (angle_.x < LIMIT_X_UP_RAD)
	{
		angle_.x = LIMIT_X_UP_RAD;
	}
}

void Camera::RotMouse(bool isLimit)
{
	// 現在のマウス座標を取得
	GetMousePoint(&mousePosX_, &mousePosY_);

	// 画面中央からのマウスの移動量を調べる
	int deltaX = mousePosX_ - Application::SCREEN_SIZE_X / 2;
	int deltaY = mousePosY_ - Application::SCREEN_SIZE_Y / 2;

	// マウスの移動量が一定のしきい値以下であれば処理をスキップ
	if ((float)std::abs(deltaX) < THRESHOLD && (float)std::abs(deltaY) < THRESHOLD)
	{
		// マウスカーソルを画面中央に戻す
		SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
		return;
	}

	// マウスの移動量からカメラの回転量を更新する
	angle_.y += deltaX * SystemManager::GetInstance().GetMouseSensitivity();
	angle_.x += deltaY * SystemManager::GetInstance().GetMouseSensitivity();

	// ピッチ角の角度制限（真上や真下を向きすぎないようにする）
	if (isLimit && angle_.x > LIMIT_X_DW_RAD)
	{
		angle_.x = LIMIT_X_DW_RAD;
	}
	if (isLimit && angle_.x < LIMIT_X_UP_RAD)
	{
		angle_.x = LIMIT_X_UP_RAD;
	}

	//// マウスカーソルを画面中央に戻す
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
}
