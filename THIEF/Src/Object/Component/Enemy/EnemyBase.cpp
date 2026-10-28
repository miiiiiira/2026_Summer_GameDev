#include <queue>
#include <functional>
#include <algorithm>
#include <cfloat>

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

namespace
{
	// 重力加速度
	constexpr float GRAVITY = -1.98f;

	// 最大落下速度
	constexpr float MAX_FALL = -40.0f;

	// ノードに到達したと判定する距離
	constexpr float NODE_ARRIVE_DISTANCE = 60.0f;

	// 高低差による発見判定の閾値
	constexpr float MAX_DETECTION_HEIGHT_DIFFERENCE = 50.0f;

	// 最小移動速度の閾値
	constexpr float MIN_MOVE_SPEED_THRESHOLD = 0.001f;

	// 回転のLerpに使う割合
	constexpr float ROTATE_LERP_RATE = 0.2f;

	// 視野角
	constexpr float VIEW_ANGLE_DEG = 30.0f;

	// 接地を維持するために与える下向きの速度
	constexpr float GROUND_STICK_VELOCITY = 0.1f;
}

EnemyBase::EnemyBase(void)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Init(void)
{
	// コンポーネントの取得
	GetComponents();

	// テーブルから共通設定
	InitTableData();

	// 敵ごとのパラメータ設定
	InitParam();

	// アニメーションの登録
	InitAnimation();

	// スケール・向きの設定
	InitTransform();

	// 現在位置に一番近いノードから巡回を始める
	ResetToNearestNode();

	// 前回の座標を現在の座標に設定
	if (transform_)
	{
		transform_->prevPos_ = transform_->pos_;
	}

	// 個別の後処理
	InitPost();
}

void EnemyBase::Draw3D(void)
{
#ifdef _DEBUG
	// デバッグ表示
	VECTOR start = capColl_->GetStart();
	VECTOR end = capColl_->GetEnd();

	// 当たり判定用のカプセル大きさ確認
	DrawCapsule3D(
		start,
		end,
		info_.radius_,
		8,
		0xff0000,
		0xff0000,
		false);

#endif // _DEBUG
}

void EnemyBase::SetPlayer(PlayerController* player)
{
	player_ = player;		// プレイヤーのポインタを保持
}

void EnemyBase::SetPathData(int stageId, std::shared_ptr<StagePathData> pathData)
{
	stageId_ = stageId;		// ステージIDを保持
	pathData_ = pathData;	// パスデータを弱参照で保持
}

void EnemyBase::SetEnemyData(const EnemyData& data)
{
	// 敵のデータを保持
	info_.startOffset_ = data.capStartOffset_;	// カプセルコライダーの開始位置オフセット
	info_.endOffset_ = data.capEndOffset_;		// カプセルコライダーの終了位置オフセット
	info_.radius_ = data.capRadius_;			// カプセルコライダーの半径
}

void EnemyBase::AddAnimationAll(int animNum, float animSpeed)
{
	// Animationが存在しない場合は処理を中断
	if (!animation_) return;

	// アニメーションの初期化
	animation_->Init();

	// FBX内のアニメーションを番号順に登録
	for (int i = 0; i < animNum; i++)
	{
		animation_->AddInFbx(i, animSpeed, i);
	}
}

void EnemyBase::ApplyModelTransform(const VECTOR& scale, float forwardOffsetDeg)
{
	// モデルが存在しない場合は処理を中断
	if (info_.modelId_ == -1) return;

	// モデルの大きさを設定
	info_.scale_ = scale;
	MV1SetScale(info_.modelId_, info_.scale_);

	// モデルの正面補正を設定
	info_.localAngle_ = { 0.0f, Math::Deg2Rad(forwardOffsetDeg), 0.0f };
}

void EnemyBase::ResetToNearestNode(void)
{
	// Transformが存在しない場合は処理を中断
	if (!transform_) return;

	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();
	if (!pathData) return;

	// ウェイポイントのリストを取得
	const auto* wayList = pathData->GetWayList();
	if (!wayList || wayList->empty()) return;

	// 一番近いノードを現在ノードにし、履歴をリセット
	info_.currentNodeId_ = FindNearestNode(transform_->pos_);
	info_.prevNodeId_ = -1;
	info_.prevPrevNodeId_ = -1;

	// 次の目的地を現在ノードの座標に設定
	info_.nextWayPoint_ = (*wayList)[info_.currentNodeId_].pos_;
}

void EnemyBase::FindPath(int startNodeId, int goalNodeId)
{
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();

	// パスデータが存在しない場合、中断
	if (!pathData) return;

	// StagePathDataからリストを取得
	const auto* wayList = pathData->GetWayList();
	const auto* edgeList = pathData->GetEdgeList();

	// wayListまたはedgeListが存在しない場合、中断
	if (!wayList || !edgeList) return;

	// 経路情報をクリア
	info_.path_.clear();

	// 全ノードのコストを最大値、親を -1で初期化
	std::vector<float> minCosts(wayList->size(), FLT_MAX);
	std::vector<int> parentNodes(wayList->size(), -1);

	// スタート地点のコストは0にする
	minCosts[startNodeId] = 0.0f;

	// 優先度付きキューを使用して、コストの小さい順に探索する
	std::priority_queue<std::pair<float, int>,
						std::vector<std::pair<float, int>>,
						std::greater<std::pair<float, int>>> que;

	// スタート地点をキューに追加
	que.push({ 0.0f, startNodeId });

	// キューが空になるまで探索を続ける
	while (!que.empty())
	{
		// 先頭を取得
		float currentCost = que.top().first;
		int currentNodeId = que.top().second;

		// 取り出したら、削除
		que.pop();

		// もし取り出したIDがゴールと一緒なら抜ける
		if (currentNodeId == goalNodeId) break;

		// 取り出したコストが、すでに minCosts にある最小コストより大きければスキップ
		if (currentCost > minCosts[currentNodeId]) continue;

		// 現在のノードからつながっているエッジを調べる
		for (const auto& edge : (*edgeList)[currentNodeId])
		{
			// 隣接しているノードIDを取得
			int nextNodeId = edge.way_.id_;

			// 現在のコスト+エッジのコストから新しいコストを計算する
			float newCost = currentCost + edge.cost_;

			// 新しいコストが最小コストよりも小さかったら
			if (newCost < minCosts[nextNodeId])
			{
				minCosts[nextNodeId] = newCost;
				parentNodes[nextNodeId] = currentNodeId;

				// 新しいコストと隣のノードIDをペアにしてqueにプッシュする
				que.push({ newCost, nextNodeId });
			}
		}
	}

	// ゴールまでの経路を逆順にたどる
	int i = goalNodeId;

	// 親ノードが-1になるまでループ
	while (i != -1)
	{
		// 逆順にたどった経路をinfo_.path_に追加する
		StagePathData::Edge path = {};
		path.way_.id_ = (*wayList)[i].id_;
		path.way_.pos_ = (*wayList)[i].pos_;
		path.cost_ = minCosts[i];

		info_.path_.push_back(path);

		// 次の親ノードへ進む
		i = parentNodes[i];
	}

	// ゴールからたどったので、スタートからの順番に並べ替える
	std::reverse(info_.path_.begin(), info_.path_.end());
}

void EnemyBase::DelayRotate(void)
{
	// Transformが存在しない場合は処理を中断
	if (!transform_) return;

	// 移動方向がほぼゼロなら、今の向きを保つ
	if (!IsMoving()) return;

	// 移動方向から角度に変換する
	float goal = atan2f(info_.moveDir_.x, info_.moveDir_.z);

	// 常に最短経路で補間
	transform_->angle_.y = Math::LerpAngle(transform_->angle_.y, goal, ROTATE_LERP_RATE);
}

void EnemyBase::LookPlayer(void)
{
	// TransformまたはPlayerが存在しない場合は処理を中断
	if (!transform_ || !player_) return;

	// プレイヤーの座標を取得
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 相手へのベクトルを計算
	VECTOR diff = VSub(playerPos, transform_->pos_);

	// Y軸方向の回転のみを考慮するため、Y成分を0にする
	diff.y = 0.0f;

	// ベクトルの正規化で単位ベクトルを取得
	info_.moveDir_ = VNorm(diff);

	// 回転はY軸のみ
	transform_->angle_.x = 0.0f;
	transform_->angle_.z = 0.0f;
}

void EnemyBase::Move(void)
{
	// Transformが存在しない場合は処理を中断
	if (!transform_) return;

	// 移動量を計算する
	info_.movePow_ = VScale(info_.moveDir_, info_.moveSpeed_);

	// 移動量処理
	transform_->pos_ = VAdd(transform_->pos_, info_.movePow_);
}

bool EnemyBase::CheckPlayerDiscovery(float radius)
{
	// TransformまたはPlayerが存在しない場合は処理を中断
	if (!transform_ || !player_) return false;

	// プレイヤーの位置
	VECTOR playerPos = player_->GetTransform()->pos_;

	// 敵とプレイヤーの直線距離をチェック
	float distanceSQ = GetDistanceSQ(playerPos, transform_->pos_);

	// 半径の二乗と比較して、範囲外ならfalseを返す
	if (distanceSQ > radius * radius) return false;

	// 高低差チェック
	float heightDiff = fabsf(playerPos.y - transform_->pos_.y);

	// 高低差が閾値より大きければ、発見できないとする
	if (heightDiff > MAX_DETECTION_HEIGHT_DIFFERENCE) return false;

	// 敵の正面方向ベクトルを計算
	VECTOR dirEnemy = Math::VECTOR_ZERO;

	// 移動方向がほぼゼロの場合は、現在の角度から正面を計算する
	if (!IsMoving())
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
	const float viewAngleRad = Math::Deg2Rad(VIEW_ANGLE_DEG);

	// 視野内にいて、頭上同士の間に障害物がなければ発見
	if (angle <= viewAngleRad)
	{
		// 各キャラの頭上位置
		VECTOR playerHead = player_->GetCapsule()->GetStart();
		VECTOR enemyHead = capColl_->GetStart();

		if (!CheckChaseLineCollision(enemyHead, playerHead, info_.radius_)) return true;
	}

	// 視野外、または障害物に遮られている場合
	return false;
}

bool EnemyBase::IsPlayerInArea(VECTOR minPos, VECTOR maxPos)
{
	// Playerが存在しない場合は処理を中断
	if (!player_) return false;

	// プレイヤーの座標を取得
	VECTOR playerPos = player_->GetTransform()->pos_;

	// プレイヤーの座標が、エリアの最大、最小の中に収まっているか
	bool isInsideX = (playerPos.x >= minPos.x && playerPos.x <= maxPos.x);
	bool isInsideY = (playerPos.y >= minPos.y && playerPos.y <= maxPos.y);
	bool isInsideZ = (playerPos.z >= minPos.z && playerPos.z <= maxPos.z);

	// プレイヤーがエリア内なら
	if (isInsideX && isInsideY && isInsideZ) return true;

	return false;
}

void EnemyBase::ApplyGravity(void)
{
	// ステージコライダが無ければ処理を行わない
	if (!stageColl_) return;

	transform_->pos_.y += info_.velocityY_;

	// 接地していなければ重力を加算する
	if (!stageColl_->IsGround())
	{
		// 重力加算
		info_.velocityY_ += GRAVITY;

		// 最大落下速度
		if (info_.velocityY_ < MAX_FALL)
		{
			info_.velocityY_ = MAX_FALL;
		}
	}
	else
	{
		// 接地中は、坂や段差で接地判定が途切れないよう
		// わずかに下向きの速度を与える
		info_.velocityY_ = -GROUND_STICK_VELOCITY;
	}
}

void EnemyBase::SetMoveDirPatrol(void)
{
	// 次の目的地の座標を取得
	VECTOR targetPos = info_.nextWayPoint_;

	// Y座標を0にすることで、水平距離のみで移動方向を計算する
	targetPos.y = 0.0f;

	// 敵の座標を取得
	VECTOR pos = transform_->pos_;
	pos.y = 0.0f;

	// 移動方向を計算する
	info_.moveDir_ = VNorm(VSub(targetPos, pos));
}

void EnemyBase::ArriveNode(void)
{
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();

	// パスデータが存在しない場合は処理を中断
	if (!pathData) return;

	// 次のノードを選ぶ
	int nextId = SelectNextNode();

	// 履歴を更新する
	info_.prevPrevNodeId_ = info_.prevNodeId_;	// 前々回のノードを更新
	info_.prevNodeId_ = info_.currentNodeId_;	// 前回のノードを更新
	info_.currentNodeId_ = nextId;				// 現在のノードを更新

	// 次の目的地の座標を設定する
	const auto* wayList = pathData->GetWayList();
	info_.nextWayPoint_ = (*wayList)[info_.currentNodeId_].pos_;
}

int EnemyBase::SelectNextNode(void)
{
	// lock() して shared_ptr を一時的に取得
	auto pathData = pathData_.lock();

	// パスデータが存在しない場合は処理を中断
	if (!pathData) return info_.currentNodeId_;

	// StagePathDataからエッジリストを取得
	const auto* edgeList = pathData->GetEdgeList();

	// edgeListが存在しない、または現在のノードIDが範囲外の場合は現在のノードIDを返す
	if (!edgeList || info_.currentNodeId_ < 0 ||
		info_.currentNodeId_ >= static_cast<int>(edgeList->size()))
	{
		return info_.currentNodeId_;
	}

	// 次に向かうノードの候補
	std::vector<int> candidates;

	// 現在のノードに接続されているエッジをループする
	for (const auto& edge : (*edgeList)[info_.currentNodeId_])
	{
		// 接続されているノードIDを取得
		int nextId = edge.way_.id_;

		// 接続されているノードとの距離を計算
		float distanceSQ = GetDistanceSQ(edge.way_.pos_, transform_->pos_);

		// 敵の座標から半径以内に無いポイントは除外
		if (distanceSQ > info_.patrolRadius_ * info_.patrolRadius_) continue;

		// 前回、前々回のノードは除外する
		if (nextId == info_.prevNodeId_) continue;
		if (nextId == info_.prevPrevNodeId_) continue;

		// 候補に追加する
		candidates.push_back(nextId);
	}

	// 候補があった場合ランダムに選ぶ
	if (!candidates.empty())
	{
		int index = GetRand(static_cast<int>(candidates.size() - 1));

		return candidates[index];
	}

	// 候補が無い場合は、前回のノードに戻る
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

	// パスデータが存在しない場合は処理を中断
	if (!pathData) return 0;

	// StagePathDataからウェイリストを取得
	const auto* wayList = pathData->GetWayList();

	// wayListが存在しない、または空の場合は0を返す
	if (!wayList || wayList->empty()) return 0;

	// 最も近いノードのIDと距離を初期化
	int nearNodeId = -1;
	float minDistanceSQ = FLT_MAX;

	// 全ノードの中から、指定位置に一番近いノードを探す
	for (const auto& way : (*wayList))
	{
		// 指定位置とノードの距離
		float distanceSQ = GetDistanceSQ(pos, way.pos_);

		// 最小距離を更新
		if (distanceSQ < minDistanceSQ)
		{
			minDistanceSQ = distanceSQ;	// 最小距離を更新
			nearNodeId = way.id_;		// 最も近いノードのIDを更新
		}
	}

	// もし近いノードが見つからなかった場合は、0を返す
	if (nearNodeId == -1)
	{
		nearNodeId = 0;
	}

	return nearNodeId;
}

void EnemyBase::ChaseNode(void)
{
	// 次の目的地の座標を設定する
	info_.nextWayPoint_ = info_.path_[info_.nextNodeId_].way_.pos_;

	// 移動方向を設定
	SetMoveDirPatrol();

	VECTOR enemyPos = transform_->pos_;
	enemyPos.y = 0.0f;

	VECTOR targetNode = info_.path_[info_.nextNodeId_].way_.pos_;
	targetNode.y = 0.0f;

	// 水平方向の純粋な距離を測る
	float nodeDistance = VSize(VSub(targetNode, enemyPos));

	// ある程度近づいたら次のノードに進む
	if (nodeDistance < NODE_ARRIVE_DISTANCE)
	{
		info_.nextNodeId_++;
	}
}

void EnemyBase::ChaseDirect(void)
{
	// プレイヤーの方向を見る
	LookPlayer();
}

bool EnemyBase::CheckChaseLineCollision(VECTOR startPos, VECTOR endPos, float radius)
{
	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(
		stageId_,
		-1,
		startPos,
		endPos,
		radius);

	// 当たったかどうかを判定
	bool isHit = (res.HitNum > 0);

	// 必ず最後に Terminateを呼んでから結果を返す
	MV1CollResultPolyDimTerminate(res);

	return isHit;
}

void EnemyBase::ApplyModelRotation(void)
{
	// モデルまたはTransformが存在しないなら中断
	if (info_.modelId_ == -1) return;
	if (!transform_) return;

	// 敵の現在の向き＋ローカル回転補正
	VECTOR finalAngle = VAdd(transform_->angle_, info_.localAngle_);

	// モデルに回転をセット
	MV1SetRotationXYZ(info_.modelId_, finalAngle);
}

bool EnemyBase::IsMoving(void) const
{
	// 移動方向の大きさが閾値以上なら移動中とみなす
	return VSize(info_.moveDir_) >= MIN_MOVE_SPEED_THRESHOLD;
}

bool EnemyBase::IsPathFinished(void) const
{
	// 次に向かうノードの番号が経路の数以上なら、たどり終えている
	return info_.nextNodeId_ >= static_cast<int>(info_.path_.size());
}

void EnemyBase::StartPath(VECTOR startPos, VECTOR goalPos)
{
	// 開始地点と目的地それぞれに一番近いノードを探す
	int startNodeId = FindNearestNode(startPos);
	int goalNodeId = FindNearestNode(goalPos);

	// ノード間の経路を探す
	FindPath(startNodeId, goalNodeId);

	// 経路の0番目は自分に最も近いノードなので、
	// 2つ以上あれば1番目から向かう
	info_.nextNodeId_ = 0;

	if (info_.path_.size() > 1)
	{
		info_.nextNodeId_ = 1;
	}
}

void EnemyBase::ClearPath(void)
{
	// 経路と、次に向かうノードの番号をリセットする
	info_.path_.clear();
	info_.nextNodeId_ = 0;
}

void EnemyBase::GetComponents(void)
{
	// オーナーからTransform取得
	transform_ = owner_->GetComponent<Transform>();

	// オーナーからCapsuleCollider取得
	capColl_ = owner_->GetComponent<CapsuleCollider>();

	// オーナーからStageCollider取得
	stageColl_ = owner_->GetComponent<StageCollider>();

	// オーナーから3D描画コンポーネントを取得
	auto render = owner_->GetComponent<Render3D>();

	// オーナーからAnimationコンポーネントを取得
	animation_ = owner_->GetComponent<Animation>();

	if (!animation_)
	{
		// 存在しない場合のみ追加する安全対策
		animation_ = owner_->AddComponent<Animation>();
	}

	if (render)
	{
		// モデルIDが存在する場合のみ取得
		info_.modelId_ = render->GetHandle();
	}
}

void EnemyBase::InitTableData(void)
{
	// 派生クラスから自分の種類を取得
	ENEMY_TAG tag = GetTag();

	// テーブルからデータを取得して設定
	SetEnemyData(EnemyTable::Table.at(tag));

	// 移動方向を0にしておく
	info_.moveDir_ = Math::VECTOR_ZERO;
}