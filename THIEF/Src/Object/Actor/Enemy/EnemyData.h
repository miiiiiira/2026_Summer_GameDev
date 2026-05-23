#pragma once
#include <DxLib.h>

struct EnemyData
{
	// モデルのハンドル
	int modelId_;

	// 大きさ
	VECTOR scale_;

	// 向き
	VECTOR angle_;

	// 座標
	VECTOR pos_;

	// 移動方向
	VECTOR moveDir_;

	// 移動スピード
	float moveSpeed_;

	// 移動量
	VECTOR movePow_;

	// 移動前の座標
	VECTOR prevPos_;

	// ジャンプ量
	VECTOR jumpPow_;

	// ジャンプ入力受付時間
	float stepJump_;

	// ジャンプ判定
	bool isJump_;
};