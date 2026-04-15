#pragma once

class Fader
{

public:

	// フェードが進む速さ
	static constexpr float SPEED_ALPHA = 3;

	// 状態
	enum class STATE
	{
		NONE, 
		FADE_OUT,	// 徐々に暗転
		FADE_IN		// 徐々に明転
	};

	//シングルトン
	static void  CreateInstance(void){ if (instance_ == nullptr){instance_ = new Fader();} }
	static Fader* GetInstance(void){ return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; } }

	void Init(void);		//初期化処理
	void Update(void);		//更新処理
	void Draw(void);		//描画処理

	// 状態の取得
	STATE GetState(void);

	// 指定フェードを開始する
	void SetFade(STATE state, unsigned int color = 0x000000);

private:

	// コンストラクタ・デストラクタ
	Fader(void);
	~Fader(void);

	//静的インスタンス
	static Fader* instance_;

	// 状態
	STATE state_;

	// 色情報 (unsignedはマイナスの値が使えない)
	unsigned int color_;

	// 透明度
	float alpha_;
};
