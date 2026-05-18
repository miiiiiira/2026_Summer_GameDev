#include "Lantern.h"


#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/Math/Math.h"
#include "../../../Input/InputManager.h"
#include "../Render/Render3D.h"
#include "../../Object.h"

Lantern::Lantern(void)
	:cameraPos_(nullptr)
	,cameraAngle_(nullptr)
{
	// 追加ポイントライト
	pointLightHandle_ = CreatePointLightHandle(
		{ 0.0f, 0.0f, 0.0f }, LANTEERN_RANGE, 0.000f, 0.002f, 0.000f);

	// ハンドルのポイントライトに色をつける
	SetLightDifColorHandle(pointLightHandle_, GetColorF(0.5f, 0.2f, 0.3f, 1.0f));
}

Lantern::~Lantern(void)
{
	// ポイントライトのハンドルを解放
	DeleteLightHandle(pointLightHandle_);
}

void Lantern::Init(void)
{
	// 大きさの初期化
	scale_ = SCALE;

	// 向きの初期化
	angle_ = DEFAULT_ANGLE;

	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();
	if (!render) return;

	// モデルIDを取得
	modelId_ = render->GetHandle();

	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	// モデルに大きさ、向き、座標を設定
	MV1SetScale(modelId_, scale_);
	MV1SetRotationXYZ(modelId_, angle_);
	MV1SetPosition(modelId_, trans->pos_);

	// 衝突情報構築
	MV1SetupCollInfo(modelId_, -1);
}

void Lantern::Update(void)
{
	// ランタンの座標を更新
	UpdatePos();
}

void Lantern::Draw(void)
{
#ifdef _DEBUG
	// デバッグ用の描画処理
	DebugDraw();
#endif // _DEBUG

}

void Lantern::SetCameraPosAngle(VECTOR* cameraPos, VECTOR* cameraAngle)
{
	// カメラの座標と向きのポインタを保持
	cameraPos_ = cameraPos;
	cameraAngle_ = cameraAngle;
}

void Lantern::SetLight(bool lightFlg)
{
	// 指定されたライト状態にする
	SetLightEnableHandle(pointLightHandle_, lightFlg);
}

bool Lantern::GetLight(void)
{
	return GetLightEnableHandle(pointLightHandle_);
}

void Lantern::UpdatePos(void)
{
	// カメラの座標や向きのポインタの中身がなかったら処理を行わない
	if (cameraPos_ == nullptr || cameraAngle_ == nullptr)return;

	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	// 前の座標を保持しておく
	VECTOR prePos = trans->pos_;

	// カメラの回転行列
	VECTOR vec = { cameraAngle_->x ,cameraAngle_->y ,0.0f };
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
	trans->pos_ = VAdd(*cameraPos_, localPosRot);

	// 線形補間で滑らかにする
	trans->pos_ = Math::Lerp(prePos, trans->pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(modelId_, trans->pos_);

	// 回転
	// ランタンの回転を行列にする
	MATRIX weaponMat = Matrix::GetMatrixRotateXYZ(angle_);

	// プレイヤーの回転をランタンの回転行列に反映する
	MATRIX mat = Matrix::Multiplication(weaponMat, matRot);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// ポイントライトの座標を更新
	SetLightPositionHandle(pointLightHandle_, trans->pos_);
}

void Lantern::DebugDraw(void)
{
	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	DrawSphere3D(trans->pos_, 30.0f, 10, 0xff0000, 0xff0000, false);
}
