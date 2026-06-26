#pragma once
#include <vector>
class Transform;
class PlayerController;
class Item;

class Map
{
public:
	Map(PlayerController* player);
	~Map(void);

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

private:

	// アイテムのサイズによって表示する大きさを変更
	static constexpr int BIG_RAD = 15;
	static constexpr int MEDIUM_RAD = 10;
	static constexpr int SMALL_RAD = 5;

	// プレイヤーのポインタ
	PlayerController* player_;

	// 発見したアイテムの格納場所
	std::vector<Item*> foundItems_;

	// 地図画像
	int mapImg_;

	// プレイヤー画像
	int playerImg_;

	// 発見されたアイテムを格納する
	void AddFoundItem(Item* item);
};

