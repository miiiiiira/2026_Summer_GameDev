#pragma once
#include <vector>
#include <memory>
#include "../Object.h"
#include "../Tag.h"

// Objectをまとめて管理
class ObjectManager
{
public:

	// オブジェクトの生成
	Object* CreateObject(void);

	void Init(void);	// 初期化(全オブジェクト)
	void Update(void);	// 更新(全オブジェクト)
	void PreDraw(void);	// 描画前(全オブジェクト)
	void Draw2D(void);	// 2D描画(全オブジェクト)
	void Draw3D(void);	// 3D描画(全オブジェクト)

	Object* FindWithTag(Tag tag);

	template<class T>
	T* FindComponentWithTag(Tag tag);

private:

	// 管理しているObject
	std::vector<std::unique_ptr<Object>> objects_;
};

template<class T>
inline T* ObjectManager::FindComponentWithTag(Tag tag)
{
	for (auto& obj : objects_)
	{
		if (obj->GetTag() != tag) continue;

		T* comp = obj->GetComponent<T>();
		if (comp) return comp;
	}

	return nullptr;
}
