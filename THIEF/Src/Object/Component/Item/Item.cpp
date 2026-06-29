#include "Item.h"
#include <DxLib.h>
#include <math.h>
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/Math/Math.h"
#include "../Render/Render3D.h"
#include "../../Object.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "../../../Application.h"

Item::~Item(void)
{
	damageDrawList_.clear();

	// フォントの削除
	DeleteFontToHandle(edgeFont_);
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

	// まだ見つかっていないことにする
	info_.isFound_ = false;

	// 個々のパラメータを設定
	SetParam();

	damageDrawList_.clear();

	edgeFont_ = CreateFontToHandle("Shikakufuto_Free", FONT_SIZE, 1, DX_FONTTYPE_ANTIALIASING);
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

	// 一定の座標いったら
	if (trans_->pos_.y < DEAD_POS_Y)
	{
		// 0初期化
		info_.price_ = 0;

		// 生存フラグを折る
		info_.isAlive_ = false;

		// 描画フラグを折る
		auto render = owner_->GetComponent<Render3D>();
		render->SetIsDraw(false);

		// 壊れた瞬間の処理
		Break();
		return;
	}
}

void Item::Draw2D(void)
{
	int price = info_.price_;
	int offset = 0;

	// 金額が0円の場合は1桁として扱う
	if (price == 0)
	{
		offset = 1;
	}
	else
	{
		while (price > 0)
		{
			price /= 10;
			offset++;
		}
	}

	// 生存していなかったら描画しない
	if (info_.isAlive_)
	{
		if (info_.isGrabbed_)
		{
			VECTOR pricePos = ConvWorldPosToScreenPos(trans_->pos_);

			// お金表示
			// 縁
			DrawFormatStringFToHandle(
				pricePos.x - ((offset * FONT_SIZE) / 2),
				pricePos.y,
				0x000000,
				edgeFont_,
				"%d",
				info_.price_);

			DrawFormatStringFToHandle(
				pricePos.x - ((offset * FONT_SIZE) / 2),
				pricePos.y,
				0x00ff00,
				Application::GetInstance()->GetFont(),
				"%d",
				info_.price_);
		}
	}

	for (const DamageInfo damage : damageDrawList_)
	{
		// ダメージの場所が視界内に入っていないのであれば処理をスキップ
		if (CheckCameraViewClip(damage.pos))continue;

		// ワールド座標をスクリーン座標にする
		VECTOR pos = ConvWorldPosToScreenPos(damage.pos);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (255 * damage.count) / DAMAGE_DRAW_COUNT);
		DrawFormatStringFToHandle(
			pos.x - ((offset * Application::FONT_SIZE) / 2),
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

float Item::GetCameraDistance(void)
{
	// アイテムの座標とカメラの座標のベクトルを求めて、距離を算出
	return VSize(VSub(trans_->pos_, CameraUtility::GetCameraPos()));
}

void Item::SetDamage(VECTOR pos)
{
	//　無敵時間があったら処理をしない
	if (info_.invincibilityFrames_ > 0)return;

	// 掴まれて離されて、一度も設置していないなら処理をしない
	if (info_.hasTouchedStage_)return;

	// 納品場所にはいっているなら処理をしない
	if (info_.hasTouchedDeliveryLocation_)return;

	int damage = 0;

	// アイテムが掴まれていたら
	if (info_.isGrabbed_)
	{
		damage = static_cast<int>(VSize(VSub(pos, trans_->prevPos_)));

		damage *= DAMAGE_MULT;
		// 指定のダメージから頑丈さ分引いた数値を実際に与えるダメージとする
		int dmg = damage - info_.hardness_;
	}

	// 掴まれていないかつ、空中状態から1度も設置していなかったら
	if (!info_.isGrabbed_ && !info_.hasTouchedStage_)
	{
		damage = static_cast<int>(VSize(VSub(info_.grabbedPos_, pos)));

		// 設置したためフラグを接触フラグを立てる
		info_.hasTouchedStage_ = true;
	}

	// ダメージがマイナス値だったらHPに変更を行わない(回復してしまうため)
	if (damage <= 0)return;

	// ダメージ表記用に情報を保持しておく
	if (damage <= info_.price_)
	{
		damageDrawList_.push_back({ pos,damage,DAMAGE_DRAW_COUNT });
	}
	else
	{
		damageDrawList_.push_back({ pos,info_.price_,DAMAGE_DRAW_COUNT });
	}

	// 残高にダメージを反映させる
	info_.price_ -= damage;

	// 無敵時間を初期化
	info_.invincibilityFrames_ = INVINCIBILITY_FRAMES_ISGRABB;

	// お金が0以下になったら
	if (info_.price_ <= 0)
	{
		// 0初期化
		info_.price_ = 0;

		// 生存フラグを折る
		info_.isAlive_ = false;

		// 描画フラグを折る
		auto render = owner_->GetComponent<Render3D>();
		render->SetIsDraw(false);

		// 壊れた瞬間の処理
		Break();
	}
	else
	{
		// 個々のダメージ時の処理
		Damage();
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

	// 空中状態で一度もステージに接触していないとする
	info_.hasTouchedStage_ = false;
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

	// XとZの比率から、Y軸の角度を直接計算する
	float forwardX = CameraUtility::GetCameraMatrix().m[2][0];
	float forwardZ = CameraUtility::GetCameraMatrix().m[2][2];
	// ベクトルから角度を出す
	info_.angle_.y = atan2f(forwardX, forwardZ);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(info_.modelId_, CameraUtility::GetCameraMatrix());

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
		info_.invincibilityFrames_ = 0;
	}

	// 掴んでないなら処理をしない
	if (!info_.isGrabbed_)return;

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
