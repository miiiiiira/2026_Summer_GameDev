#include "Item.h"
#include <DxLib.h>
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/Math/Math.h"
#include "../Render/Render3D.h"
#include "../../Object.h"
#include "../PlayerController/PlayerController.h"

Item::Item()
	:Component()
{
	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();
	if (!render) return;

	// モデルIDを取得
	info_.modelId_ = render->GetHandle();

	// オーナーからTransformを取得
	auto trans = owner_->GetComponent<Transform>();

	// 座標の更新
	MV1SetPosition(info_.modelId_, trans->pos_);

	// 衝突情報構築
	MV1SetupCollInfo(info_.modelId_, -1);

	// 重力の初期化
	info_.velocity_ = VGet(0.0f, 0.0f, 0.0f);

	// 初めは掴まれていない状態にする
	info_.isGrabbed = false;

	// 地面に接触していることにする
	info_.hasTouchedStage_ = true;
}

Item::~Item(void)
{
}

void Item::Update(void)
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

void Item::Draw(void)
{
	// 生存していなかったら描画しない
	if (!info_.isAlive_)return;

	owner_->Draw();
#ifdef _DEBUG
	// デバッグ表示
	DrawDebug();
#endif // _DEBUG
}

void Item::SetPlayerCameraInfo(PlayerController* player, VECTOR* cameraPos, VECTOR* cameraAngle)
{
	player_ = player;
	cameraPos_ = cameraPos;
	cameraAngle_ = cameraAngle;
}

Transform* Item::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

const ItemInfo& Item::GetInfo(void)
{
	return info_;
}

VECTOR Item::GetLineStartPos(void)
{
	// カメラの位置をラインの初め座標とする
	return *cameraPos_;
}

VECTOR Item::GetLineEndPos(void)
{
	// ローカル座標
	VECTOR localPosRot;

	// 相対座標
	VECTOR LOCAL_POS = { 0.0f,0.0f,player_->GetRangeMax() };

	localPosRot = VTransform(LOCAL_POS, CameraMatrix());

	// 座標に反映
	VECTOR downPos = VAdd(*cameraPos_, localPosRot);

	return downPos;
}

float Item::GetCameraDistance(VECTOR pos)
{
	return VSize(VSub(pos, *cameraPos_));
}

void Item::TrackingPlayer(void)
{
	// 前の座標を保持しておく
	VECTOR prePos = info_.pos_;

	// ローカル座標に
	info_.pos_ = VAdd(*cameraPos_, ToCameraLocalPosRot());;

	// 線形補間で滑らかにする
	info_.pos_ = Math::Lerp(prePos, info_.pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(info_.modelId_, info_.pos_);

	// 回転
	// 回転行列をモデルに反映
	MV1SetRotationMatrix(info_.modelId_, AngleToMatrix());

	// 当たり判定情報を最新の状態に更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

VECTOR Item::ToCameraLocalPosRot(void)
{
	// ローカル座標
	VECTOR localPosRot = VTransform(info_.localPos_, CameraMatrix());

	return  localPosRot;
}

MATRIX Item::AngleToMatrix(void)
{
	// アイテムの回転を行列にする
	MATRIX lanternMat = Matrix::GetMatrixRotateXYZ(info_.angle_);

	// プレイヤーの回転をランタンの回転行列に反映する
	MATRIX mat = Matrix::Multiplication(lanternMat, CameraMatrix());
	return mat;
}

MATRIX Item::CameraMatrix(void)
{
	// カメラの回転行列
	VECTOR vec = { cameraAngle_->x ,cameraAngle_->y ,0.0f };
	MATRIX matRot = Matrix::GetMatrixRotateXYZ(vec);
	return matRot;
}

void Item::DrawDebug(void)
{
	// お金表示
	DrawFormatString(0, 0, 0xff0000, "%d", info_.money_);

	VECTOR start = info_.pos_;
	start.y -= info_.collisionOffset_;
	VECTOR end = info_.pos_;
	end.y += info_.collisionOffset_;

	// 当たり判定用のカプセル大きさ確認
	DrawCapsule3D(start, end, info_.collisionRadius_, 8, 0xff0000, 0xff0000, false);
}
