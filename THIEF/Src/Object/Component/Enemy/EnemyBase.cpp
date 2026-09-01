#include <queue>

#include "../../../Application.h"
#include "../../Object.h"

#include "../../../Common/Math/Math.h"
#include "../PlayerController/PlayerController.h"
#include "../Collider/3DCollider/CapsuleCollider.h"
#include "../Collider/StageCollider/StageCollider.h"
#include "../Transform/Transform.h"
#include "../Animation/Animation.h"
#include "../Render/Render3D.h"

#include "StagePathData.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
	:
	transform_(nullptr),
	capColl_(nullptr),
	stageColl_(nullptr),
	anim_(nullptr)
{
	info_.moveSpeed_ = 0.0f;
	info_.moveDir_ = Math::VECTOR_ZERO;
	info_.movePow_ = Math::VECTOR_ZERO;
	info_.isAlive_ = true;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
	// オーナーからTransform取得
	transform_ = owner_->GetComponent<Transform>();

	// オーナーからCapsuleCollider取得
	capColl_ = owner_->GetComponent<CapsuleCollider>();

	// オーナーからStageCollider取得
	stageColl_ = owner_->GetComponent<StageCollider>();

	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();

	anim_ = owner_->GetComponent<Animation>();
	if (!anim_)
	{
		// 存在しない場合のみ追加する安全対策
		anim_ = owner_->AddComponent<Animation>();
	}
	if (render)
	{
		// モデルIDが存在する場合のみ取得
		info_.modelId_ = render->GetHandle();
	}
}

void EnemyBase::Draw3D(void)
{
#ifdef _DEBUG
	// デバッグ表示
		VECTOR start = capColl_->GetStart();
		VECTOR end = capColl_->GetEnd();

		// 当たり判定用のカプセル大きさ確認
		DrawCapsule3D(start, end, info_.radius_, 8, 0xff0000, 0xff0000, false);

#endif // _DEBUG
}

void EnemyBase::Draw2D(void)
{
}

void EnemyBase::SetPathData(PlayerController* player, int stageId, std::shared_ptr<StagePathData> pathData)
{
	player_ = player;
	stageId_ = stageId;
	pathData_ = pathData;
}

void EnemyBase::SetEnemyData(const EnemyData& data)
{
	info_.startOffset_ = data.capStartOffset;
	info_.endOffset_ = data.capEndOffset;
	info_.radius_ = data.capRadius;
}

Transform* EnemyBase::GetTransform()
{
	return transform_;
}

CapsuleCollider* EnemyBase::GetCapsule(void)
{
	return capColl_;
}

WeaponBase* EnemyBase::GetWeapon(void)
{
	return useWeapon_;
}

float EnemyBase::GetAttackDamagePow(void) const
{
	return info_.attackDamagePow_;
}

float EnemyBase::GetAttackMoveSpeed(void) const
{
	return info_.attackMoveSpeed_;
}

float EnemyBase::GetAttackJumpPow(void) const
{
	return info_.attackJumpPow_;
}

ENEMY_TAG EnemyBase::GetTag(void) const
{
	return info_.tag_;
}

void EnemyBase::SetPos(VECTOR pos)
{
	transform_->pos_ = pos;
}

void EnemyBase::FindPath(int startNodeId, int goalNodeId)
{
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return;

	// StagePathDataからリストを取得
	const auto* wayList = pathData->GetWayList();
	const auto* edgeList = pathData->GetEdgeList();

	if (!wayList || !edgeList) return;

	info_.path_.clear();

	// 全て同じ値で埋め尽くす
	info_.minCosts_.assign(wayList->size(), FLT_MAX);
	info_.parentNodes_.assign(wayList->size(), -1);

	// スタート地点のコストは0にする
	info_.minCosts_[startNodeId] = 0.0f;

	std::priority_queue <std::pair<float, int>, 
						std::vector<std::pair<float, int>>,
						std::greater<std::pair<float, int>>> que;

	que.push({ 0.0f, startNodeId });

	while (!que.empty())
	{
		// 先頭を取得
		float currentCost = que.top().first;
		int currentNodeId = que.top().second;

		// 取り出したら、削除
		que.pop();

		// もし取り出したIDがゴールと一緒なら抜ける
		if (currentNodeId == goalNodeId) break;

		// 取り出したコストが、すでに minCosts_ にある最小コストより大きければスキップ
		if (currentCost > info_.minCosts_[currentNodeId]) continue;

		// つながっているエッジ
		for (const auto& edge : (*edgeList)[currentNodeId])
		{
			// 隣接しているノードIDを取得
			int nextNodeId = edge.way.id;

			// 現在のコスト+エッジのコストから新しいコストを計算する
			float newCost = currentCost + edge.cost;

			//　新しいコストが最小コストよりも小さかったら
			if (newCost < info_.minCosts_[nextNodeId])
			{
				// minCosts_とparentNodes_を更新
				info_.minCosts_[nextNodeId] = newCost;
				info_.parentNodes_[nextNodeId] = currentNodeId;

				// 新しいコストと隣のノードIDをペアにしてqueにプッシュする
				que.push({ newCost, nextNodeId });
			}
		}
	}

	int i = goalNodeId;
	while (i != -1)
	{
		StagePathData::EDGE path;
		path.way.id = (*wayList)[i].id;
		path.way.pos = (*wayList)[i].pos;
		path.cost = info_.minCosts_[i];

		info_.path_.push_back(path);

		i = info_.parentNodes_[i]; // 次の親ノードへ進む
	}

	// 逆にする
	std::reverse(info_.path_.begin(), info_.path_.end());
}

void EnemyBase::DelayRotate(void)
{
	if (!transform_) return;

	// 移動方向から角度に変換する
	float goal = atan2f(info_.moveDir_.x, info_.moveDir_.z);

	// 常に最短経路で補間
	transform_->angle_.y = Math::LerpAngle(transform_->angle_.y, goal, 0.2f);
}

void EnemyBase::LookPlayer(void)
{
	if (!transform_ || !player_) return;

	// プレイヤー（相手）の座標を取得
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 相手へのベクトルを計算
	VECTOR diff = VSub(playerPos, transform_->pos_);
	diff.y = 0.0f;

	// ベクトルの正規化で単位ベクトル（方向）を取得
	info_.moveDir_ = VNorm(diff);

	// 回転はY軸のみ
	transform_->angle_.x = transform_->angle_.z = 0.0f;
}

void EnemyBase::Move(void)
{
	if (!transform_) return;

	// 移動量を計算する
	info_.movePow_ = VScale(info_.moveDir_, info_.moveSpeed_);
	// 移動量処理
	transform_->pos_ = VAdd(transform_->pos_, info_.movePow_);
}

float EnemyBase::GetDistance(VECTOR pos1, VECTOR pos2)
{
	return VSquareSize(VSub(pos1, pos2));
}

bool EnemyBase::CheckPlayerDiscovery(float radius)
{
	if (!transform_ || !player_) return false;

	// プレイヤーの位置
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 敵とプレイヤーの直線距離をチェック
	float distance = GetDistance(playerPos, transform_->pos_);
	if (distance > radius * radius) return false;

	// 高低差チェック
	float pos = fabsf(playerPos.y - transform_->pos_.y);
	if (pos > 50.0f) return false;

	// 敵の正面方向ベクトルを計算
	VECTOR dirEnemy = VECTOR();
	if (VSize(info_.moveDir_) < 0.001f)
	{
		// 移動していない場合は現在の向きから正面を計算
		dirEnemy.x = sinf(transform_->angle_.y);
		dirEnemy.y = 0.0f;
		dirEnemy.z = cosf(transform_->angle_.y);
	}
	else
	{
		// 移動中の場合は移動方向を正面とする
		dirEnemy = VNorm(info_.moveDir_);
	}

	// 敵からプレイヤーへの方向ベクトルを計算
	VECTOR diff = VSub(playerPos, transform_->pos_);
	dirEnemy.y = 0.0f;
	diff.y = 0.0f;
	dirEnemy = VNorm(dirEnemy);
	VECTOR dirPlayerForEnemy = VNorm(diff);

	// 内積を使ってベクトルの比較
	float dot = VDot(dirEnemy, dirPlayerForEnemy);
	float angle = acosf(dot);

	// 敵の視野角
	const float viweRad = Math::Deg2Rad(30.0f);

	// 視野内にいるか確認
	if (angle <= viweRad)
	{
		// プレイヤーの頭上位置
		VECTOR playerOffsetStart = player_->GetCapsule()->GetStart();

		// 敵の頭上位置
		VECTOR enemyOffsetStart = capColl_->GetStart();

		// 頭上同士を結ぶ直線上にステージがあるか
		MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, enemyOffsetStart, playerOffsetStart, info_.radius_);

		bool hit = (res.HitNum > 0);
		MV1CollResultPolyDimTerminate(res);

		// 障害物に当たらなかったら、目線が通っているとみなす
		if (!hit)
		{
			info_.isNotice_ = true;
			return true;
		}
	}

	// 視野外、または障害物に遮られている場合
	info_.isNotice_ = false;
	return false;
}

bool EnemyBase::IsPlayerInArea(VECTOR minPos, VECTOR maxPos)
{
	if (!player_) return false;

	bool ret = false;

	VECTOR playerPos = player_->GetTransform()->pos_;

	// プレイヤーの座標が、エリアの最大、最小の中に収まっているか
	bool isInsideX = (playerPos.x >= minPos.x && playerPos.x <= maxPos.x);
	bool isInsideY = (playerPos.y >= minPos.y && playerPos.y <= maxPos.y);
	bool isInsideZ = (playerPos.z >= minPos.z && playerPos.z <= maxPos.z);

	// プレイヤーがエリア内なら
	if (isInsideX && isInsideY && isInsideZ)
	{
		ret = true;
	}
	return ret;
}

void EnemyBase::Jump(void)
{
	if (!stageColl_) return;

	// ジャンプ力を設定
	info_.velocityY_ = JUMP_POW;

	// 接地フラグを折る
	stageColl_->IsGroundFold();

	// ジャンプ音
	//AudioManager::GetInstance()->PlaySE(SoundID::SE_JUMP);
}

void EnemyBase::ApplyGravity()
{
	// ステージコライダが無ければ処理を行わない
	if (!stageColl_) return;

	// Y座標へ反映
	transform_->pos_.y += info_.velocityY_;

	if (!stageColl_->IsGround())
	{
		// 重力加算
		info_.velocityY_ += GRAVITY;

		// 最大落下速度
		if (info_.velocityY_ < MAX_FALL)
			info_.velocityY_ = MAX_FALL;
	}
	else
	{
		info_.velocityY_ = -0.1f;
	}
}

void EnemyBase::SetMoveDirPatrol(void)
{
	VECTOR tmpPos = info_.nextWayPoint_;
	tmpPos.y = 0.0f;

	VECTOR pos = transform_->pos_;
	pos.y = 0.0f;

	info_.moveDir_ = VNorm(VSub(tmpPos, pos));
}

void EnemyBase::ArriveNode(void)
{
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return;

	// 次のノードを選ぶ
	int nextId = SelectNextNode();

	// 履歴を更新する
	info_.prevPrevNodeId_ = info_.prevNodeId_;
	info_.prevNodeId_ = info_.currentNodeId_;
	info_.currentNodeId_ = nextId;

	// 次の目的地の座標を設定する
	const auto* wayList = pathData->GetWayList();
	info_.nextWayPoint_ = (*wayList)[info_.currentNodeId_].pos;
}

int EnemyBase::SelectNextNode(void)
{	
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return info_.currentNodeId_;

	const auto* edgeList = pathData->GetEdgeList();
	if (!edgeList || info_.currentNodeId_ < 0 || info_.currentNodeId_ >= static_cast<int>(edgeList->size()))
	{
		return info_.currentNodeId_;
	}

	// 有効ノードを探す前に空にする
	info_.candidates_.clear();

	for (const auto& edge : (*edgeList)[info_.currentNodeId_])
	{
		int nextId = edge.way.id;

		float distance = GetDistance(edge.way.pos, transform_->pos_);

		// 敵の座標から半径以内に無いポイントは除外
		if (distance > info_.patrolRadius_ * info_.patrolRadius_) continue;

		// 前回、前々回のノードは除外する
		if (nextId == info_.prevNodeId_) continue;
		if (nextId == info_.prevPrevNodeId_) continue;

		info_.candidates_.push_back(nextId);
	}

	// 候補があった場合ランダムに選ぶ
	if (!info_.candidates_.empty())
	{
		int index = GetRand(static_cast<int>(info_.candidates_.size() - 1));
		return info_.candidates_[index];
	}

	if (info_.prevNodeId_ != -1)
	{
		return info_.prevNodeId_;
	}

	return info_.currentNodeId_;
}

int EnemyBase::FindNearestNode(VECTOR pos)
{
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return 0;

	const auto* wayList = pathData->GetWayList();
	if (!wayList || wayList->empty()) return 0;

	int nearNodeId = -1;
	float minCost = FLT_MAX;

	// 視線が通っているノードの中で
	for (const auto& way : (*wayList))
	{
		// 一番近いノードを探す
		float distance = VSize(VSub(pos, way.pos));

		if (distance < minCost)
		{
			minCost = distance;
			nearNodeId = way.id;
		}
	}

	if (nearNodeId == -1)
	{
		nearNodeId = 0;
	}

	return nearNodeId;
}

void EnemyBase::ChaseNode(void)
{
	info_.nextWayPoint_ = info_.path_[info_.nextNodeId_].way.pos;
	// 移動方向を設定
	SetMoveDirPatrol();

	VECTOR enemyPos = transform_->pos_;
	enemyPos.y = 0.0f;

	VECTOR targetNode = info_.path_[info_.nextNodeId_].way.pos;
	targetNode.y = 0.0f;

	// 水平方向の純粋な距離を測る
	float nodeDistance = VSize(VSub(targetNode, enemyPos));

	if (nodeDistance < 60.0f)
	{
		info_.nextNodeId_++;
	}
}

void EnemyBase::ChaseDirect(void)
{
	LookPlayer();
}

bool EnemyBase::CheckChaseLineCollision(VECTOR pPos, VECTOR ePos, float radius)
{
	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, pPos, ePos, radius);

	bool isHit = (res.HitNum > 0);

	// 必ず最後に Terminate を呼んでから結果を返す
	MV1CollResultPolyDimTerminate(res);

	return isHit;
}