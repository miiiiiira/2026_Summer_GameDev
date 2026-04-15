#include <DxLib.h>
#include "InputManager.h"

InputManager* InputManager::instance_ = nullptr;

void InputManager::Init(void)
{
	// ゲームで使用したいキーを、
	// 事前にここで登録しておいてください
	Add(KEY_INPUT_SPACE);
	Add(KEY_INPUT_RETURN);
	Add(KEY_INPUT_LSHIFT);
	Add(KEY_INPUT_LCONTROL);

	Add(KEY_INPUT_W);
	Add(KEY_INPUT_A);
	Add(KEY_INPUT_S);
	Add(KEY_INPUT_D);

	Add(KEY_INPUT_Q);
	Add(KEY_INPUT_E);
	Add(KEY_INPUT_R);
	Add(KEY_INPUT_F);
	Add(KEY_INPUT_X);
	Add(KEY_INPUT_Z);

	Add(KEY_INPUT_LEFT);
	Add(KEY_INPUT_RIGHT);
	Add(KEY_INPUT_UP);
	Add(KEY_INPUT_DOWN);

	Add(KEY_INPUT_C);
	Add(KEY_INPUT_O);
	Add(KEY_INPUT_ESCAPE);

	InputManager::MouseInfo info;

	// 左クリック
	info = InputManager::MouseInfo();
	info.key = MOUSE_INPUT_LEFT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	// 右クリック
	info = InputManager::MouseInfo();
	info.key = MOUSE_INPUT_RIGHT;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	mouseInfos_.emplace(info.key, info);

	for (int i = 0; i < static_cast<int>(JOYPAD_STICK::MAX); i++)
	{
		prevPadLStick_[i] = nowPadLStick_[i] = false;
	}
}

void InputManager::Update(void)
{

	// キーボード検知
	for (auto& p : keyInfos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = CheckHitKey(p.second.key);
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}

	// マウス検知
	mouseInput_ = GetMouseInput();

	int mousePosX = 0;
	int mousePosY = 0;

	GetMousePoint(&mousePosX, &mousePosY);

	mousePos_.x = static_cast<float>(mousePosX);
	mousePos_.y = static_cast<float>(mousePosY);

	for (auto& p : mouseInfos_)
	{
		p.second.keyOld = p.second.keyNew;
		p.second.keyNew = mouseInput_ == p.second.key;
		p.second.keyTrgDown = p.second.keyNew && !p.second.keyOld;
		p.second.keyTrgUp = !p.second.keyNew && p.second.keyOld;
	}

	// パッド情報
	SetJPadInState(JOYPAD_NO::KEY_PAD1);
	SetJPadInState(JOYPAD_NO::PAD1);
	SetJPadInState(JOYPAD_NO::PAD2);
	SetJPadInState(JOYPAD_NO::PAD3);
	SetJPadInState(JOYPAD_NO::PAD4);

	for (int i = 0; i < static_cast<int>(JOYPAD_STICK::MAX); i++)
	{
		prevPadLStick_[i] = nowPadLStick_[i];
		nowPadLStick_[i] = IsPadLStickNew(JOYPAD_NO::PAD1, static_cast<JOYPAD_STICK>(i));
	}

}

void InputManager::Destroy(void)
{
	// インスタンスのメモリ解放
	delete instance_;
}

void InputManager::Add(int key)
{
	InputManager::Info info = InputManager::Info();
	info.key = key;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	keyInfos_.emplace(key, info);
}

void InputManager::Clear(void)
{
	keyInfos_.clear();
}

bool InputManager::IsNew(int key) const
{
	return Find(key).keyNew;
}

bool InputManager::IsTrgDown(int key) const
{
	return Find(key).keyTrgDown;
}

bool InputManager::IsTrgUp(int key) const
{
	return Find(key).keyTrgUp;
}

Vector2 InputManager::GetMousePos(void) const
{
	return mousePos_;
}

int InputManager::GetMouse(void) const
{
	return mouseInput_;
}

bool InputManager::IsClickMouseLeft(void) const
{
	return mouseInput_ == MOUSE_INPUT_LEFT;
}

bool InputManager::IsClickMouseRight(void) const
{
	return mouseInput_ == MOUSE_INPUT_RIGHT;
}

bool InputManager::IsTrgMouseLeft(void) const
{
	return FindMouse(MOUSE_INPUT_LEFT).keyTrgDown;
}

bool InputManager::IsTrgMouseRight(void) const
{
	return FindMouse(MOUSE_INPUT_RIGHT).keyTrgDown;
}

bool InputManager::ChangeDeviceMouse(void)
{
	return  FindMouse(MOUSE_INPUT_LEFT).keyTrgDown || IsTrgDown(KEY_INPUT_E) || IsTrgDown(KEY_INPUT_Q)
		|| IsTrgDown(KEY_INPUT_W) || IsTrgDown(KEY_INPUT_A) || IsTrgDown(KEY_INPUT_S) || IsTrgDown(KEY_INPUT_D);
}

bool InputManager::ChangeDevicePad(void)
{
	return IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::A) || IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::B)
		|| IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::X) || IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::Y)
		|| IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::DOWN) || IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::UP)
		|| IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::LEFT) || IsPadBtnTrgDown(JOYPAD_NO::PAD1, JOYPAD_BTN::RIGHT)
		|| IsPadLStickTrgDown(JOYPAD_NO::PAD1, JOYPAD_STICK::UP) || IsPadLStickTrgDown(JOYPAD_NO::PAD1, JOYPAD_STICK::DOWN)
		|| IsPadLStickTrgDown(JOYPAD_NO::PAD1, JOYPAD_STICK::LEFT) || IsPadLStickTrgDown(JOYPAD_NO::PAD1, JOYPAD_STICK::RIGHT);
}

InputManager::InputManager(void)
{
	mouseInput_ = -1;
}

InputManager::~InputManager(void)
{
}

const InputManager::Info& InputManager::Find(int key) const
{

	auto it = keyInfos_.find(key);
	if (it != keyInfos_.end())
	{
		return it->second;
	}

	return infoEmpty_;

}

const InputManager::MouseInfo& InputManager::FindMouse(int key) const
{
	auto it = mouseInfos_.find(key);
	if (it != mouseInfos_.end())
	{
		return it->second;
	}

	return mouseInfoEmpty_;
}

InputManager::JOYPAD_TYPE InputManager::GetJPadType(JOYPAD_NO no)
{
	return static_cast<InputManager::JOYPAD_TYPE>(GetJoypadType(static_cast<int>(no)));
}

DINPUT_JOYSTATE InputManager::GetJPadDInputState(JOYPAD_NO no)
{
	// コントローラ情報
	GetJoypadDirectInputState(static_cast<int>(no), &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE InputManager::GetJPadXInputState(JOYPAD_NO no)
{
	// コントローラ情報
	GetJoypadXInputState(static_cast<int>(no), &joyXInState_);
	return joyXInState_;
}

void InputManager::SetJPadInState(JOYPAD_NO jpNo)
{

	int no = static_cast<int>(jpNo);
	auto stateNew = GetJPadInputState(jpNo);
	auto& stateNow = padInfos_[no];

	int max = static_cast<int>(JOYPAD_BTN::MAX);
	for (int i = 0; i < max; i++)
	{

		stateNow.ButtonsOld[i] = stateNow.ButtonsNew[i];
		stateNow.ButtonsNew[i] = stateNew.ButtonsNew[i];

		stateNow.IsOld[i] = stateNow.IsNew[i];
		//stateNow.IsNew[i] = stateNow.ButtonsNew[i] == 128 || stateNow.ButtonsNew[i] == 255;
		stateNow.IsNew[i] = stateNow.ButtonsNew[i] > 0;

		stateNow.IsTrgDown[i] = stateNow.IsNew[i] && !stateNow.IsOld[i];
		stateNow.IsTrgUp[i] = !stateNow.IsNew[i] && stateNow.IsOld[i];


		stateNow.AKeyLX = stateNew.AKeyLX;
		stateNow.AKeyLY = stateNew.AKeyLY;
		stateNow.AKeyRX = stateNew.AKeyRX;
		stateNow.AKeyRY = stateNew.AKeyRY;

	}

}

InputManager::JOYPAD_IN_STATE InputManager::GetJPadInputState(JOYPAD_NO no)
{

	JOYPAD_IN_STATE ret = JOYPAD_IN_STATE();

	auto type = GetJPadType(no);
	
	switch (type)
	{
	case InputManager::JOYPAD_TYPE::OTHER:
		break;
	case InputManager::JOYPAD_TYPE::XBOX_360:
	{
	}
		break;
	case InputManager::JOYPAD_TYPE::XBOX_ONE:
	{
		auto d = GetJPadDInputState(no);
		auto x = GetJPadXInputState(no);

		int idx;

		//   Y
		// X   B
		//   A

		idx = static_cast<int>(JOYPAD_BTN::UP);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_DPAD_UP];// ↑

		idx = static_cast<int>(JOYPAD_BTN::DOWN);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_DPAD_DOWN];// ↓

		idx = static_cast<int>(JOYPAD_BTN::LEFT);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_DPAD_LEFT];// ←

		idx = static_cast<int>(JOYPAD_BTN::RIGHT);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_DPAD_RIGHT];// →

		idx = static_cast<int>(JOYPAD_BTN::A);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_A];// A

		idx = static_cast<int>(JOYPAD_BTN::B);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_B];// B

		idx = static_cast<int>(JOYPAD_BTN::X);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_X];// X

		idx = static_cast<int>(JOYPAD_BTN::Y);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_Y];// Y

		idx = static_cast<int>(JOYPAD_BTN::START);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_START];// START

		idx = static_cast<int>(JOYPAD_BTN::LB);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_LEFT_SHOULDER];// LB

		idx = static_cast<int>(JOYPAD_BTN::RB);
		ret.ButtonsNew[idx] = x.Buttons[XINPUT_BUTTON_RIGHT_SHOULDER];// RB

		idx = static_cast<int>(JOYPAD_BTN::R_TRIGGER);
		ret.ButtonsNew[idx] = x.RightTrigger;// R_TRIGGER

		idx = static_cast<int>(JOYPAD_BTN::L_TRIGGER);
		ret.ButtonsNew[idx] = x.LeftTrigger; // L_TRIGGER

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;

		// 右スティック
		ret.AKeyRX = d.Rx;
		ret.AKeyRY = d.Ry;

	}
		break;
	case InputManager::JOYPAD_TYPE::DUAL_SHOCK_4:
		break;
	case InputManager::JOYPAD_TYPE::DUAL_SENSE:
	{
		
		auto d = GetJPadDInputState(no);
		int idx;

		//   △
		// □  〇
		//   ×

		idx = static_cast<int>(JOYPAD_BTN::Y);
		ret.ButtonsNew[idx] = d.Buttons[3];// △

		idx = static_cast<int>(JOYPAD_BTN::X);
		ret.ButtonsNew[idx] = d.Buttons[0];// □

		idx = static_cast<int>(JOYPAD_BTN::B);
		ret.ButtonsNew[idx] = d.Buttons[2];// 〇

		idx = static_cast<int>(JOYPAD_BTN::A);
		ret.ButtonsNew[idx] = d.Buttons[1];// ×

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;
		
		// 右スティック
		ret.AKeyRX = d.Z;
		ret.AKeyRY = d.Rz;

	}
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_L:
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_JOY_CON_R:
		break;
	case InputManager::JOYPAD_TYPE::SWITCH_PRO_CTRL:
		break;
	case InputManager::JOYPAD_TYPE::MAX:
		break;
	}

	return ret;

}

bool InputManager::IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsNew[static_cast<int>(btn)];
}

bool InputManager::IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsTrgDown[static_cast<int>(btn)];
}

bool InputManager::IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsTrgUp[static_cast<int>(btn)];
}

bool InputManager::IsPadLStickNew(JOYPAD_NO no, JOYPAD_STICK stick)
{
	nowPadLStick_[static_cast<int>(stick)] = false;

	switch (stick)
	{
	case InputManager::JOYPAD_STICK::UP:

		if (padInfos_[static_cast<int>(no)].AKeyLY < -THRESHOLD_STICK)
		{
			nowPadLStick_[static_cast<int>(stick)] = true;
		}

		break;
	case InputManager::JOYPAD_STICK::DOWN:

		if (padInfos_[static_cast<int>(no)].AKeyLY > THRESHOLD_STICK)
		{
			nowPadLStick_[static_cast<int>(stick)] = true;
		}

		break;
	case InputManager::JOYPAD_STICK::LEFT:

		if (padInfos_[static_cast<int>(no)].AKeyLX < -THRESHOLD_STICK)
		{
			nowPadLStick_[static_cast<int>(stick)] = true;
		}

		break;
	case InputManager::JOYPAD_STICK::RIGHT:

		if (padInfos_[static_cast<int>(no)].AKeyLX > THRESHOLD_STICK)
		{
			nowPadLStick_[static_cast<int>(stick)] = true;
		}

		break;
	}

	return nowPadLStick_[static_cast<int>(stick)];
}

bool InputManager::IsPadLStickTrgDown(JOYPAD_NO no, JOYPAD_STICK stick)
{
	return !prevPadLStick_[static_cast<int>(stick)] && nowPadLStick_[static_cast<int>(stick)];
}

bool InputManager::IsPadLStickTrgUp(JOYPAD_NO no, JOYPAD_STICK stick)
{
	return prevPadLStick_[static_cast<int>(stick)] && !nowPadLStick_[static_cast<int>(stick)];
}

VECTOR InputManager::GetDirectionXZAKey(int aKeyX, int aKeyY)
{

	VECTOR ret = { 0.0f, 0.0f, 0.0f };

	// スティックの個々の入力値は、
	// -1000.0f ～ 1000.0f の範囲で返ってくるが、
	// X:1000.0f、Y:1000.0fになることは無い(1000と500くらいが最大)
	
	// スティックの入力値を -1.0 ～ 1.0 に正規化
	float dirX = static_cast<float>(aKeyX) / AKEY_VAL_MAX;
	float dirZ = static_cast<float>(aKeyY) / AKEY_VAL_MAX;

	// ピタゴラスの定理でニュートラル状態からの長さベクトルにする
	// ( 円形のデッドゾーンになる )

	// 平方根により、おおよその最大値が1.0となる
	float len = sqrtf(dirX * dirX + dirZ * dirZ);
	if (len < THRESHOLD)
	{
		// (0.0f, 0.0f, 0.0f)
		return ret;
	}

	// デッドゾーン境界からに再スケーリング(可変デッドゾーン)
	// ( しきい値 0.35 の場合は、 0.0 ～ 0.65 / 0.65 になる )
	float scale = (len - THRESHOLD) / (1.0f - THRESHOLD);
	dirX = (dirX / len) * scale;
	dirZ = (dirZ / len) * scale;

	// Zは前に倒すとマイナス値が返ってくるので反転
	ret = VNorm({ dirX, 0.0f, -dirZ });

	return ret;

}
