#include "Wisp.h"

#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Manager/Input/InputManager.h"
#include "../../../Common/Manager/Light/LightManager.h"
#include "../Render/Render3D.h"
#include "../../Object.h"

Wisp::Wisp(void)
{
	// ライトの範囲初期化
	range_ = POINTLIGHT_RANGE_MAX;
	// 最大値とする
	isRangeMax_ = true;

	// ライトの光量初期化
	lightPow_ = LIGHT_POW_MAX;

	// 追加ポイントライト
	pointLightHandle_ = CreatePointLightHandle(
		{ 0.0f, 0.0f, 0.0f }, range_,
		ATTEN_0,
		lightPow_,
		ATTEN_2
	);

	// ハンドルのポイントライトに色をつける
	SetLightDifColorHandle(pointLightHandle_,
		GetColorF(
			DEFAULT_LIGHT_COLOR.x / 255.0f,
			DEFAULT_LIGHT_COLOR.y / 255.0f,
			DEFAULT_LIGHT_COLOR.z / 255.0f,
			1.0f));

	 // テクスチャをロード
	for (auto table : LightTable::Table)
	{
		textureId_[static_cast<int>(table.first)] = LoadGraph(table.second.path.c_str());
	}

	lightType_ = LightManager::GetInstance().GetLightType();
}

Wisp::~Wisp(void)
{
	// ポイントライトのハンドルを解放
	DeleteLightHandle(pointLightHandle_);

	for (auto table : LightTable::Table)
	{
		// テクスチャの解放
		DeleteGraph(static_cast<int>(table.first));
	}
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
	trans_ = owner_->GetComponent<Transform>();

	// モデルに大きさ、向き、座標を設定
	MV1SetScale(wispModelId_, scale_);
	MV1SetPosition(wispModelId_, trans_->pos_);
	// プレイヤー側を向くようにする
	LookPlayer();

	// 衝突情報構築
	MV1SetupCollInfo(wispModelId_, -1);
}

void Wisp::Update(void)
{
	// 座標を更新
	UpdatePos();

	// ライトの範囲を更新
	UpdateRange();


	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_H))
	{
		switch (lightType_)
		{
		case COLOR_1:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_2);
			break;
		case COLOR_2:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_3);
			break;
		case COLOR_3:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_4);
			break;
		case COLOR_4:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_5);
			break;
		case COLOR_5:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_6);
			break;
		case COLOR_6:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_7);
			break;
		case COLOR_7:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_8);
			break;
		case COLOR_8:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_9);
			break;
		case COLOR_9:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_10);
			break;
		case COLOR_10:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_11);
			break;
		case COLOR_11:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_12);
			break;
		case COLOR_12:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_MAX);
			break;
		case COLOR_MAX:
			LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_1);
			break;
		default:
			break;
		}

	}

	// ライトの設定に変更があったら設定し直し
	if (LightManager::GetInstance().GetLightType() != lightType_)
	{
		ChangeLightTexture(LightManager::GetInstance().GetLightType());
	}
}

void Wisp::Draw3D(void)
{
#ifdef _DEBUG

	// デバッグ用の描画処理
	DebugDraw();

#endif // _DEBUG
}

void Wisp::SetIsRangeMax(bool flg)
{
	isRangeMax_ = flg;
}

bool Wisp::GetIsRangeMax(void)
{
	return isRangeMax_;
}

void Wisp::ChangeLightTexture(LIGHT_TYPE lightType)
{
	lightType_ = lightType;

	if (lightType_ == LIGHT_TYPE::COLOR_MAX)
	{
		// テクスチャをデフォルトに戻す
		MV1SetTextureGraphHandle(wispModelId_, 0, -1, false);

		// ライトの色をデフォルトに戻す
		SetLightDifColorHandle(pointLightHandle_, 
			GetColorF(
				DEFAULT_LIGHT_COLOR.x / 255.0f,
				DEFAULT_LIGHT_COLOR.y / 255.0f, 
				DEFAULT_LIGHT_COLOR.z / 255.0f,
				1.0f));

		return;
	}

	auto lightData = LightTable::Table.find(lightType_);

	// テクスチャを変更
	MV1SetTextureGraphHandle(wispModelId_, 0, textureId_[static_cast<int>(lightType_)], false);
	// ライトの色を変更
	SetLightDifColorHandle(pointLightHandle_,
		GetColorF(
			lightData->second.color.x / 255.0f,
			lightData->second.color.y / 255.0f,
			lightData->second.color.z / 255.0f,
			1.0f));
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

	// プレイヤー側を向くようにする
	LookPlayer();
}

void Wisp::UpdateRange(void)
{
	// 前の情報を保持しておく
	float prevRange = range_;
	float prevLightPow = lightPow_;

	if (isRangeMax_)
	{
		// 最大値じゃなければ変更
		if (range_ < POINTLIGHT_RANGE_MAX)
		{
			range_ = POINTLIGHT_RANGE_MAX;
		}
		// 最大値じゃなければ変更
		if (lightPow_ > LIGHT_POW_MAX)
		{
			lightPow_ = LIGHT_POW_MAX;
		}
	}
	else
	{
		// 最小値じゃなければ変更
		if (range_ > POINTLIGHT_RANGE_MIN)
		{
			range_ = POINTLIGHT_RANGE_MIN;
		}
		// 最小値じゃなければ変更
		if (lightPow_ < LIGHT_POW_MIN)
		{
			lightPow_ = LIGHT_POW_MIN;
		}
	}

	// 変更があったら
	if (range_ != prevRange 
		|| lightPow_ != prevLightPow)
	{
		// 線形補間で滑らかにする
		range_ = Math::Lerp(prevRange, range_, COEFFICIENT);
		lightPow_ = Math::Lerp(prevLightPow, lightPow_, COEFFICIENT);

		// ライトに範囲を反映
		SetLightRangeAttenHandle(
			pointLightHandle_,
			range_,
			ATTEN_0,
			lightPow_,
			ATTEN_2);
	}
}

void Wisp::LookPlayer(void)
{
	// カメラの座標
	VECTOR cameraPos = CameraUtility::GetCameraPos();

	// 相手へのベクトルを計算(引き算)
	VECTOR vec;
	vec.x = cameraPos.x - trans_->pos_.x;
	vec.y = cameraPos.y - trans_->pos_.y;
	vec.z = cameraPos.z - trans_->pos_.z;

	// ベクトルの正規化で単位ベクトル(方向)を取得する
	float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

	// Y軸の回転
	trans_->angle_.y = atan2f(vec.x, vec.z);

	// 今回のモデルのY軸向きが逆なので向きを反転させる
	trans_->angle_.y += 180.0f * (DX_PI_F / 180.0f);

	// X軸の回転
	trans_->angle_.x = atan2f(vec.y, length);

	// Z軸は回転させない
	trans_->angle_.z = 0.0f;

	// モデルに向きを設定
	MV1SetRotationXYZ(wispModelId_, trans_->angle_);
}

void Wisp::DebugDraw(void)
{
	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	DrawSphere3D(trans->pos_, 30.0f, 10, 0xff0000, 0xff0000, false);
}
