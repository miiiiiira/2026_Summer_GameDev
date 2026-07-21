#include "KeyConfig.h"
#include "InputManager.h"

KeyConfig::KeyConfig()
    : state_(State::IDLE),
    targetAction_(INPUT_INFO::ACTION::MOVE_FORWARD)
{
}

void KeyConfig::Begin(
    INPUT_INFO::ACTION action,
    DeviceType device,
    INPUT_INFO::JOYPAD_NO pad,
    int slot
)
{
    state_ = State::WAIT_INPUT;
    targetAction_ = action;
    deviceType_ = device;
    padNo_ = pad;
    slot_ = slot;
}

void KeyConfig::Cancel()
{
    state_ = State::IDLE;
}

void KeyConfig::Update()
{
    if (state_ != State::WAIT_INPUT) return;

    DetectInput();
}

void KeyConfig::DetectInput()
{
    auto* input = InputManager::GetInstance();
    bool decided = false;

    if (deviceType_ == DeviceType::KEY_MOUSE)
    {
        int key;
        INPUT_INFO::MouseBtn mouse;

        if (input->DetectKey(key))
        {
            InputManager::BindInput bi{};
            bi.type = InputManager::BindType::KEY;
            bi.code = key;
            input->AddBind(targetAction_, slot_, bi);
            decided = true;
        }
        else if (input->DetectMouse(mouse))
        {
            InputManager::BindInput bi{};
            bi.type = InputManager::BindType::MOUSE;
            bi.code = (int)mouse;
            input->AddBind(targetAction_, slot_, bi);
            decided = true;
        }
    }
    else
    {
        INPUT_INFO::PAD_BTN btn;
        INPUT_INFO::PAD_DIR dir;
        INPUT_INFO::PAD_TRIGGER trigger;
        INPUT_INFO::PAD_STICK stick;

        if (input->DetectPadBtn(padNo_, btn))
        {
            InputManager::BindInput bi{};
            bi.type = InputManager::BindType::PAD_BTN;
            bi.pad = padNo_;
            bi.code = (int)btn;
            input->AddBind(targetAction_, slot_, bi);
            decided = true;
        }
        else if (input->DetectPadDir(padNo_, dir))
        {
            InputManager::BindInput bi{};
            bi.type = InputManager::BindType::PAD_DIR;
            bi.pad = padNo_;
            bi.code = (int)dir;
            input->AddBind(targetAction_, slot_, bi);
            decided = true;
        }
        else if (input->DetectPadTrigger(padNo_, trigger))
        {
            InputManager::BindInput bi{};
            bi.type = InputManager::BindType::PAD_TRIGGER;
            bi.pad = padNo_;
            bi.code = (int)trigger;
            input->AddBind(targetAction_, slot_, bi);
            decided = true;
        }
        else if (input->DetectPadStick(padNo_, stick))
        {
            InputManager::BindInput bi{};
            bi.type = InputManager::BindType::PAD_STICK;
            bi.pad = padNo_;
            bi.code = (int)stick;
            input->AddBind(targetAction_, slot_, bi);
            decided = true;
        }
    }

    // 入力が確定したら即終了
    if (decided)
    {
        state_ = State::FINISHED;
        return;
    }

    // キャンセルだけは残す
    if (input->IsActionDown(INPUT_INFO::ACTION::CANCEL))
    {
        state_ = State::CANCELED;
    }
}
