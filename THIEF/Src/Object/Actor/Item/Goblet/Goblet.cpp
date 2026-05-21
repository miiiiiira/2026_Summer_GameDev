#include<DxLib.h>
#include "Goblet.h"

Goblet::Goblet(void)
{
}

Goblet::~Goblet(void)
{
}

void Goblet::Load(void)
{
	// モデルの読み込み
	info_.modelId_ = MV1LoadModel("Data/Model/Item/Goblet.mv1");
}

void Goblet::Init(void)
{
	// 大きさ初期化
	info_.scale_ = SCALE;
	MV1SetScale(info_.modelId_, info_.scale_);

	// 向き初期化
	info_.angle_ = DEFAULT_ANGLE;
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);

	// 座標初期化
	info_.pos_ = DEFAULT_POS;
	MV1SetPosition(info_.modelId_, info_.pos_);

	// 前回座標初期化
	info_.prevPos_ = info_.pos_;

	// 離された時の座標を初期化
	info_.grabbedPos_ = info_.pos_;

	// アイテムの種類初期化
	info_.itemTag_ = ITEM_TAG::GOBLET;

	// 金額(アイテムのHP)初期化
	info_.money_ = MONEY;

	// 頑丈さ初期化
	info_.hardness_ = HARDNESS;

	// 当たり判定用の半径を初期化
	info_.collisionRadius_ = COLLISION_RADIUS;
	info_.collisionOffset_ = COLLISION_OFFSET;

	// 生存中にする
	info_.isAlive_ = true;

	// 初めは掴まれていない状態にする
	info_.isGrabbed = false;

	// 初めは
	info_.hasTouchedStage_ = true;
}
