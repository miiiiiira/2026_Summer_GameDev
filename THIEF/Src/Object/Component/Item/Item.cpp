#include "Item.h"
#include <DxLib.h>
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/Math/Math.h"
#include "../Render/Render3D.h"
#include "../../Object.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "../../../Application.h"

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

	// 離された時の座標を初期化
	info_.grabbedPos_ = trans_->pos_;

	// 重力の初期化
	info_.velocity_ = VGet(0.0f, 0.0f, 0.0f);

	// 初めは掴まれていない状態にする
	info_.isGrabbed_ = false;

	// 地面に接触していることにする
	info_.hasTouchedStage_ = true;

	// 納品場所に入っていない状態にする
	info_.hasTouchedDeliveryLocation_ = false;

	// 無敵時間を初期化しておく
	info_.invincibilityFrames_ = INVINCIBILITY_FRAMES;

	// 個々のパラメータを設定
	SetParam();
}

void Item::Update(void)
{
	// ダメージ表記用のカウントを進める
	CountUpdate();

	// 生存していなかったら描画しない
	if (!info_.isAlive_)return;

	// 無敵時間の更新処理
	UpdateInvincibility();

	// 掴まれていたら
	if (info_.isGrabbed_)
	{
		if (info_.hasTouchedStage_)
		{
			// 空中状態で一度もステージに接触していないとする
			info_.hasTouchedStage_ = false;
		}

		// プレイヤーの位置を見て移動処理を行う
		TrackingPlayer();

		// アイテムの重さを反映
		Weight();
	}
	else
	{

		// 重力をかける
		Gravity();

	}
}

void Item::Draw2D(void)
{
	// 生存していなかったら描画しない
	if (!info_.isAlive_)return;

	if (info_.isGrabbed_)
	{
		VECTOR moneyPos = ConvWorldPosToScreenPos(trans_->pos_);

		// お金表示
		DrawFormatStringF(moneyPos.x, moneyPos.y, 0xff0000, "%d円", info_.money_);
	}

	for (const DamageInfo damage : damageDrawList_)
	{
		VECTOR pos = ConvWorldPosToScreenPos(damage.pos);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, damage.count % 255);
		DrawFormatStringToHandle(
			pos.x,
			pos.y, 
			0xff0000, 
			Application::GetInstance()->GetFont(), 
			"-%d",
			damage.damage);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void Item::Draw3D(void)
{
	// 生存していなかったら描画しない
	if (!info_.isAlive_)return;

	// デバッグ表示
	DrawDebug();
}

Transform* Item::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

const ItemInfo& Item::GetInfo(void)
{
	return info_;
}

float Item::GetCameraDistance(VECTOR pos)
{
	return VSize(VSub(pos, CameraUtility::GetCameraPos()));
}

void Item::SetDamage(int damage,VECTOR pos)
{
	// 指定のダメージから頑丈さ分引いた数値を実際に与えるダメージとする
	int dmg = damage - info_.hardness_;

	// ダメージがマイナス値だったらHPに変更を行わない(回復してしまうため)
	if (dmg < 0)return;

	// ダメージ表記用に情報を保持しておく
	damageDrawList_.push_back({ pos,damage,DAMAGE_DRAW_COUNT });

	// 残高にダメージを反映させる
	info_.money_ -= dmg;

	// 無敵時間を初期化
	info_.invincibilityFrames_ = INVINCIBILITY_FRAMES_ISGRABB;

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
	info_.isGrabbed_ = true;

	// プレイヤーとの相対座標をセット
	info_.localPos_ = localPos;
	info_.velocity_.y = 0.0f;
}

void Item::EndGrabbed(void)
{
	// 掴まれていない状態にする
	info_.isGrabbed_ = false;

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

void Item::Weight(void)
{
	// 座標にアイテムの重さを反映
	trans_->pos_.y += info_.weight_;

	// モデルの座標を反映
	MV1SetPosition(info_.modelId_, trans_->pos_);
}

void Item::TrackingPlayer(void)
{
	// 前の座標を保持しておく
	VECTOR prePos = trans_->pos_;

	// ローカル座標に
	trans_->pos_ = CameraUtility::AddCameraPosLocalPos(info_.localPos_);

	// 線形補間で滑らかにする
	trans_->pos_ = Math::Lerp(prePos, trans_->pos_, COEFFICIENT);

	// モデルに座標を反映
	MV1SetPosition(info_.modelId_, trans_->pos_);

	//// 相手へのベクトルを計算(引き算)
	//VECTOR vec = VSub(CameraUtility::GetCameraPos(),trans_->pos_);

	//// ベクトルの正規化で単位ベクトル(方向)を取得する
	//float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

	//if (length == 0.0f)
	//{
	//	return;
	//}

	//// 大きさで割って単位ベクトルにする
	//VECTOR dir = VGet( 0.0f,0.0f,0.0f );
	//dir.x = vec.x / length;
	//dir.x = vec.y / length;
	//dir.z = vec.z / length;

	//// 方向から角度を出す
	//info_.angle_.x = atan2(dir.y, dir.z);
	//info_.angle_.y = atan2(dir.x, dir.z);
	//info_.angle_.z = atan2(dir.x, dir.y);

	// アイテムの回転を行列にする
	MATRIX itemMat = Matrix::GetMatrixRotateXYZ(info_.angle_);

	// プレイヤーの回転を杖のの回転行列に反映する
	MATRIX mat = Matrix::Multiplication(itemMat, CameraUtility::GetCameraMatrix());
	
	// 回転行列をモデルに反映
	MV1SetRotationMatrix(info_.modelId_, mat);

	// 当たり判定情報を最新の状態に更新
	MV1RefreshCollInfo(info_.modelId_, -1);
}

void Item::UpdateInvincibility(void)
{
	// 掴まれていないかつ無敵時間が初期化されていなかったら
	if (!info_.isGrabbed_
		&& info_.invincibilityFrames_ <= 0)
	{
		// 無敵時間を初期化
		info_.invincibilityFrames_ = INVINCIBILITY_FRAMES;
	}

	// 最低値まで行ったら処理を行わない
	if (info_.invincibilityFrames_ < 0)return;

	// フレーム数がデフォルトより小さければ持っていると判断する
	if (info_.invincibilityFrames_ > 0)
	{
		// 無敵時間を縮める
		info_.invincibilityFrames_--;
	}
}

void Item::CountUpdate(void)
{
	for (auto it = damageDrawList_.begin(); it != damageDrawList_.end();)
	{
		// カウントを減らす
		--it->count;

		if (it->count < 0)
		{
			// eraseは削除した次の要素のイテレータを返すので、それをitに代入する
			it = damageDrawList_.erase(it);
		}
		else
		{
			// 削除しなかった場合だけ、次の要素へ進める
			++it;
		}
	}
}

void Item::DrawDebug(void)
{
#ifdef _DEBUG

	VECTOR start = trans_->pos_;
	start.y -= info_.collisionRadiusY_;
	VECTOR end = trans_->pos_;
	end.y += info_.collisionRadiusY_;

	// 当たり判定用のカプセル大きさ確認
	DrawCapsule3D(start, end, info_.collisionRadiusX_, 8, 0xff0000, 0xff0000, false);
#endif // _DEBUG

}
