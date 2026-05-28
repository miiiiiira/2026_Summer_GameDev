#include "ObjectManager.h"
#include "../Collision/CollisionManager.h"
#include "../Component/Component.h"
#include <algorithm>

// Object生成
Object* ObjectManager::CreateObject(void)
{
	objects_.push_back(std::make_unique<Object>());
	return objects_.back().get();
}

// 初期化(全オブジェクト)
void ObjectManager::Init(void)
{
	for (auto& obj : objects_)
		obj->Init();
}

// 更新(全オブジェクト)
void ObjectManager::Update(void)
{
	for (auto& obj : objects_)
		obj->Update();

	CollisionManager::Instance().CheckAll();

	CollisionManager::Instance().Clear();
}

void ObjectManager::PreDraw(void)
{
	for (auto& obj : objects_)
		obj->PreDraw();
}

// 描画(全オブジェクト)
void ObjectManager::Draw(void)
{
	// 描画フェーズの最初で、プライオリティが低い順（奥から手前）に並び替える
	//std::sort(objects_.begin(), objects_.end(), [](Component* a, Component* b) {
	//	return a->GetOwner()->GetPriority() < b->GetOwner()->GetPriority();
	//	});

	for (auto& obj : objects_)
		obj->Draw();
}

Object* ObjectManager::FindWithTag(Tag tag)
{
	for (auto& obj : objects_)
	{
		if (obj->GetTag() == tag)
		{
			return obj.get();
		}
	}
	return nullptr;
}
