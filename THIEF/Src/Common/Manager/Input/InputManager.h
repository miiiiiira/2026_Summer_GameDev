#pragma once
#include <array>
#include <map>
#include <Dxlib.h>
#include <vector>
#include <string>

#include "InputInfo.h"
#include "../../Math/Vector2.h"

class InputManager
{

public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new InputManager(); } };
	static InputManager* GetInstance(void) { return instance_; };
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:
	// インスタンス
	static InputManager* instance_;
	
	InputManager(void) {};		// デフォルトコンストラクタをprivateにして、
	~InputManager(void);		// 外部から生成できない様にする

	// コピー・ムーブ操作を禁止
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

public:

	static constexpr int MAX_BIND_PER_TYPE = 2;

	// 入力種別
	enum class BindType
	{
		KEY,
		PAD_BTN,
		PAD_DIR,
		PAD_STICK,
		PAD_TRIGGER,
		MOUSE
	};

	struct BindInput
	{
		BindType type = BindType::KEY;
		int code = -1;
		INPUT_INFO::JOYPAD_NO pad = INPUT_INFO::JOYPAD_NO::PAD1;
	};

	// 各アクションの入力装置の登録用
	struct ActionBind
	{
		std::array<BindInput, MAX_BIND_PER_TYPE> keyMouse;
		std::array<BindInput, MAX_BIND_PER_TYPE> pad;

		ActionBind()
		{
			for (int i = 0; i < MAX_BIND_PER_TYPE; ++i)
			{
				keyMouse[i].code = -1;
				pad[i].code = -1;
			}
		}
	};

	// アクション登録
	void AddBind(INPUT_INFO::ACTION action, int slot, const BindInput& input);

	// アクション判定
	bool IsAction(INPUT_INFO::ACTION action) const;			// 押しっぱなし
	bool IsActionDown(INPUT_INFO::ACTION action) const;		// 押した瞬間
	bool IsActionUp(INPUT_INFO::ACTION action) const;		// 離した瞬間

	const std::map<INPUT_INFO::ACTION, ActionBind>& GetActionBinds() const { return actionBinds_; }

	enum class ActiveDevice
	{
		KEY_MOUSE,
		PAD
	};

	ActiveDevice GetActiveDevice() const { return activeDevice_; }

private:

	// キー情報
	struct KeyState
	{
		bool now = false;	// 現フレーム
		bool old = false;	// 1フレーム前
		bool down = false;	// 現フレームでボタンが押されたか
		bool up = false;	// 現フレームでボタンが離されたか
	};

	// パッド情報
	struct PadState
	{
		KeyState dir[(int)INPUT_INFO::PAD_DIR::MAX]; // 十字キー
		KeyState btn[(int)INPUT_INFO::PAD_BTN::MAX]; // ボタン
		KeyState stick[(int)INPUT_INFO::PAD_STICK::MAX];

		KeyState lt;
		KeyState rt;

		int lx = 0;
		int ly = 0;

		int rx = 0;
		int ry = 0;

		int ltValue = 0;
		int rtValue = 0;
	};

public:

	void Init(void);		// 初期化
	void Update(void);		// 更新

	// マウス
	Vector2 GetMousePos(void) const;
	bool IsMouse(INPUT_INFO::MouseBtn btn) const;			// 押しっぱなし
	bool IsMouseDown(INPUT_INFO::MouseBtn btn) const;		// 押した瞬間
	bool IsMouseUp(INPUT_INFO::MouseBtn btn) const;			// 離した瞬間

	// パッド移動
	bool IsPadDir(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir) const;
	bool IsPadDirDown(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir) const;
	bool IsPadDirUp(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir) const;

	bool IsPadLT(INPUT_INFO::JOYPAD_NO pad) const;
	bool IsPadRT(INPUT_INFO::JOYPAD_NO pad) const;
	float GetPadLTValue(INPUT_INFO::JOYPAD_NO pad) const;
	float GetPadRTValue(INPUT_INFO::JOYPAD_NO pad) const;

	// アナログキーの入力値から方向を取得
	VECTOR GetDirectionXZAKey(INPUT_INFO::JOYPAD_NO pad) const;
	VECTOR GetRightStickDirection(INPUT_INFO::JOYPAD_NO pad) const;

	// キー割り当て
	void SetActionKey(INPUT_INFO::ACTION action, const std::vector<int>& keys);
	void SetActionMouse(INPUT_INFO::ACTION action, INPUT_INFO::MouseBtn btn);

	void SetActionPadBtn(INPUT_INFO::ACTION action, INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_BTN btn);
	void SetActionPadDir(INPUT_INFO::ACTION action, INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir);
	void SetActionPadTrigger(INPUT_INFO::ACTION action, INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_TRIGGER trigger);

	// キーコンフィグ
	void ClearActionBind(INPUT_INFO::ACTION action);
	void ClearBindByType(INPUT_INFO::ACTION action, BindType type, INPUT_INFO::JOYPAD_NO pad = INPUT_INFO::JOYPAD_NO::PAD1);
	void ClearKeyMouse(INPUT_INFO::ACTION action);
	void ClearPad(INPUT_INFO::ACTION action);

	int CountPadBtn(const ActionBind& bind, INPUT_INFO::JOYPAD_NO pad) const;
	int CountPadDir(const ActionBind& bind, INPUT_INFO::JOYPAD_NO pad) const;
	int CountPadTrigger(const ActionBind& bind, INPUT_INFO::JOYPAD_NO pad) const;

	bool DetectKey(int& outkey);
	bool DetectMouse(INPUT_INFO::MouseBtn& outBtn);
	bool DetectPadBtn(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_BTN& outBtn);
	bool DetectPadDir(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR& outDir);
	bool DetectPadTrigger(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_TRIGGER& out);
	bool DetectPadStick(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_STICK& out);
	
	void SetActionBinds(const std::map<INPUT_INFO::ACTION, ActionBind>& binds);



	// デバッグ用
	bool IsDebugActionDown(INPUT_INFO::DEBUG_ACTION action)const;
	bool IsDebugAction(INPUT_INFO::DEBUG_ACTION action)const;
	bool IsDebugActionUp(INPUT_INFO::DEBUG_ACTION action)const;

private:
	ActiveDevice activeDevice_ = ActiveDevice::KEY_MOUSE;

	std::map<int, KeyState> keyStates_;
	std::map<INPUT_INFO::ACTION, ActionBind> actionBinds_;

	KeyState mouseStates_[(int)INPUT_INFO::MouseBtn::MAX];
	Vector2 mousePos_;

	PadState padStates_[5];

	void UpdateKeyboard();
	void UpdateMouse();
	void UpdatePad(INPUT_INFO::JOYPAD_NO pad);
	static void UpdateKeyState(bool isPressed, KeyState& st);
	bool IsInputAlreadyUsed(const BindInput& input, INPUT_INFO::ACTION ignoreAction) const;
	void RemoveDuplicateOtherActions(INPUT_INFO::ACTION action, const BindInput& input);
	bool IsMatchDebugAction(INPUT_INFO::DEBUG_ACTION action, const BindInput& input) const;

	inline int ToDxPad(INPUT_INFO::JOYPAD_NO pad) { return DX_INPUT_PAD1 + (int)pad; }

	static constexpr float ANALOG_MAX = 32767.0f;
	static constexpr float DEAD_ZONE = 0.35f;


	// デバッグ用
	std::map<INPUT_INFO::DEBUG_ACTION, ActionBind> debugBinds_;

};
