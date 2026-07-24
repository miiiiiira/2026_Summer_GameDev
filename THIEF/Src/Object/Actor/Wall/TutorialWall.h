#pragma once
#include <vector>
#include <string>
#include <DxLib.h>

#include "../../../Scene/Tutorial/TutorialInfo.h"

class TutorialWall
{
public:

	struct ShaderParam
	{
		float timer_;
		float threshold_;
		float dummy_[2];
	};

	struct WallData
	{
		VECTOR pos_;
		std::vector<Tutorial::STATE> activeStates_;
		bool isActive_;
		float fadeProgress_;
	};

	TutorialWall(void);
	~TutorialWall(void);

	void Init(void);
	void Load(void);
	void Update(Tutorial::STATE currentState, bool isClearState, int clearStateEndCount, int maxClearCount);
	void Draw(void);
	void Release(void);

	void AddWall(VECTOR pos, const std::vector<Tutorial::STATE>& states);

private:

	int modelId_;  // モデルハンドル

	std::vector<WallData> walls_; // 生成された全壁データ
};