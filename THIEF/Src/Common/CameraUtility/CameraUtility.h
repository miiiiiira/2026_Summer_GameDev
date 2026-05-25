#pragma once

#include <DxLib.h>

class Camera;

namespace CameraUtility
{
	namespace
	{
		Camera* camera_ = nullptr;
	}

	// カメラのポインタをセット
	void SetCameraPoint(Camera* camera);

	// カメラの座標を渡す
	VECTOR GetCameraPos(void);

	// カメラのアングルを渡す
	VECTOR GetCameraAngle(void);

	// カメラの回転行列を渡す
	MATRIX  GetCameraMatrix(void);

	// 指定の座標にカメラの回転を適用する
	VECTOR CameraRotToPos(VECTOR pos);

	// 指定のアングルを行列に変換し、カメラの回転を反映する
	MATRIX AngleToMatrix(VECTOR angle);

	// ローカル座標とカメラ座標を足す
	VECTOR AddCameraPosLocalPos(VECTOR localPos);
}

