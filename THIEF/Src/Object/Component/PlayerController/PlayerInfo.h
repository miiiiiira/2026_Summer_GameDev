#pragma once

class PlayerController; // 前方宣言

// 状態関数型
typedef void(*playerStateFunction)(PlayerController&);

// プレイヤーの状態
enum PLAYER_STATE
{
	PLAYER_STATE_IDLE,		// 待機
	PLAYER_STATE_MOVE,		// 移動
	PLAYER_STATE_DASH,		// ダッシュ	
	PLAYER_STATE_CROUCHING,	// しゃがみ
	PLAYER_STATE_SLIDING,	// スライディング
	PLAYER_STATE_HIT_REACT,	// ダメージ時のリアクション
	PLAYER_STATE_DEAD,		// 死亡

	PLAYER_STATE_MAX
};

enum GRABBING_STATE
{
	NOT_GRABBING,	// 掴もうとしてない
	TRY_GRABBING,	// 掴もうとしている
	IS_GRABBING,	// 掴んでいる

	GRABBING_MAX
};

// 状態遷移
struct playerStateCtrl
{
	// ステート
	PLAYER_STATE state_;

	// ステートテーブル
	playerStateFunction initTable_[PLAYER_STATE_MAX];
	playerStateFunction updateTable_[PLAYER_STATE_MAX];
};

// 掴み状態遷移
struct playerGrabStateCtrl
{
	// ステート
	GRABBING_STATE state_;

	// ステートテーブル
	playerStateFunction updateTable_[GRABBING_MAX];
};