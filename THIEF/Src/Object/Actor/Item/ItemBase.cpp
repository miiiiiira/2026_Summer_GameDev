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
	info_.gravity_ = 0.0f;
}

ItemBase::~ItemBase(void)
{
}

void ItemBase::Update(void)
{
	// 掴まれていたら
	if (info_.isGrabbed)
	{
		// プレイヤーの位置を見て移動処理を行う
		TrackingPlayer();

		if (InputManager::GetInstance()->IsClickMouseLeft())
		{
			EndGrabbed();
		}
	}
	// 掴まれていなかったら
	else
	{
		// 重力をかける
		Gravity();
	}
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

void ItemBase::SetPosAngle(const VECTOR& pos, const VECTOR& angle)
{
	// 指定された座標と向きを反映
	info_.pos_ = pos;
	info_.angle_ = angle;

	MV1SetPosition(info_.modelId_, info_.pos_);
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);
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

void ItemBase::StartGrabbed(VECTOR localPos)
{
	// 掴まれた状態にする
	info_.isGrabbed = true;

	info_.localPos_ = localPos;
}

void ItemBase::EndGrabbed(void)
{
	// 掴まれていない状態にする
	info_.isGrabbed = false;

	MV1SetPosition(info_.modelId_, info_.pos_);
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);
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
	info_.pos_.y += info_.gravity_;
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

	// カメラの視線方向のベクトルを計算
	// DxlibのVTransformを使用
	VECTOR forward = VGet(0.0f, 0.0f, 1.0f); // 前方向をZ軸とする
	// カメラの方向を算出
	VECTOR cameraDir = VTransform(forward, matRot);

	// ローカル座標
	VECTOR localPosRot;

	// 座標
	localPosRot = VTransform(info_.localPos_, matRot);

	// ランタンの座標に反映
	info_.pos_ = VAdd(*cameraPos_, localPosRot);

	// 線形補間で滑らかにする
	info_.pos_ = Math::Lerp(prePos, info_.pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(info_.modelId_, info_.pos_);

	// 回転
	// ランタンの回転を行列にする
	MATRIX weaponMat = Matrix::GetMatrixRotateXYZ(info_.angle_);

	// プレイヤーの回転をランタンの回転行列に反映する
	MATRIX mat = Matrix::Multiplication(weaponMat, matRot);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(info_.modelId_, mat);

}

void ItemBase::DrawDebug(void)
{
	DrawSphere3D(info_.pos_, 20.0f, 8, 0xff0000, 0xff0000, false);
}
