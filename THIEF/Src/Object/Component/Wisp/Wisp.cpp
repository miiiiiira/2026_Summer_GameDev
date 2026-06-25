#include "Wisp.h"

#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Manager/Input/InputManager.h"
#include "../Render/Render3D.h"
#include "../../Object.h"

Wisp::Wisp(void)
{
	// 追加ポイントライト
	pointLightHandle_ = CreatePointLightHandle(
		{ 0.0f, 0.0f, 0.0f }, LANTEERN_RANGE,
		0.0f,
		0.0009f,
		0.000f
	);

	// ハンドルのポイントライトに色をつける
	SetLightDifColorHandle(pointLightHandle_, GetColorF(0x41 / 255.0f, 0x69 / 255.0f, 0xe1 / 255.0f, 1.0f));
}

Wisp::~Wisp(void)
{
	// ポイントライトのハンドルを解放
	DeleteLightHandle(pointLightHandle_);
}

void Wisp::Init(void)
{
	pointPos_ = DEFAULT_POS;

	// 大きさの初期化
	scale_ = SCALE;

	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();
	if (!render) return;

	// モデルIDを取得
	wispModelId_ = render->GetHandle();

	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	// モデルに大きさ、向き、座標を設定
	MV1SetScale(wispModelId_, scale_);
	MV1SetPosition(wispModelId_, trans->pos_);
	// 回転行列をモデルに反映
	MV1SetRotationMatrix(wispModelId_, CameraUtility::GetCameraMatrix());

	// 衝突情報構築
	MV1SetupCollInfo(wispModelId_, -1);
}

void Wisp::Update(void)
{
	// 座標を更新
	UpdatePos();
}

void Wisp::Draw3D(void)
{
#ifdef _DEBUG

	// デバッグ用の描画処理
	DebugDraw();

#endif // _DEBUG
}

void Wisp::SetLight(bool lightFlg)
{
	// 指定されたライト状態にする
	SetLightEnableHandle(pointLightHandle_, lightFlg);
}

bool Wisp::GetLight(void)
{
	return GetLightEnableHandle(pointLightHandle_);
}

void Wisp::UpdatePos(void)
{
	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	// ポイントライトの座標
	VECTOR pointPrevPos = pointPos_;

	// 前の座標を保持しておく
	VECTOR prePos = trans->pos_;

	// カメラの視線方向のベクトルを計算
	// DxlibのVTransformを使用
	VECTOR forward = VGet(0.0f, 0.0f, 1.0f); // 前方向をZ軸とする
	// カメラの方向を算出
	VECTOR cameraDir = CameraUtility::CameraRotToPos(forward);

	// 座標
	// 方向と同じ要領で、相対座標を回転
	if (InputManager::GetInstance()->PushLightButtons())
	{
		// 座標に反映
		trans->pos_ = CameraUtility::AddCameraPosLocalPos(REACH_MAX_LIGHT);
		pointPos_ = CameraUtility::AddCameraPosLocalPos(VAdd(REACH_MAX_LIGHT, POINTLIGHT_OFFSET));
	}
	else
	{
		// 座標に反映
		trans->pos_ = CameraUtility::AddCameraPosLocalPos(REACH_DEFAULT_LIGHT);
		pointPos_ = CameraUtility::AddCameraPosLocalPos(VAdd(REACH_DEFAULT_LIGHT, POINTLIGHT_OFFSET ));
	}

	// 線形補間で滑らかにする
	pointPos_ = Math::Lerp(pointPrevPos, pointPos_, COEFFICIENT);
	trans->pos_ = Math::Lerp(prePos, trans->pos_, COEFFICIENT);

	// ポイントライトの座標を更新
	SetLightPositionHandle(pointLightHandle_, pointPos_);

	// モデルに座標を反映
	MV1SetPosition(wispModelId_, trans->pos_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(wispModelId_, CameraUtility::GetCameraMatrix());
}

void Wisp::DebugDraw(void)
{
	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	DrawSphere3D(trans->pos_, 30.0f, 10, 0xff0000, 0xff0000, false);
}
