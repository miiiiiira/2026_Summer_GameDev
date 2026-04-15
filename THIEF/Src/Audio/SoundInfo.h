#pragma once
#include <string>

// サウンドID
enum class SoundID
{
	NON = -1,
	
	BGM_TITLE,
	BGM_BATTLE,
	SE_BUTTON,
	SE_RUN,
	SE_ATTACK,
};

// 読み込むシーン
enum class LoadScene
{
	SYSTEM,		// システム
	TITLE,		// タイトル
	GAME,		// ゲーム
};

// サウンド属性
struct SoundData
{
	std::string path;	// サウンドのパス
};
