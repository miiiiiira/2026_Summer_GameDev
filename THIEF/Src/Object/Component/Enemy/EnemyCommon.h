#pragma once
#include <DxLib.h>
#include <unordered_map>
#include <string>
#include <vector>

#include "StagePathData.h"

// 敵の種類を表すタグ
enum class ENEMY_TAG
{
    YETI,
    MUSHNUB,
    SKELETON,
    STATUE,
    GIGGLE,
    MAX,
};

enum class ENEMY_SIDE
{
	RIGHT,
	LEFT,
};

// 敵生成時のオプションパラメータ
struct EnemySpawnParam
{
	VECTOR minAreaPos_ = { 0.0f, 0.0f, 0.0f };		// エリア最小座標（Mushnub, Statue等）
	VECTOR maxAreaPos_ = { 0.0f, 0.0f, 0.0f };		// エリア最大座標（Mushnub, Statue等）
	VECTOR chasePos_ = { 0.0f, 0.0f, 0.0f };		// 初期追跡・巡回座標
	VECTOR angle_ = { 0.0f, 0.0f, 0.0f };			// 初期角度
	ENEMY_SIDE side_ = ENEMY_SIDE::RIGHT;			// 左右の指定（Skeleton用）
};

// 敵のデータ
struct EnemyInfo
{
	// モデル情報
	int modelId_ = -1;						// モデルのハンドル
	VECTOR scale_ = { 1.0f, 1.0f, 1.0f };	// 拡大率
	VECTOR localAngle_ = {};				// モデルごとのローカル回転補正値

	ENEMY_TAG tag_ = ENEMY_TAG::MAX;        // 敵のタグ

	// ステータス
	int hp_ = 0;                     // 現在のHP
	float moveSpeed_ = 0.0f;         // 現在の移動速度
	bool isAlive_ = true;            // 生存フラグ
	int invincibleTime_ = 0;         // 無敵時間

	// 攻撃用パラメータ
	float attackMoveSpeed_ = 0.0f;   // 攻撃時の移動速度
	float attackJumpPow_ = 0.0f;     // 攻撃時のジャンプ力
	float attackDamagePow_ = 0.0f;   // 攻撃力

	// 索敵パラメータ
	float viewRadius_ = 0.0f;        // 視野半径
	float patrolRadius_ = 0.0f;      // 巡回半径
	float targetLostTimer_ = 0.0f;   // プレイヤー見失いタイマー
	float step_ = 0.0f;              // ステート遷移用タイマー

	// 判定・状態フラグ
	bool isNotice_ = false;          // プレイヤー発見フラグ
	bool isHit_ = false;             // 視線遮断・ヒット判定用フラグ

	// 移動
	VECTOR moveDir_ = {};            // 移動方向ベクトル
	VECTOR movePow_ = {};            // 移動力ベクトル

	// 視線判定
	VECTOR startOffset_ = {};        // 敵キャラクターのカプセル開始位置
	VECTOR endOffset_ = {};          // 敵キャラクターのカプセル終了位置
	float radius_ = 0.0f;            // 敵キャラクターのカプセル判定用半径

	// 経路探索
	std::vector<float> minCosts_;
	std::vector<int> parentNodes_;
	std::vector<StagePathData::EDGE> path_;
	VECTOR nextWayPoint_ = {};
	std::vector<int> candidates_;    // ノード候補
	int currentNodeId_ = -1;
	int prevNodeId_ = -1;
	int prevPrevNodeId_ = -1;
	int nextNodeId_ = -1;

	float velocityY_ = 0.0f;        // Y軸方向の速度
};

// 敵生成用データ
struct EnemyData
{
	std::string path;              // モデルのファイルパス

	VECTOR capStartOffset = {};    // カプセル開始オフセット
	VECTOR capEndOffset = {};      // カプセル終了オフセット
	float capRadius = 0.0f;        // カプセル半径
};

namespace EnemyTable
{
    // テーブルの外部宣言
    extern const std::unordered_map<ENEMY_TAG, EnemyData> Table;
}