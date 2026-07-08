#include "Cart.h"
#include "../Render/Render3D.h"
#include "../../Object.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "../../../Common/Math/Math.h"
#include "../Collider/StageCollider/StageCollider.h"
#include "../Collider/3DCollider/CapsuleCollider.h"

Cart::~Cart(void)
{
}

void Cart::Init(void)
{
	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();
	if (!render) return;

	// モデルIDを取得
	modelId_ = render->GetHandle();

	// オーナーからTransformを取得
	trans_ = owner_->GetComponent<Transform>();

	// 座標の更新
	MV1SetPosition(modelId_, trans_->pos_);

	// 向きの初期化
	angleY_ = 0.0f;
	// モデルに座標を反映
	MV1SetRotationXYZ(modelId_, { 0.0f,angleY_,0.0f });

	isGrabbed_ = false;

	// 衝突情報構築
	MV1SetupCollInfo(modelId_, -1);
	MV1SetupCollInfo(modelId_, 1);
}

void Cart::Update(void)
{
	if (isGrabbed_)
	{
		TrackingPlayer();
	}

	// ステージコライダー取得
	auto stageCol = owner_->GetComponent<StageCollider>();

	if (!stageCol) return;

	// ステージの当たり判定の計算処理
	stageCol->StageColl(velocityY_);

	// 当たり判定情報を最新の状態に更新
	MV1RefreshCollInfo(modelId_, -1);
	MV1RefreshCollInfo(modelId_, 1);
}

void Cart::Draw3D(void)
{
#ifdef _DEBUG
	DrawDebug();
#endif // _DEBUG
}

Transform* Cart::GetTransform(void)
{
	return trans_;
}

float Cart::GetAngleY(void)
{
	return angleY_;
}

void Cart::StartGrabbing(VECTOR localPos)
{
	// 掴まれた状態にする
	isGrabbed_ = true;
	localPos_ = localPos;
}

void Cart::EndGrabbed(void)
{
	// 掴まれていない状態にする
	isGrabbed_ = false;
}

void Cart::SetLocalPos(VECTOR localPos)
{
	localPos_ = localPos;
}

void Cart::TrackingPlayer(void)
{
	// 前の座標を保持しておく
	VECTOR prePos = trans_->pos_;

	// ローカル座標に
	trans_->pos_ = CameraUtility::AddCameraPosLocalPos(localPos_);
	trans_->pos_.y = CameraUtility::GetCameraPos().y - localPos_.y;
	// 線形補間で滑らかにする
	trans_->pos_ = Math::Lerp(prePos, trans_->pos_, COEFFICIENT);
	//trans_->pos_.y = 5.0f;

	// モデルに座標を反映
	MV1SetPosition(modelId_, trans_->pos_);

	// 前の座標を保持しておく
	float preAngleY = angleY_;
	// カメラのアングルを適用
	angleY_ = CameraUtility::GetCameraAngle().y;
	// 線形補間で滑らかにする
	angleY_ = Math::Lerp(preAngleY, angleY_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetRotationXYZ(modelId_, { 0.0f,angleY_,0.0f });

	// 当たり判定情報を最新の状態に更新
	MV1RefreshCollInfo(modelId_, -1);
	MV1RefreshCollInfo(modelId_, 1);
}

void Cart::DrawDebug(void)
{
	// 納品場所の当たり判定の視覚化
	VECTOR startPos, endPos;
	startPos = endPos = trans_->pos_;
	startPos.y += CART_SIZE_HIG_RAD;
	endPos.y += CART_SIZE_HIG_RAD;

	MATRIX mat = MGetRotY(angleY_);

	VECTOR startLotPos = VTransform(VGet(-CART_SIZE_WID_RAD, -CART_SIZE_HIG_RAD, -CART_SIZE_DEPTH_RAD), mat);
	startPos = VAdd(startLotPos, startPos);

	VECTOR endLotPos = VTransform(VGet(CART_SIZE_WID_RAD, CART_SIZE_HIG_RAD, CART_SIZE_DEPTH_RAD), mat);
	endPos = VAdd(endLotPos, endPos);

	DrawCube3D(endPos, startPos,  0xffff00, 0xffff00, false);

	//auto capsule = owner_->GetComponent<CapsuleCollider>();

	//if (!capsule) return;

	//for (const auto& cap : capsule->GetCapsules())
	//{
	//	VECTOR start = VAdd(trans_->pos_, cap.startOffset);
	//	VECTOR end = VAdd(trans_->pos_, cap.endOffset);

	//	// 両端
	//	DrawSphere3D(
	//		start,
	//		cap.radius,
	//		12,
	//		GetColor(255, 0, 0),
	//		GetColor(255, 0, 0),
	//		TRUE);

	//	DrawSphere3D(
	//		end,
	//		cap.radius,
	//		12,
	//		GetColor(255, 0, 0),
	//		GetColor(255, 0, 0),
	//		TRUE);

	//	// 中心線
	//	DrawLine3D(
	//		start,
	//		end,
	//		GetColor(0, 255, 0));
	//}
}