
#include <algorithm>
#include <queue>
#include "../../Common/AnimationController.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "Weapon/WeaponPunch.h"
#include "../../Component/PlayerController/PlayerController.h"
#include "../../Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../Component/Transform/Transform.h"
#include "EnemyManager.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase(int modelId)
	:
	animationController_(nullptr),
	modelId_(modelId)
{
	currentNodeId_ = 0;
	nextNodeId_ = 0;
	prevNodeId_ = -1;
	prevPrevNodeId_ = -1;
	targetLostTimer_ = 0.0f;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Load(void)
{
}

void EnemyBase::Init(PlayerController* player, int stageId, const std::vector<EnemyCommon::WAYPOINT>& way, const std::vector<std::vector<EnemyCommon::EDGE>>& edgeList)
{
	player_ = player;
	stageId_ = stageId;
	way_ = &way;
	edgeList_ = &edgeList;

	path_.reserve(way_->size());
	minCosts_.reserve(way_->size());
	parentNodes_.reserve(way_->size());

	OnInitialize();
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);

#ifdef _DEBUG
	MATRIX mat = MGetIdent();
	mat = Matrix::GetMatrixRotateXYZ(angle_);

	const VECTOR dirForwardBase = VGet(0.0f, 0.0f, 1.0f);

	// 前方方向
	VECTOR forward = VTransform(dirForwardBase, mat);

	// 右
	MATRIX rightMat = MMult(mat, MGetRotY(Math::Deg2Rad(30.0f)));
	VECTOR right = VTransform(dirForwardBase, rightMat);
	// 左
	MATRIX leftMat = MMult(mat, MGetRotY(Math::Deg2Rad(-30.0f)));
	VECTOR left = VTransform(dirForwardBase, leftMat);

	VECTOR pos0 = pos_;

	VECTOR pos1 = VAdd(pos0, VScale(forward, 1000.0f));
	VECTOR pos2 = VAdd(pos0, VScale(left, 1000.0f));
	VECTOR pos3 = VAdd(pos0, VScale(right, 1000.0f));

	pos0.y = pos1.y = pos2.y = pos3.y = 10.0f;

	if (isNotice_)
	{
		DrawTriangle3D(pos0, pos2, pos1,
			0xcc44cc, true);

		DrawTriangle3D(pos0, pos1, pos3,
			0xcc44cc, true);
	}
	else
	{
		DrawTriangle3D(pos0, pos2, pos1,
			0x00ff00, true);

		DrawTriangle3D(pos0, pos1, pos3,
			0x00ff00, true);
	}

#endif //_DEBUG
}

void EnemyBase::Release(void)
{
	MV1DeleteModel(modelId_);

	if (animationController_ != nullptr)
	{
		animationController_->Release();
		delete animationController_;
		animationController_ = nullptr;
	}

	if (weaponPunch_ != nullptr)
	{
		weaponPunch_->Release();
		delete weaponPunch_;
		weaponPunch_ = nullptr;
	}

	if (useWeapon_ != nullptr)
	{
		useWeapon_ = nullptr;
	}
}

void EnemyBase::FindPath(int startNodeId, int goalNodeId)
{
	path_.clear();

	// 全て同じ値で埋め尽くす
	minCosts_.assign(way_->size(), FLT_MAX);
	parentNodes_.assign(way_->size(), -1);

	// スタート地点のコストは0にする
	minCosts_[startNodeId] = 0.0f;

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
		if (currentCost > minCosts_[currentNodeId]) continue;

		// つながっているエッジ
		for (const auto& edge : (*edgeList_)[currentNodeId])
		{
			// 隣接しているノードIDを取得
			int nextNodeId = edge.way.id;

			// 現在のコスト+エッジのコストから新しいコストを計算する
			float newCost = currentCost + edge.cost;

			//　新しいコストが最小コストよりも小さかったら
			if (newCost < minCosts_[nextNodeId])
			{
				// minCosts_とparentNodes_を更新
				minCosts_[nextNodeId] = newCost;
				parentNodes_[nextNodeId] = currentNodeId;

				// 新しいコストと隣のノードIDをペアにしてqueにプッシュする
				que.push({ newCost, nextNodeId });
			}
		}
	}

	int i = goalNodeId;
	while (i != -1)
	{
		EDGE path;
		path.way.id = (*way_)[i].id;   // 「現在のノードID」を正しく登録する
		path.way.pos = (*way_)[i].pos; // ★座標の設定漏れもここで修正！
		path.cost = minCosts_[i];

		path_.push_back(path);

		i = parentNodes_[i]; // 次の親ノードへ進む
	}

	// 逆にする
	std::reverse(path_.begin(), path_.end());
}

WeaponBase* EnemyBase::GetUseWeapon(void)
{
	return useWeapon_;
}

VECTOR EnemyBase::GetPos(void)
{
	return pos_;
}

void EnemyBase::SetPos(VECTOR pos)
{
	pos_ = pos;
	MV1SetPosition(modelId_, pos_);
	// 当たり判定情報を最新の状態に更新
	MV1RefreshCollInfo(modelId_, -1);
}

float EnemyBase::GetRadius(void)
{
	return radius_;
}

VECTOR EnemyBase::GetPrevPos(void)
{
	return prevPos_;
}

VECTOR EnemyBase::GetStart(void)
{
	return startOffset_;
}

VECTOR EnemyBase::GetEnd(void)
{
	return endOffset_;
}

void EnemyBase::SetGround(bool isGround)
{
	isGround_ = isGround;
}

float EnemyBase::GetVelocity(void)
{
	return velocityY_;
}

void EnemyBase::SetVelocity(float velocityY)
{
	velocityY_ = velocityY;
}

void EnemyBase::SetTag(ENEMY_TAG tag)
{
	tag_ = tag;
}

ENEMY_TAG EnemyBase::GetTag(void)
{
	return tag_;
}

void EnemyBase::DelayRotate(void)
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angle_.y = Math::LerpAngle(angle_.y, goal, 0.2f);
}

void EnemyBase::LookPlayer(void)
{
	// プレイヤー（相手）の座標を取得
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 相手へのベクトルを計算
	VECTOR diff = VSub(playerPos, pos_);
	diff.y = 0.0f;

	// ベクトルの正規化で単位ベクトル（方向）を取得
	moveDir_ = VNorm(diff);

	// 回転はY軸のみ
	angle_.x = angle_.z = 0.0f;
}

void EnemyBase::Move(void)
{
	// 移動量を計算する
	movePow_ = VScale(moveDir_, moveSpeed_);
	// 移動量処理
	pos_ = VAdd(pos_, movePow_);
	// モデルに座標を設定
	MV1SetPosition(modelId_, pos_);
}

float EnemyBase::GetDistance(VECTOR pos1, VECTOR pos2)
{
	return VSquareSize(VSub(pos1, pos2));
}

bool EnemyBase::CheckPlayerDiscovery(float radius)
{
	// プレイヤーの位置
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 敵とプレイヤーの直線距離をチェック
	float distance = GetDistance(playerPos, pos_);
	if (distance > radius * radius) return false;

	// 高低差チェック
	float pos = fabsf(playerPos.y - pos_.y);
	if (pos > 50.0f) return false;

	// 敵の正面方向ベクトルを計算
	VECTOR dirEnemy = VECTOR();
	if (VSize(moveDir_) < 0.001f)
	{
		// 移動していない場合は現在の向きから正面を計算
		dirEnemy.x = sinf(angle_.y);
		dirEnemy.y = 0.0f;
		dirEnemy.z = cosf(angle_.y);
	}
	else
	{
		// 移動中の場合は移動方向を正面とする
		dirEnemy = VNorm(moveDir_);
	}

	// 敵からプレイヤーへの方向ベクトルを計算
	VECTOR diff = VSub(playerPos, pos_);
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
		VECTOR enemyPos = VAdd(pos_, startOffset_);

		// 頭上同士を結ぶ直線上にステージがあるか
		MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, enemyPos, playerOffsetStart, radius_);

		// 障害物に当たらなかったら、目線が通っているとみなす
		if (res.HitNum <= 0)
		{
			MV1CollResultPolyDimTerminate(res);
			isNotice_ = true;
			return true;
		}
		MV1CollResultPolyDimTerminate(res);
	}

	// 視野外、または障害物に遮られている場合
	isNotice_ = false;
	return false;
}

bool EnemyBase::IsPlayerInArea(VECTOR minPos, VECTOR maxPos)
{
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
	if (!stageId_) return;

	// ジャンプ音
	//AudioManager::GetInstance()->PlaySE(SoundID::SE_JUMP);

	// ジャンプ力を設定
	velocityY_ = JUMP_POW;

	// 接地フラグを折る
	isGround_ = false;
}

void EnemyBase::ApplyGravity()
{
	if (stageId_ == -1) return;

	// Y座標へ反映
	pos_.y += velocityY_;

	if (!isGround_)
	{
		// 重力加算
		velocityY_ += GRAVITY;

		// 最大落下速度
		if (velocityY_ < MAX_FALL)
			velocityY_ = MAX_FALL;
	}
	else
	{
		velocityY_ = -0.1f;
	}
}

void EnemyBase::SetMoveDirPatrol(void)
{
	VECTOR tmpPos = nextWayPoint_;
	tmpPos.y = 0.0f;

	VECTOR pos = pos_;
	pos.y = 0.0f;

	moveDir_ = VNorm(VSub(tmpPos, pos));
}

void EnemyBase::ArriveNode(void)
{
	// 次のノードを選ぶ
	int nextId = SelectNextNode();

	// 履歴を更新する
	prevPrevNodeId_ = prevNodeId_;
	prevNodeId_ = currentNodeId_;
	currentNodeId_ = nextId;

	// 次の目的地の座標を設定する
	nextWayPoint_ = (*way_)[currentNodeId_].pos;
}

int EnemyBase::SelectNextNode(void)
{
	// 有効ノードを探す前に空にする
	candidates_.clear();

	for (const auto& edge : (*edgeList_)[currentNodeId_])
	{
		int nextId = edge.way.id;

		float distance = GetDistance(edge.way.pos, pos_);

		// 敵の座標から半径以内に無いポイントは除外
		if (distance > patrolRadius_ * patrolRadius_) continue;

		// 前回、前々回のノードは除外する
		if (nextId == prevNodeId_) continue;
		if (nextId == prevPrevNodeId_) continue;

		candidates_.push_back(nextId);
	}

	// 候補があった場合ランダムに選ぶ
	if (!candidates_.empty())
	{
		int index = GetRand(static_cast<int>(candidates_.size() - 1));
		return candidates_[index];
	}

	if (prevNodeId_ != -1)
	{
		return prevNodeId_;
	}

	return currentNodeId_;
}

int EnemyBase::FindNearestNode(VECTOR pos)
{
	int nearNodeId = -1;
	float minCost = FLT_MAX;

	// 視線が通っているノードの中で
	for (const auto& way : (*way_))
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
	nextWayPoint_ = path_[nextNodeId_].way.pos;
	// 移動方向を設定
	SetMoveDirPatrol();

	VECTOR enemyPos = pos_;
	enemyPos.y = 0.0f;

	VECTOR targetNode = path_[nextNodeId_].way.pos;
	targetNode.y = 0.0f;

	// 水平方向の純粋な距離を測る
	float nodeDistance = VSize(VSub(targetNode, enemyPos));

	if (nodeDistance < 60.0f)
	{
		nextNodeId_++;
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

	// 当たっていたら、trueを返す
	if (res.HitNum > 0)
	{
		MV1CollResultPolyDimTerminate(res);
		return true;
	}
	MV1CollResultPolyDimTerminate(res);
	return false;
}