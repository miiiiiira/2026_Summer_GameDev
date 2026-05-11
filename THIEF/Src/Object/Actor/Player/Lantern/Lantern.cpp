#include <DxLib.h>
#include "Lantern.h"

#include "../../../../Common/Transform/MatrixUtility.h"
#include "../../../../Common/Math/Math.h"
#include "../../Player/Player.h"
#include "../../../../Input/InputManager.h"

Lantern::Lantern(Player* player)
{
	player_ = player;

	// ポイントライトハンドルの初期化
	pointLightHandle_ = -1;

	// モデルハンドルの初期化
	modelId_ = -1;
}

Lantern::~Lantern(void)
{
}

void Lantern::Load(void)
{
	// 追加ポイントライト
	pointLightHandle_ = CreatePointLightHandle(
		{ 0.0f, 0.0f, 0.0f }, LANTEERN_RANGE, 0.000f, 0.002f, 0.000f);

	// ハンドルのポイントライトに色をつける
	SetLightDifColorHandle(pointLightHandle_, GetColorF(0.5f, 0.2f, 0.3f, 1.0f));

	// モデルの読み込み
	modelId_ = MV1LoadModel("");
}

void Lantern::Init(void)
{
	// 大きさの初期化
	scale_ = SCALE;

	// 向きの初期化
	angle_ = DEFAULT_ANGLE;

	// 座標の初期化
	pos_ = DEFAULT_POS;

	// モデルに大きさ、向き、座標を設定
	MV1SetScale(modelId_, scale_);
	MV1SetRotationXYZ(modelId_, angle_);
	MV1SetPosition(modelId_, pos_);
}

void Lantern::Update(const VECTOR& cameraPos, const VECTOR& cameraAngle)
{
	// ランタンの座標更新
	UpdatePos(cameraPos, cameraAngle);

	// ポイントライトの座標を更新
	SetLightPositionHandle(pointLightHandle_, pos_);

}

void Lantern::Draw(void)
{
	// モデルを描画
	MV1DrawModel(modelId_);

#ifdef _DEBUG
	// デバッグ用の描画処理
	DebugDraw();
#endif // _DEBUG

}

void Lantern::Release(void)
{
	// ポイントライトのハンドルを解放
	DeleteLightHandle(pointLightHandle_);
}


void Lantern::UpdatePos(const VECTOR& cameraPos, const VECTOR& cameraAngle)
{
	// 前の座標を保持しておく
	VECTOR prePos = pos_;

	// カメラの回転行列
	VECTOR vec = { cameraAngle.x ,cameraAngle.y ,0.0f };
	MATRIX matRot = Matrix::GetMatrixRotateXYZ(vec);

	// カメラの視線方向のベクトルを計算
	// DxlibのVTransformを使用
	VECTOR forward = VGet(0.0f, 0.0f, 1.0f); // 前方向をZ軸とする
	// カメラの方向を算出
	VECTOR cameraDir = VTransform(forward, matRot);

	// ローカル座標
	VECTOR localPosRot;

	// 座標
	// 方向と同じ要領で、相対座標を回転
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_Q))
	{
		// ランタンを遠くに離す相対座標を回転させる
		localPosRot = VTransform(REACH_MAX_LIGHT, matRot);
	}
	else
	{
		// ランタンを近くにする相対座標を回転させる
		localPosRot = VTransform(REACH_DEFAULT_LIGHT, matRot);

	}

	// ランタンの座標に反映
	pos_ = VAdd(cameraPos, localPosRot);

	// 線形補間で滑らかにする
	pos_ = Math::Lerp(prePos, pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(modelId_, pos_);

	// 回転
	// ランタンの回転を行列にする
	MATRIX weaponMat = Matrix::GetMatrixRotateXYZ(angle_);

	// プレイヤーの回転をランタンの回転行列に反映する
	MATRIX mat = Matrix::Multiplication(weaponMat, matRot);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);
}

void Lantern::DebugDraw(void)
{
	DrawSphere3D(pos_, 30.0f, 10, 0xff0000, 0xff0000, false);
}
