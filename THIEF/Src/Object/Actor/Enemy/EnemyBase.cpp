#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#include "../../Common/AnimationController.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
{
	animationController_ = nullptr;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Load(void)
{
	LoadCsvData();

	path_.reserve(way_.size());
	minCosts_.reserve(way_.size());
	parentNodes_.reserve(way_.size());
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
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
}

void EnemyBase::FindPath(int startNodeId, int goalNodeId)
{
	path_.clear();

	// 全て同じ値で埋め尽くす
	minCosts_.assign(way_.size(), FLT_MAX);
	parentNodes_.assign(way_.size(), -1);

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

		// ③ 取り出したコストが、すでに minCosts_ にある最小コストより大きければスキップ
		if (currentCost > minCosts_[currentNodeId]) continue;

		// つながっているエッジ
		for (const auto& edge : edgeList_[currentNodeId])
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
		Edge path;
		path.way.id = way_[i].id;   // 「現在のノードID」を正しく登録する
		path.way.pos = way_[i].pos; // ★座標の設定漏れもここで修正！
		path.cost = minCosts_[i];

		path_.push_back(path);

		i = parentNodes_[i]; // 次の親ノードへ進む
	}

	// 逆にする
	std::reverse(path_.begin(), path_.end());
}

void EnemyBase::LoadCsvData(void)
{
	// 初期化
	way_.clear();
	std::ifstream ifs = std::ifstream("Data/PointSave.csv");

	if (!ifs) return;
	//ファイルを１行ずつ読み込む
	std::string line;		//1行の文字情報
	std::string c;			//1文字情報

	while (getline(ifs, line))
	{
		//1行情報　string を ifstream　の仲間に変換
		std::istringstream stream(line);
		int index = 0;
		int pointId = 0;
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;

		while (getline(stream, c, ','))
		{
			// 列のインデックスに応じて代入先を切り替える
			if (index == 0)
			{
				pointId = std::stoi(c);
			}
			else if (index == 1)
			{
				posX = std::stof(c);
			}
			else if (index == 2)
			{
				posY = std::stof(c);
			}
			else if (index == 3)
			{
				posZ = std::stof(c);
			}
			index++;
		}

		VECTOR position = VGet(posX, posY, posZ);

		Waypoint way = {};
		way.id = pointId;
		way.pos = position;

		way_.push_back(way);
	}
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
	VECTOR playerPos = *playerPos_;

	// 相手へのベクトルを計算
	VECTOR diff = VSub(playerPos, pos_);
	diff.y = 0.0f;

	// ベクトルの正規化で単位ベクトル（方向）を取得
	moveDir_ = VNorm(diff);

	// 回転はY軸のみ
	angle_.x = angle_.z = 0.0f;
}

float EnemyBase::GetDistance(VECTOR pos1, VECTOR pos2)
{
	return VSquareSize(VSub(pos1, pos2));
}

bool EnemyBase::CheckPlayerDiscovery(float radius)
{
	float distance = GetDistance(*playerPos_, pos_);

	if (distance > radius * radius) return false;

	if (fabsf(playerPos_->y - pos_.y) > 20.0f) return false;

	VECTOR dirEnemy = VNorm(moveDir_);
	VECTOR diff = VSub(*playerPos_, pos_);
	dirEnemy.y = 0.0f;
	diff.y = 0.0f;
	dirEnemy = VNorm(dirEnemy);
	VECTOR dirPlayerForEnemy = VNorm(diff);

	// 内積を使ってベクトルの比較
	float dot = VDot(dirEnemy, dirPlayerForEnemy);
	float angle = acosf(dot);

	const float viweRad = Math::Deg2Rad(40.0f);

	// 視野内にいるか確認
	if (angle <= viweRad)
	{
		MV1_COLL_RESULT_POLY hit = MV1CollCheck_Line(stageId_, -1, pos_, *playerPos_);

		if (!hit.HitFlag)
		{
			return true;
		}
	}

	return false;
}

void EnemyBase::AddEdge(int fromId, int toId)
{
	VECTOR posA = way_[fromId].pos;
	VECTOR posB = way_[toId].pos;

	float checkRadius = 10.0f;

	// カプセルとモデルの衝突判定
	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, posA, posB, checkRadius);

	// 当たっていたら、省く
	if (res.HitNum > 0)
	{
		// 後始末をする
		MV1CollResultPolyDimTerminate(res);
		return;
	}

	Edge edge = {};
	edge.way.id = way_[toId].id;	// 行った先
	edge.way.pos = way_[toId].pos;	// 行った先の座標
	// 行った先から行った元を引いて、VSizeでfloat型に変換
	edge.cost = VSize(VSub(posB, posA));

	edgeList_[fromId].push_back(edge);

	// 後始末をする
	MV1CollResultPolyDimTerminate(res);
}