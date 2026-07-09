#include "CameraUtility.h"
#include "../../Object/Component/Camera/Camera.h"
#include "../../Object/Component/Transform/Transform.h"
#include "../Transform/MatrixUtility.h"

void CameraUtility::SetCameraPoint(Camera* camera)
{
	camera_ = camera;
}

VECTOR CameraUtility::GetCameraPos(void)
{
	return camera_->GetTransform()->pos_;
}

VECTOR CameraUtility::GetCameraAngle(void)
{
	return camera_->GetTransform()->angle_;
}

MATRIX CameraUtility::GetCameraMatrix(void)
{
	// カメラの回転行列
	VECTOR vec = { camera_->GetTransform()->angle_.x,camera_->GetTransform()->angle_.y,0.0f };
	MATRIX matRot = Matrix::GetMatrixRotateXYZ(vec);

	return matRot;
}

VECTOR CameraUtility::CameraRotToPos(VECTOR pos)
{
	// ローカル座標
	VECTOR localPosRot = VTransform(pos, GetCameraMatrix());

	return  localPosRot;
}

MATRIX CameraUtility::AngleToMatrix(VECTOR angle)
{
	// アイテムの回転を行列にする
	MATRIX lanternMat = Matrix::GetMatrixRotateXYZ(angle);

	// プレイヤーの回転をランタンの回転行列に反映する
	MATRIX mat = Matrix::Multiplication(lanternMat, GetCameraMatrix());
	return mat;
}

VECTOR CameraUtility::AddCameraPosLocalPos(VECTOR localPos)
{
	VECTOR pos = VAdd(camera_->GetTransform()->pos_, CameraRotToPos(localPos));
	return pos;
}
