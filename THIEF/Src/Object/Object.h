#pragma once
#include <vector>
#include <memory>
#include <typeinfo>
#include "Component/Component.h"
#include "Tag.h"

class Object
{
public:

	// コンポーネントの追加
	template<class T>
	T* AddComponent()
	{
		// コンポーネントの生成
		auto component = std::make_unique<T>();

		// 自分を所有者として設定
		component->SetOwner(this);

		T* ptr = component.get();

		// unique_ptrとして保存
		components_.push_back(std::move(component));

		return ptr;
	}

	// コンポーネントの取得
	template<class T>
	T* GetComponent()
	{
		// このオブジェクトが持つコンポーネントを全て検索
		for (auto& c : components_)
		{
			// 型を確認してキャスト
			T* cast = dynamic_cast<T*>(c.get());
			if (cast) return cast;
		}
		return nullptr;
	}

	void SendCollision(Collider* other) { for (auto& c : components_) c->OnCollision(other); }

	void Init() { for (auto& c : components_)c->Init(); }		// 全コンポ―ネント初期化
	void Update() { for (auto& c : components_)c->Update(); }	// 全コンポーネント更新
	void PreDraw() { for (auto& c : components_)c->PreDraw(); }	// 全コンポーネント描画前
	void Draw2D() { for (auto& c : components_)c->Draw2D(); }		// 全コンポーネント2D描画
	void Draw3D() { for (auto& c : components_)c->Draw3D(); }		// 全コンポーネント3D描画

	// タグと優先度をセット
	void SetTagAndPriority(Tag tag) { tag_ = tag, priority_ = static_cast<int>(tag); }
	// タグを渡す
	Tag GetTag() const { return tag_; }

	// 描画優先度を渡す
	int GetPriority(void) { return priority_; }

private:
	// このオブジェクトが持つコンポーネント
	std::vector<std::unique_ptr<Component>> components_;

	int priority_ = 0;

	Tag tag_ = Tag::None;
};
