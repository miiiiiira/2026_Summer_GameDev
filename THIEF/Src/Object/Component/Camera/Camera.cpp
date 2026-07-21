#include "Camera.h"
#include "../Transform/Transform.h"
#include "../../Object.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Manager/Input/InputManager.h"
#include "../../../Common/Manager/System/SystemManager.h"
#include "../../../Application.h"

#include "../PlayerController/PlayerController.h"

void Camera::Init(void)
{
	transform_ = owner_->GetComponent<Transform>();

	angleMoveCount = 0.0f;

	// マウスカーソルを画面中央に戻す
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

	SetCameraPositionAndAngle(
		transform_->pos_,
		transform_->angle_.x,
		transform_->angle_.y,
		transform_->angle_.z
	);
}

void Camera::Update(void)
{
	// カメラの回転処理
	ProcessRot(true);

	switch (mode_)
	{
	case Camera::MODE::FIXED:
		break;
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::FOLLOW:
		UpdateFollow();
		break;
	case Camera::MODE::NONE:
	default:
		break;
	}
}

void Camera::PreDraw(void)
{
	SetBeforeDraw();
}

void Camera::SetBeforeDraw(void)
{
	// NearFarの初期化
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

void Camera::UpdateFollow(void)
{
	// カメラの回転行列を作成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(transform_->angle_.x));
	mat = MMult(mat, MGetRotY(transform_->angle_.y));

	// 追従対象の座標
	VECTOR followPos = target_->pos_;

	// カメラY軸座標を保持しておく
	float prePosY = transform_->pos_.y;

	VECTOR followCameraPos = {};

	// 相対座標からワールド座標に直して、カメラ座標とする
	// しゃがみ状態かスライディング状態であれば、カメラの位置を下げる
	if (playerController_->GetState() == PLAYER_STATE::CROUCHING
		|| playerController_->GetState() == PLAYER_STATE::SLIDING)
	{
		followCameraPos = VAdd(PlayerController::CROUCHING_CAP_START_OFFSET, PlayerController::STANDING_CAP_END_OFFSET);
	}
	// しゃがみ状態でなければ、カメラの位置は立ち状態のまま
	else
	{
		followCameraPos = VAdd(PlayerController::STANDING_CAP_START_OFFSET, PlayerController::STANDING_CAP_END_OFFSET);
	}

	transform_->pos_ = VAdd(followPos, followCameraPos);

	// プレイヤーの状態が移動しない状態であれば
	if (playerController_->GetState() == PLAYER_STATE::CROUCHING
		|| playerController_->GetState() == PLAYER_STATE::SLIDING
		|| playerController_->GetState() == PLAYER_STATE::HIT_REACT)
	{
		// 移動カウントが動いていたら初期化
		if (angleMoveCount > 0)
		{
			angleMoveCount = 0;
		}
	}

	// プレイヤーの現在位置と前フレーム位置の移動ベクトルを作る
	VECTOR playerMoveVec = VSub(playerController_->GetTransform()->pos_, playerController_->GetTransform()->prevPos_);

	// Y軸移動は抜いたXZ軸の移動距離を計算
	float playerMoveDis = VSize({ playerMoveVec.x,0.0f,playerMoveVec.z });

	// プレイヤーの速度から角度の速度を計算
	angleMoveCount += playerMoveDis * SHAKE_ADJUST;

	// 少しでも動いていたら
	if (playerMoveDis >= 1.0f)
	{
		// 角度を計算
		float angle = (angleMoveCount / MOVE_COUNT_MAX) * DX_PI_F * 2.0f;

		// sin波を使ってY座標を揺らす
		transform_->pos_.y += sinf(angle) * SHAKE_SIZE;
	}

	// 線形補間で滑らかにする
	transform_->pos_.y = Math::Lerp(prePosY, transform_->pos_.y, COEFFICIENT);

	// 注視点の移動
	// 回転させた相対座標
	VECTOR targetLocalRotPos = VTransform(FOLLOW_TARGET_LOCAL_POS, mat);

	// カメラ座標との高さを一致させるためカメラ座標から回転させた相対座標を足す
	targetPos_ = VAdd(transform_->pos_, targetLocalRotPos);

	// 3Dサウンドのリスナーの位置とリスナーの前方位置を設定する
	Set3DSoundListenerPosAndFrontPos_UpVecY(transform_->pos_, targetPos_);
}

void Camera::SetBeforeDrawFixedPoint()
{
	SetCameraPositionAndAngle(
		transform_->pos_,
		transform_->angle_.x,
		transform_->angle_.y,
		transform_->angle_.z
	);
}

void Camera::SetBeforeDrawFree()
{
	SetCameraPositionAndAngle(
		transform_->pos_,
		transform_->angle_.x,
		transform_->angle_.y,
		transform_->angle_.z
	);
}

void Camera::SetBeforeDrawFollow()
{
	if (!target_) return;

	// カメラの回転行列を作成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(transform_->angle_.x));
	mat = MMult(mat, MGetRotY(transform_->angle_.y));

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


Transform* Camera::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

void Camera::ProcessRot(bool isLimit)
{
	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE)
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

	// Y軸回転の上限値
	float rad360 = 360.0f * (DX_PI_F / 180.0f);

	// 0 ～ 360度の範囲にまるめる
	transform_->angle_.y = fmodf(transform_->angle_.y, rad360);

	// fmodfの結果がマイナスになった場合の補正
	if (transform_->angle_.y < 0.0f)
	{
		transform_->angle_.y += rad360;
	}

		// 座標とアングルを更新
	SetCameraPositionAndAngle(
		transform_->pos_,
		transform_->angle_.x,
		transform_->angle_.y,
		transform_->angle_.z
	);
}

void Camera::RotKeyboard(bool isLimit)
{
	// 矢印キーでカメラの角度を変える
	float rotPow = 1.0f * DX_PI_F / 180.0f;

	// isLimitがtrueだった場合カメラの視点操作(上下)に上限を付ける
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_DOWN))
	{
		transform_->angle_.x += rotPow;

		if (isLimit && transform_->angle_.x > LIMIT_X_DW_RAD)
		{
			transform_->angle_.x = LIMIT_X_DW_RAD;
		}
	}

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_UP))
	{
		transform_->angle_.x -= rotPow;

		if (isLimit && transform_->angle_.x < LIMIT_X_UP_RAD)
		{
			transform_->angle_.x = LIMIT_X_UP_RAD;
		}
	}

	// 視点操作(左右)
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_RIGHT)) { transform_->angle_.y += rotPow; }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_LEFT)) { transform_->angle_.y -= rotPow; }
}

void Camera::RotGamePad(bool isLimit)
{
	const float ROT_POW_DEG = 2.0f;
	const float rotPow = ROT_POW_DEG * DX_PI_F / 180.0f;

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_UP))
	{
		transform_->angle_.x -= rotPow; // または += （回転させたい方向）
	}
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_DOWN))
	{
		transform_->angle_.x += rotPow;
	}
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_LEFT))
	{
		transform_->angle_.y -= rotPow;
	}
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CAMERA_RIGHT))
	{
		transform_->angle_.y += rotPow;
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
	transform_->angle_.y += deltaX * SystemManager::GetInstance().GetMouseSensitivity();
	transform_->angle_.x += deltaY * SystemManager::GetInstance().GetMouseSensitivity();

	// ピッチ角の角度制限（真上や真下を向きすぎないようにする）
	if (isLimit && transform_->angle_.x > LIMIT_X_DW_RAD)
	{
		transform_->angle_.x = LIMIT_X_DW_RAD;
	}
	if (isLimit && transform_->angle_.x < LIMIT_X_UP_RAD)
	{
		transform_->angle_.x = LIMIT_X_UP_RAD;
	}

	//// マウスカーソルを画面中央に戻す
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
}
