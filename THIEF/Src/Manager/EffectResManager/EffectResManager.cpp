#include <EffekseerForDXLib.h>
#include "../../Application.h"
#include "EffectResManager.h"

// 中身をnullptrで初期化
EffectResManager* EffectResManager::instance_ = nullptr;

void EffectResManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		// 中身がnullptrで何も入っていなかったら行う
		instance_ = new EffectResManager();
	}
}

EffectResManager& EffectResManager::GetInstance(void)
{
	return *instance_;
}

void EffectResManager::Destroy(void)
{
	for (const auto& pair : resourceIds_)
	{
		// エフェクト停止
		StopEffekseer3DEffect(pair.second);

		// エフェクトのメモリ解放
		DeleteEffekseerEffect(pair.second);
	}

	resourceIds_.clear();

	// インスタンスのメモリ解放
	delete instance_;
	instance_ = nullptr;

}

int EffectResManager::GetResourceId(TYPE type)
{
	// 指定されたタイプがなかったら-1を返す
	if (resourceIds_.count(type) == 0)
	{
		return -1;
	}

	// 指定したタイプに対応するIdを返す
	return resourceIds_[type];
}

int EffectResManager::PlayEffect(float scale, VECTOR dir, VECTOR pos, EffectResManager::TYPE effectType)
{

	// エフェクトの再生
	int resId = EffectResManager::GetInstance().GetResourceId(effectType);
	int effectPlayId = PlayEffekseer3DEffect(resId);

	// エフェクトの大きさ
	SetScalePlayingEffekseer3DEffect(
		effectPlayId, scale, scale, scale);

	// エフェクトの回転
	// 方向から角度を出す
	VECTOR angle;
	angle.y = atan2(dir.x, dir.z);

	// XZのベクトルの長さを計算する
	float XZLength = sqrtf(dir.x * dir.x + dir.z * dir.z);

	// X軸の角度を計算する
	angle.x = atan2(dir.y, XZLength);

	// 回転はXY軸のみとする
	angle.z = 0.0f;

	SetRotationPlayingEffekseer3DEffect(
		effectPlayId, -angle.x, angle.y, angle.z);

	// エフェクトの位置
	SetPosPlayingEffekseer3DEffect(
		effectPlayId, pos.x, pos.y, pos.z);

	return effectPlayId;
}

EffectResManager::EffectResManager(void)
{
}

void EffectResManager::Load(void)
{
	// アイテム破壊時のエフェクト
	resourceIds_.emplace(TYPE::ITEM_BREAK_AMPHORA, LoadEffekseerEffect("Data/Effect/BreakAmphora.efkproj", 100.0f));
	resourceIds_.emplace(TYPE::ITEM_BREAK_BOTTLE, LoadEffekseerEffect("Data/Effect/BreakBottle.efkproj", 100.0f));
	resourceIds_.emplace(TYPE::ITEM_BREAK_GOBLET, LoadEffekseerEffect("Data/Effect/BreakGoblet.efkproj", 100.0f));
	resourceIds_.emplace(TYPE::ITEM_BREAK_JAR, LoadEffekseerEffect("Data/Effect/BreakJar.efkproj", 100.0f));
	resourceIds_.emplace(TYPE::ITEM_BREAK_MUG, LoadEffekseerEffect("Data/Effect/BreakMug.efkproj", 100.0f));
	resourceIds_.emplace(TYPE::ITEM_BREAK_POTION, LoadEffekseerEffect("Data/Effect/BreakPotion.efkproj", 100.0f));
	resourceIds_.emplace(TYPE::ITEM_BREAK_SKULL, LoadEffekseerEffect("Data/Effect/BreakSkull.efkproj", 100.0f));
}
