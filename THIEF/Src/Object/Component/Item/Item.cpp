#include "Item.h"
#include <DxLib.h>
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/Math/Math.h"
#include "../Render/Render3D.h"
#include "../../Object.h"
#include "../PlayerController/PlayerController.h"

Item::~Item(void)
{
}

void Item::Init(void)
{
	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();
	if (!render) return;

	// モデルIDを取得
	info_.modelId_ = render->GetHandle();

	// オーナーからTransformを取得
	trans_ = owner_->GetComponent<Transform>();

	// 座標の更新
	MV1SetPosition(info_.modelId_, trans_->pos_);

	// 衝突情報構築
	MV1SetupCollInfo(info_.modelId_, -1);

	// 前回座標初期化
	info_.prevPos_ = trans_->pos_;

	// 離された時の座標を初期化
	info_.grabbedPos_ = trans_->pos_;

	// 重力の初期化
	info_.velocity_ = VGet(0.0f, 0.0f, 0.0f);

	// 初めは掴まれていない状態にする
	info_.isGrabbed = false;

	// 地面に接触していることにする
	info_.hasTouchedStage_ = true;

	// 個々のパラメータを設定
	SetParam();
}

void Item::Update(void)
{
	// 生存していなかったら描画しない
	if (!info_.isAlive_)return;

	// 前回座標を更新
	info_.prevPos_ = trans_->pos_;

	// 掴まれていたら
	if (info_.isGrabbed)
	{
		// プレイヤーの位置を見て移動処理を行う
		TrackingPlayer();
	}
	// 重力をかける
	//Gravity();
}

void Item::Draw(void)
{
	// 生存していなかったら描画しない
	if (!info_.isAlive_)return;

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

void Item::SetDamage(int damage)
{
	// 指定のダメージから頑丈さ分引いた数値を実際に与えるダメージとする
	int dmg = damage - info_.hardness_;

	// ダメージがマイナス値だったらHPに変更を行わない(回復してしまうため)
	if (dmg < 0)return;

	info_.money_ -= dmg;
	// お金が0以下になったら
	if (info_.money_ <= 0)
	{
		// 0初期化
		info_.money_ = 0;

		// 生存フラグを折る
		info_.isAlive_ = false;

		// 描画フラグを折る
		auto render = owner_->GetComponent<Render3D>();
		render->SetIsDraw(false);
	}

}

void Item::SetPos(const VECTOR& pos)
{
	// 指定された座標に設定
	trans_->pos_ = pos;

	// モデルに座標を反映
	MV1SetPosition(info_.modelId_, trans_->pos_);

	// 当たり判定更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void Item::SetLocalPosZ(float localPosZ)
{
	info_.localPos_.z = localPosZ;
}

void Item::StartGrabbing(VECTOR localPos)
{
	// 掴まれた状態にする
	info_.isGrabbed = true;

	// 空中状態で一度もステージに接触していないとする
	info_.hasTouchedStage_ = false;

	// プレイヤーとの相対座標をセット
	info_.localPos_ = localPos;
	info_.velocity_.y = 0.0f;
}

void Item::EndGrabbed(void)
{
	// 掴まれていない状態にする
	info_.isGrabbed = false;

	// 離された瞬間の座標を取っておく
	info_.grabbedPos_ = trans_->pos_;

	MV1SetPosition(info_.modelId_, trans_->pos_);
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);

	// 重力を初期化する
	info_.velocity_.y = 0.0f;
}

void Item::Gravity(void)
{
	// 座標に重力を反映
	trans_->pos_.y += info_.velocity_.y;

	// 重力加算
	info_.velocity_.y += GRAVITY;

	// 最大落下速度
	if (info_.velocity_.y < MAX_FALL)
		info_.velocity_.y = MAX_FALL;

	// モデルの座標を反映
	MV1SetPosition(info_.modelId_, trans_->pos_);
}

void Item::TrackingPlayer(void)
{
	// プレイヤーがなかったら処理をしない
	if (player_ == nullptr)return;

	// 前の座標を保持しておく
	VECTOR prePos = trans_->pos_;

	// ローカル座標に
	trans_->pos_ = VAdd(*cameraPos_, ToCameraLocalPosRot());;

	// 線形補間で滑らかにする
	trans_->pos_ = Math::Lerp(prePos, trans_->pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(info_.modelId_, trans_->pos_);

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

	VECTOR start = trans_->pos_;
	start.y -= info_.collisionOffset_;
	VECTOR end = trans_->pos_;
	end.y += info_.collisionOffset_;

	// 当たり判定用のカプセル大きさ確認
	DrawCapsule3D(start, end, info_.collisionRadius_, 8, 0xff0000, 0xff0000, false);
}
