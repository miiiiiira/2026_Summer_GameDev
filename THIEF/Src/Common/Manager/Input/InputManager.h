#pragma once
#include <map>
#include <Dxlib.h>
#include "../../Math/Vector2.h"

class InputManager
{

public:

	// アナログキーの最大値
	static constexpr float AKEY_VAL_MAX = 1000.0f;

	// アナログキーの入力受付しきい値(0.0～1.0)
	static constexpr float THRESHOLD = 0.35f;

	// 視点のしきい値
	static constexpr int THRESHOLD_STICK = 100;

	// ゲームコントローラーの認識番号
	// DxLib定数、DX_INPUT_PAD1等に対応
	enum class JOYPAD_NO
	{
		KEY_PAD1,			// キー入力とパッド１入力
		PAD1,				// パッド１入力
		PAD2,				// パッド２入力
		PAD3,				// パッド３入力
		PAD4,				// パッド４入力
		INPUT_KEY = 4096	// キー入力
	};

	// ゲームコントローラーボタン
	enum class JOYPAD_BTN
	{
		UP = 0,
		DOWN,
		LEFT,
		RIGHT,
		A,
		B,
		X,
		Y,
		START,
		LB,
		RB,
		R_TRIGGER,
		L_TRIGGER,
		MAX
	};

	// ゲームコントローラーボタン
	enum class JOYPAD_STICK
	{
		UP = 0,
		DOWN,
		LEFT,
		RIGHT,
		MAX
	};

	// ゲームコントローラーの入力情報
	struct JOYPAD_IN_STATE
	{
		unsigned char ButtonsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		unsigned char ButtonsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsOld[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsNew[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgDown[static_cast<int>(JOYPAD_BTN::MAX)];
		bool IsTrgUp[static_cast<int>(JOYPAD_BTN::MAX)];
		int AKeyLX;
		int AKeyLY;
		int AKeyRX;
		int AKeyRY;
	};

public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new InputManager(); } };
	static InputManager* GetInstance(void) { return instance_; };
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	// インスタンス
	static InputManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	InputManager(void);
	~InputManager(void);

	// コピー・ムーブ操作を禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// InputManager copy = *InputManager::GetInstance();
	// InputManager copied(*InputManager::GetInstance());
	// InputManager moved = std::move(*InputManager::GetInstance());
public:
	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// リソースの破棄
	void Destroy(void);

	// 判定を行うキーを追加
	void Add(int key);

	// 判定を行うキーをクリア
	void Clear(void);

	// キーの押下判定
	bool IsNew(int key) const;

	// キーの押下判定(押しっぱなしはNG)
	bool IsTrgDown(int key) const;

	// キーを離した時の判定
	bool IsTrgUp(int key) const;

	// マウス座標の取得
	Vector2 GetMousePos(void) const;

	// マウスが左クリックされたか
	bool IsClickMouseLeft(void) const;

	// マウスが右クリックされたか
	bool IsClickMouseRight(void) const;

	// マウスが左クリックされたか(押しっぱなしはNG)
	bool IsTrgMouseLeft(void) const;

	// マウスが右クリックされたか(押しっぱなしはNG)
	bool IsTrgMouseRight(void) const;

	// マウスの左クリックが今離されたか
	bool IsTrgUpMouseLeft(void)const;

	// コントローラの入力情報を取得する
	JOYPAD_IN_STATE GetJPadInputState(JOYPAD_NO no);

	// ボタンが押された
	bool IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const;
	bool IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const;
	bool IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const;

	// 左スティックの入力
	bool IsPadLStickNew(JOYPAD_NO no, JOYPAD_STICK stick);
	bool IsPadLStickTrgDown(JOYPAD_NO no, JOYPAD_STICK stick);
	bool IsPadLStickTrgUp(JOYPAD_NO no, JOYPAD_STICK stick);

	// アナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKey(int aKeyX, int aKeyY);

	// マウスに切り替え
	bool ChangeDeviceMouse(void);
	// パッドに切り替え
	bool ChangeDevicePad(void);

	// タイトル遷移
	bool PushAnyButton(void);

	// 確定ボタン
	bool ConfirmButton(void);

	// 選択ボタン(上)
	bool SelectUp(void);
	// 選択ボタン(下)
	bool SelectDown(void);
	// 選択ボタン(左)
	bool SelectLeft(void);
	// 選択ボタン(右)
	bool SelectRight(void);

	// ポーズボタン
	bool PauseButtons(void);

	// プレイヤー操作
	// ダッシュ
	bool DashButtons(void);
	// しゃがみ
	bool CrouchingButtons(void);
	// ジャンプ
	bool JumpButtons(void);

	// 掴む
	bool IsTrgDownGrabbingButtons(void);
	bool IsNewGrabbingButtons(void);
	bool IsUpGrabbingButtons(void);

	// 物との距離を伸ばす
	bool PushItemButtons(int wheel);
	// 物との距離を縮ませる
	bool PullItemButtons(int wheel);

	// ライトとの距離を伸ばす
	bool PushLightButtons(void);

private:

	// キー情報
	struct Info
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// マウス
	struct MouseInfo
	{
		int key;			// キーID
		bool keyOld;		// 1フレーム前の押下状態
		bool keyNew;		// 現フレームの押下状態
		bool keyTrgDown;	// 現フレームでボタンが押されたか
		bool keyTrgUp;		// 現フレームでボタンが離されたか
	};

	// コントローラ情報
	DINPUT_JOYSTATE joyDInState_;

	// コントローラ情報(XBOX)
	XINPUT_STATE joyXInState_;

	// キー情報
	std::map<int, InputManager::Info> keyInfos_;
	InputManager::Info infoEmpty_;

	// マウス情報
	std::map<int, InputManager::MouseInfo> mouseInfos_;
	InputManager::MouseInfo mouseInfoEmpty_;

	// マウスカーソルの位置
	Vector2 mousePos_;
	
	// パッド情報
	JOYPAD_IN_STATE padInfos_[5];

	bool prevPadLStick_[static_cast<int>(JOYPAD_STICK::MAX)];
	bool nowPadLStick_[static_cast<int>(JOYPAD_STICK::MAX)];

	// 配列の中からキー情報を取得する
	const InputManager::Info& Find(int key) const;

	// 配列の中からマウス情報を取得する
	const InputManager::MouseInfo& FindMouse(int key) const;

	// コントローラの入力情報を取得する
	DINPUT_JOYSTATE GetJPadDInputState(JOYPAD_NO no);

	// コントローラ(XBOX)の入力情報を取得する
	XINPUT_STATE GetJPadXInputState(JOYPAD_NO no);

	// コントローラの入力情報を更新する
	void SetJPadInState(JOYPAD_NO jpNo);

};