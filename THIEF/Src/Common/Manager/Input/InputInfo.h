#pragma once
#include <windows.h>
#include <Dxlib.h>
#include <string>
#include <unordered_map>

namespace INPUT_INFO
{
	// ゲーム内の操作アクション
	enum class ACTION
	{
		// プレイヤー操作
		MOVE_FORWARD,	// 前進
		MOVE_BACK,		// 後退
		MOVE_LEFT,		// 左移動
		MOVE_RIGHT,		// 右移動
		DASH,			// ダッシュ
		JUMP,			// ジャンプ
		CROUCH,			// しゃがみ
		LIGHT,			// ライトの移動
		MAP,			// マップ
		GRAB,			// 掴む
		ITEM_PUSH,		// アイテムを遠くへ
		ITEM_PULL,		// アイテムを近くへ

		CAMERA_UP,     // カメラ上
		CAMERA_DOWN,   // カメラ下
		CAMERA_LEFT,   // カメラ左
		CAMERA_RIGHT,  // カメラ右

		// UI操作
		UI_MOVE_UP,		// 上
		UI_MOVE_DOWN,	// 下
		UI_MOVE_LEFT,	// 左
		UI_MOVE_RIGHT,	// 右
		DECIDE,			// 決定
		CANCEL,			// キャンセル
		TAB_LEFT,		// タブ切替
		TAB_RIGHT,		// タブ切替

		// システム操作
		PAUSE,			// ポーズ

		MAX				// 最大値
	};

	enum class DEBUG_ACTION
	{
		ADD_POINT,
		REMOVE_POINT,
		RETURN,
		CLEAR,
		OVER,
		SHADER,
		DEBUG,
		TUTORIAL,
		COLOR_CHANGE,
	};

	// アクションカテゴリ
	enum class ACTION_CATEGORY
	{
		PLAYER,	// プレイヤー操作
		UI,		// UI操作
		SYSTEM	// システム操作
	};

	// アクションからカテゴリを取得する
	inline ACTION_CATEGORY GetActionCategory(ACTION action)
	{
		switch (action)
		{
			// プレイヤー操作
		case INPUT_INFO::ACTION::MOVE_FORWARD:
		case INPUT_INFO::ACTION::MOVE_BACK:
		case INPUT_INFO::ACTION::MOVE_LEFT:
		case INPUT_INFO::ACTION::MOVE_RIGHT:
		case INPUT_INFO::ACTION::DASH:
		case INPUT_INFO::ACTION::JUMP:
		case INPUT_INFO::ACTION::CROUCH:
		case INPUT_INFO::ACTION::LIGHT:
		case INPUT_INFO::ACTION::MAP:
		case INPUT_INFO::ACTION::GRAB:
		case INPUT_INFO::ACTION::ITEM_PUSH:
		case INPUT_INFO::ACTION::ITEM_PULL:
		case INPUT_INFO::ACTION::CAMERA_UP:
		case INPUT_INFO::ACTION::CAMERA_DOWN:
		case INPUT_INFO::ACTION::CAMERA_LEFT:
		case INPUT_INFO::ACTION::CAMERA_RIGHT:
			return ACTION_CATEGORY::PLAYER;

			// UI操作
		case INPUT_INFO::ACTION::UI_MOVE_UP:
		case INPUT_INFO::ACTION::UI_MOVE_DOWN:
		case INPUT_INFO::ACTION::UI_MOVE_LEFT:
		case INPUT_INFO::ACTION::UI_MOVE_RIGHT:
		case INPUT_INFO::ACTION::DECIDE:
		case INPUT_INFO::ACTION::CANCEL:
		case INPUT_INFO::ACTION::TAB_LEFT:
		case INPUT_INFO::ACTION::TAB_RIGHT:
			return ACTION_CATEGORY::UI;

			// システム操作
		case INPUT_INFO::ACTION::PAUSE:
			return ACTION_CATEGORY::SYSTEM;

			// 最大値など
		case INPUT_INFO::ACTION::MAX:
		default:
			return ACTION_CATEGORY::PLAYER;
		}
	}

	// マウス関連
	enum class MouseBtn { LEFT, RIGHT, MIDDLE, WHEEL_UP, WHEEL_DOWN, MAX };

	// ゲームパッド関連
	enum class JOYPAD_NO { PAD1 = 0, PAD2, PAD3, PAD4, MAX };
	enum class PAD_DIR { UP, DOWN, LEFT, RIGHT, MAX };
	enum class PAD_STICK { LEFT_UP, LEFT_DOWN, LEFT_LEFT, LEFT_RIGHT, RIGHT_UP, RIGHT_DOWN, RIGHT_LEFT, RIGHT_RIGHT, MAX };
	enum class PAD_BTN { START, BACK, LB, RB, L3, R3, A, B, X, Y, MAX };
	enum class PAD_TRIGGER { LT, RT, MAX };

	// パッドの方向キーに対応するXInputボタンマップ
	static constexpr int PAD_DIR_MAP[(int)PAD_DIR::MAX] =
	{
		XINPUT_BUTTON_DPAD_UP,
		XINPUT_BUTTON_DPAD_DOWN,
		XINPUT_BUTTON_DPAD_LEFT,
		XINPUT_BUTTON_DPAD_RIGHT,
	};

	// パッドのボタンに対応するXInputボタンマップ
	static constexpr int PAD_BTN_MAP[(int)PAD_BTN::MAX] =
	{
		XINPUT_BUTTON_START,
		XINPUT_BUTTON_BACK,
		XINPUT_BUTTON_LEFT_SHOULDER,
		XINPUT_BUTTON_RIGHT_SHOULDER,
		XINPUT_BUTTON_LEFT_THUMB,
		XINPUT_BUTTON_RIGHT_THUMB,
		XINPUT_BUTTON_A,
		XINPUT_BUTTON_B,
		XINPUT_BUTTON_X,
		XINPUT_BUTTON_Y,
	};

	// 文字列変換用マップ アクション⇔ 文字列変換
	inline const std::unordered_map<ACTION, std::string> ACTION_TO_STRING =
	{
		{ACTION::MOVE_FORWARD,	"MOVE FORWARD"},
		{ACTION::MOVE_BACK,		"MOVE BACK"},
		{ACTION::MOVE_LEFT,		"MOVE LEFT"},
		{ACTION::MOVE_RIGHT,	"MOVE RIGHT"},
		{ACTION::DASH,			"SPRINT"},
		{ACTION::JUMP,			"JUMP"},
		{ACTION::CROUCH,		"CROUCH"},
		{ACTION::LIGHT,			"LIGHT MOVE"},
		{ACTION::MAP,			"MAP"},
		{ACTION::GRAB,			"GRABBING"},
		{ACTION::ITEM_PUSH,		"PUSH"},
		{ACTION::ITEM_PULL,		"PULL"},

		{ACTION::CAMERA_UP,		"CAMERA UP"},
		{ACTION::CAMERA_DOWN,	"CAMERA DOWN"},
		{ACTION::CAMERA_LEFT,	"CAMERA LEFT"},
		{ACTION::CAMERA_RIGHT,	"CAMERA RIGHT"},

		{ACTION::UI_MOVE_UP,	"UI UP"},
		{ACTION::UI_MOVE_DOWN,	"UI DOWN"},
		{ACTION::UI_MOVE_LEFT,	"UI LEFT"},
		{ACTION::UI_MOVE_RIGHT,	"UI RIGHT"},
		{ACTION::DECIDE,		"DECIDE" },
		{ACTION::CANCEL,		"CANCEL"},
		{ACTION::TAB_LEFT,		"TAB LEFT"},
		{ACTION::TAB_RIGHT,		"TAB RIGHT"},

		{ACTION::PAUSE,			"PAUSE"},

	};

	// 逆引き用マップを自動生成
	inline const std::unordered_map<std::string, ACTION> STRING_TO_ACTION = [] {
		std::unordered_map<std::string, ACTION> m;
		for (auto& kv : ACTION_TO_STRING) m[kv.second] = kv.first;
		return m;
		}();

	// ACTION → 文字列
	inline std::string ActionToString(ACTION a)
	{
		auto it = ACTION_TO_STRING.find(a);
		return it != ACTION_TO_STRING.end() ? it->second : "UNKNOWN";
	}

	// 文字列 → ACTION
	inline ACTION StringToAction(const std::string& s)
	{
		auto it = STRING_TO_ACTION.find(s);
		return it != STRING_TO_ACTION.end() ? it->second : ACTION::MOVE_FORWARD;
	}

	// ACTION_CATEGORY → 文字列
	inline std::string CategoryToString(ACTION_CATEGORY c)
	{
		switch (c)
		{
		case ACTION_CATEGORY::PLAYER: return "PLAYER";
		case ACTION_CATEGORY::UI:     return "UI";
		case ACTION_CATEGORY::SYSTEM: return "SYSTEM";
		default: return "UNKNOWN";
		}
	}

	// マウスボタン → 文字列
	inline std::string MouseToString(INPUT_INFO::MouseBtn btn)
	{
		switch (btn)
		{
		case INPUT_INFO::MouseBtn::LEFT:		return "MOUSE LEFT";
		case INPUT_INFO::MouseBtn::RIGHT:		return "MOUSE RIGHT";
		case INPUT_INFO::MouseBtn::MIDDLE:		return "MOUSE MIDDLE";
		case INPUT_INFO::MouseBtn::WHEEL_UP:	return "WHEEL UP";
		case INPUT_INFO::MouseBtn::WHEEL_DOWN:	return "WHEEL DOWN";
		default:               return "MOUSE ?";
		}
	}

	// パッド番号文字列変換
	inline JOYPAD_NO StringToPadNo(const std::string& s)
	{
		if (s == "PAD1") return JOYPAD_NO::PAD1;
		if (s == "PAD2") return JOYPAD_NO::PAD2;
		if (s == "PAD3") return JOYPAD_NO::PAD3;
		if (s == "PAD4") return JOYPAD_NO::PAD4;
		return JOYPAD_NO::PAD1;
	}

	// パッドボタン → 文字列
	inline std::string PadBtnToString(INPUT_INFO::PAD_BTN btn)
	{
		switch (btn)
		{
		case INPUT_INFO::PAD_BTN::A:		return "A";
		case INPUT_INFO::PAD_BTN::B:		return "B";
		case INPUT_INFO::PAD_BTN::X:		return "X";
		case INPUT_INFO::PAD_BTN::Y:		return "Y";
		case INPUT_INFO::PAD_BTN::LB:		return "LB";
		case INPUT_INFO::PAD_BTN::RB:		return "RB";
		case INPUT_INFO::PAD_BTN::L3:		return "L3";
		case INPUT_INFO::PAD_BTN::R3:		return "R3";
		case INPUT_INFO::PAD_BTN::START:	return "START";
		case INPUT_INFO::PAD_BTN::BACK:		return "BACK";
		default: return "BTN ?";
		}
	}

	// パッド方向 → 文字列
	inline std::string PadDirToString(INPUT_INFO::PAD_DIR dir)
	{
		switch (dir)
		{
		case INPUT_INFO::PAD_DIR::UP:    return "D-PAD UP";
		case INPUT_INFO::PAD_DIR::DOWN:  return "D-PAD DOWN";
		case INPUT_INFO::PAD_DIR::LEFT:  return "D-PAD LEFT";
		case INPUT_INFO::PAD_DIR::RIGHT: return "D-PAD RIGHT";
		default: return "CONTROLLER ?";
		}
	}

	// トリガー → 文字列
	inline std::string PadTriggerToString(INPUT_INFO::PAD_TRIGGER t)
	{
		switch (t)
		{
		case INPUT_INFO::PAD_TRIGGER::LT: return "LT";
		case INPUT_INFO::PAD_TRIGGER::RT: return "RT";
		default: return "TRIGGER ?";
		}
	}

	// スティック方向 → 文字列
	inline std::string PadStickToString(INPUT_INFO::PAD_STICK s)
	{
		switch (s)
		{
		case INPUT_INFO::PAD_STICK::LEFT_UP:     return "LEFT STICK UP";
		case INPUT_INFO::PAD_STICK::LEFT_DOWN:   return "LEFT STICK DOWN";
		case INPUT_INFO::PAD_STICK::LEFT_LEFT:   return "LEFT STICK LEFT";
		case INPUT_INFO::PAD_STICK::LEFT_RIGHT:  return "LEFT STICK RIGHT";

		case INPUT_INFO::PAD_STICK::RIGHT_UP:    return "RIGHT STICK UP";
		case INPUT_INFO::PAD_STICK::RIGHT_DOWN:  return "RIGHT STICK DOWN";
		case INPUT_INFO::PAD_STICK::RIGHT_LEFT:  return "RIGHT STICK LEFT";
		case INPUT_INFO::PAD_STICK::RIGHT_RIGHT: return "RIGHT STICK RIGHT";

		default:
			return "STICK ?";
		}
	}

	// スキャンコード → キー名
	// Dxlib の KEY_INPUT_* を文字列に変換
	inline const std::unordered_map<int, std::string> KEY_NAME_TABLE =
	{
		// --- 基本キー ---
		{KEY_INPUT_BACK, "BACKSPACE"},
		{KEY_INPUT_TAB, "TAB"},
		{KEY_INPUT_RETURN, "ENTER"},
		{KEY_INPUT_ESCAPE, "ESC"},
		{KEY_INPUT_SPACE, "SPACE"},

		// --- 修飾キー ---
		{KEY_INPUT_LSHIFT, "LEFT SHIFT"},
		{KEY_INPUT_RSHIFT, "RIGHT SHIFT"},
		{KEY_INPUT_LCONTROL, "LEFT CTRL"},
		{KEY_INPUT_RCONTROL, "RIGHT CTRL"},

		// --- ナビゲーション ---
		{KEY_INPUT_LEFT, "ARROW LEFT"},
		{KEY_INPUT_RIGHT, "ARROW RIGHT"},
		{KEY_INPUT_UP, "ARROW UP"},
		{KEY_INPUT_DOWN, "ARROW DOWN"},
		{KEY_INPUT_HOME, "HOME"},
		{KEY_INPUT_END, "END"},
		{KEY_INPUT_PGUP, "PAGEUP"},
		{KEY_INPUT_PGDN, "PAGEDOWN"},
		{KEY_INPUT_INSERT, "INSERT"},
		{KEY_INPUT_DELETE, "DELETE"},

		// --- ファンクションキー ---
		{KEY_INPUT_F1, "F1"},
		{KEY_INPUT_F2, "F2"},
		{KEY_INPUT_F3, "F3"},
		{KEY_INPUT_F4, "F4"},
		{KEY_INPUT_F5, "F5"},
		{KEY_INPUT_F6, "F6"},
		{KEY_INPUT_F7, "F7"},
		{KEY_INPUT_F8, "F8"},
		{KEY_INPUT_F9, "F9"},
		{KEY_INPUT_F10, "F10"},
		{KEY_INPUT_F11, "F11"},
		{KEY_INPUT_F12, "F12"},

		// --- 数字キー ---
		{KEY_INPUT_0, "0"},
		{KEY_INPUT_1, "1"},
		{KEY_INPUT_2, "2"},
		{KEY_INPUT_3, "3"},
		{KEY_INPUT_4, "4"},
		{KEY_INPUT_5, "5"},
		{KEY_INPUT_6, "6"},
		{KEY_INPUT_7, "7"},
		{KEY_INPUT_8, "8"},
		{KEY_INPUT_9, "9"},

		// --- アルファベット ---
		{KEY_INPUT_A, "A"},
		{KEY_INPUT_B, "B"},
		{KEY_INPUT_C, "C"},
		{KEY_INPUT_D, "D"},
		{KEY_INPUT_E, "E"},
		{KEY_INPUT_F, "F"},
		{KEY_INPUT_G, "G"},
		{KEY_INPUT_H, "H"},
		{KEY_INPUT_I, "I"},
		{KEY_INPUT_J, "J"},
		{KEY_INPUT_K, "K"},
		{KEY_INPUT_L, "L"},
		{KEY_INPUT_M, "M"},
		{KEY_INPUT_N, "N"},
		{KEY_INPUT_O, "O"},
		{KEY_INPUT_P, "P"},
		{KEY_INPUT_Q, "Q"},
		{KEY_INPUT_R, "R"},
		{KEY_INPUT_S, "S"},
		{KEY_INPUT_T, "T"},
		{KEY_INPUT_U, "U"},
		{KEY_INPUT_V, "V"},
		{KEY_INPUT_W, "W"},
		{KEY_INPUT_X, "X"},
		{KEY_INPUT_Y, "Y"},
		{KEY_INPUT_Z, "Z"},
	};

	// スキャンコード → キー名取得
	inline std::string GetKeyNameFromScanCode(int scanCode)
	{
		auto it = KEY_NAME_TABLE.find(scanCode);
		if (it != KEY_NAME_TABLE.end())
		{
			return it->second;
		}

		return "KEY ?";
	}
}
