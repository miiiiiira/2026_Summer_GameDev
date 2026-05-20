#include "ItemBase.h"

#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/Math/Math.h"
#include "../../Component/PlayerController/PlayerController.h"
#include "../../../Input/InputManager.h"

ItemBase::ItemBase(void)
	:cameraPos_(nullptr)
	,cameraAngle_(nullptr)
{
	info_.modelId_ = -1;
	info_.velocity_ = VGet(0.0f,0.0f,0.0f);
	info_.prevPos_ = info_.pos_;
}

ItemBase::~ItemBase(void)
{
}

void ItemBase::Update(void)
{
	// 前回座標を更新
	info_.prevPos_ = info_.pos_;

	// 掴まれていたら
	if (info_.isGrabbed)
	{
		// プレイヤーの位置を見て移動処理を行う
		TrackingPlayer();
	}
		// 重力をかける
		Gravity();
}

void ItemBase::Draw(void)
{
	// 生存していなかったら描画しない
	if (!info_.isAlive_)return;

	// モデルを描画
	MV1DrawModel(info_.modelId_);

#ifdef _DEBUG
	// デバッグ表示
	DrawDebug();
#endif // _DEBUG

}

void ItemBase::Release(void)
{
	// モデルハンドルの解放
	MV1DeleteModel(info_.modelId_);
}

const ItemInfo& ItemBase::GetInfo(void)
{
	return info_;
}

void ItemBase::SetDamage(int damage)
{
	// 指定のダメージ分お金を削る
	info_.money_ -= damage;

	// お金が0以下になったら
	if (info_.money_ <= 0)
	{
		// 0初期化
		info_.money_ = 0;

		// 生存フラグを折る
		info_.isAlive_ = false;
	}
}

void ItemBase::SetPos(const VECTOR& pos)
{
	// 指定された座標に設定
	info_.pos_ = pos;

	// モデルに座標を反映
	MV1SetPosition(info_.modelId_, info_.pos_);

	// 当たり判定更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void ItemBase::SetLocalPosZ(float localPosZ)
{
	info_.localPos_.z = localPosZ;
}

void ItemBase::StartGrabbing(VECTOR localPos)
{
	// 掴まれた状態にする
	info_.isGrabbed = true;

	// プレイヤーとの相対座標をセット
	info_.localPos_ = localPos;
	info_.velocity_.y = 0.0f;
}

void ItemBase::EndGrabbed(void)
{
	// 掴まれていない状態にする
	info_.isGrabbed = false;

	MV1SetPosition(info_.modelId_, info_.pos_);
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);

	// 重力を初期化する
	info_.velocity_.y = 0.0f;
}

void ItemBase::SetCameraPosAngle(VECTOR* cameraPos, VECTOR* cameraAngle)
{
	// カメラの座標と向きのポインタを保持
	cameraPos_ = cameraPos;
	cameraAngle_ = cameraAngle;
}

void ItemBase::Gravity(void)
{
	// 座標に重力を反映
	info_.pos_.y += info_.velocity_.y;

	// 重力加算
	info_.velocity_.y += GRAVITY;

	// 最大落下速度
	if (info_.velocity_.y < MAX_FALL)
		info_.velocity_.y = MAX_FALL;

	// モデルの座標を反映
	MV1SetPosition(info_.modelId_, info_.pos_);
}

void ItemBase::TrackingPlayer(void)
{
	// プレイヤーの座標
	// カメラの座標や向きのポインタの中身がなかったら処理を行わない
	if (cameraPos_ == nullptr || cameraAngle_ == nullptr)return;

	// 前の座標を保持しておく
	VECTOR prePos = info_.pos_;

	// カメラの回転行列
	VECTOR vec = { cameraAngle_->x ,cameraAngle_->y ,0.0f };
	MATRIX matRot = Matrix::GetMatrixRotateXYZ(vec);

	// ローカル座標
	VECTOR localPosRot;

	// 座標
	localPosRot = VTransform(info_.localPos_, matRot);

	// アイテムの座標に反映
	info_.pos_ = VAdd(*cameraPos_, localPosRot);

	// 線形補間で滑らかにする
	info_.pos_ = Math::Lerp(prePos, info_.pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(info_.modelId_, info_.pos_);

	// 回転
	// アイテムの回転を行列にする
	MATRIX lanternMat = Matrix::GetMatrixRotateXYZ(info_.angle_);

	// プレイヤーの回転をランタンの回転行列に反映する
	MATRIX mat = Matrix::Multiplication(lanternMat, matRot);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(info_.modelId_, mat);

	// 当たり判定情報を最新の状態に更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void ItemBase::DrawDebug(void)
{
	VECTOR start = info_.pos_;
	start.y -= info_.collisionOffset_;
	VECTOR end = info_.pos_;
	end.y += info_.collisionOffset_;

	DrawCapsule3D(start, end, info_.collisionRadius_, 8, 0xff0000, 0xff0000, false);
}
