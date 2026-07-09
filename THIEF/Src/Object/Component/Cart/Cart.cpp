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

	// モデルに座標を反映
	MV1SetRotationXYZ(modelId_, trans_->angle_);

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

	ApplyGravity();

	// ステージコライダー取得
	auto stageCol = owner_->GetComponent<StageCollider>();

	if (!stageCol) return;

	// ステージの当たり判定の計算処理
	stageCol->StageColl(velocityY_);

	// 座標の更新
	MV1SetPosition(modelId_, trans_->pos_);

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

float Cart::GetVelocityY(void)
{
	return velocityY_;
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
	VECTOR a = CameraUtility::AddCameraPosLocalPos(localPos_);
	trans_->pos_.x = a.x;
	trans_->pos_.z = a.z;

	// 線形補間で滑らかにする
	trans_->pos_ = Math::Lerp(prePos, trans_->pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(modelId_, trans_->pos_);

	// 前の座標を保持しておく
	float preAngleY = trans_->angle_.y;
	// カメラのアングルを適用
	trans_->angle_.y = CameraUtility::GetCameraAngle().y;
	// 線形補間で滑らかにする
	trans_->angle_.y = Math::Lerp(preAngleY, trans_->angle_.y, COEFFICIENT);

	// モデルに座標を反映
	MV1SetRotationXYZ(modelId_, trans_->angle_);

	// 当たり判定情報を最新の状態に更新
	MV1RefreshCollInfo(modelId_, -1);
	MV1RefreshCollInfo(modelId_, 1);
}

void Cart::ApplyGravity(void)
{
	// StageCollider取得
	auto stageCol = owner_->GetComponent<StageCollider>();

	if (!stageCol) return;

	// Y座標へ反映
	trans_->pos_.y += velocityY_;

	// 接地判定

	// 空中
	if (!stageCol->IsGround())
	{
		// 重力加算
		velocityY_ += GRAVITY;

		// 最大落下速度
		if (velocityY_ < MAX_FALL)
			velocityY_ = MAX_FALL;
	}
	else
	{
		// 地面上なら少し下方向に押す
		// 0だと浮く場合があるため
		velocityY_ = -0.1f;
	}

	// モデルに座標を反映
	MV1SetPosition(modelId_, trans_->pos_);

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

	MATRIX mat = MGetRotY(trans_->angle_.y);

	VECTOR startLotPos = VTransform(VGet(-CART_SIZE_WID_RAD, -CART_SIZE_HIG_RAD, -CART_SIZE_DEPTH_RAD), mat);
	startPos = VAdd(startLotPos, startPos);

	VECTOR endLotPos = VTransform(VGet(CART_SIZE_WID_RAD, CART_SIZE_HIG_RAD, CART_SIZE_DEPTH_RAD), mat);
	endPos = VAdd(endLotPos, endPos);

	DrawCube3D(endPos, startPos,  0xffff00, 0xffff00, false);

	//auto capsule = owner_->GetComponent<CapsuleCollider>();

	//if (!capsule) return;

	//for (const auto& cap : capsule->GetCapsules())
	//{
	//	VECTOR start = VAdd(trans_->pos_, VTransform(cap.startOffset, mat));
	//	VECTOR end = VAdd(trans_->pos_, VTransform(cap.endOffset,mat));

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