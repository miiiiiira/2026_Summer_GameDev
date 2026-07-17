#pragma once
#include "InputInfo.h"

class KeyConfig
{
public:
    enum class State
    {
        IDLE,
        WAIT_INPUT,     // 入力受付中
        FINISHED,       // 完了
        CANCELED        // キャンセル
    };

    enum class DeviceType
    {
        KEY_MOUSE,
        PAD
    };

public:
    KeyConfig();

    // 設定開始（どのアクションを変更するか）
    void Begin(
        INPUT_INFO::ACTION action,
        DeviceType device,
        INPUT_INFO::JOYPAD_NO pad = INPUT_INFO::JOYPAD_NO::PAD1,
        int slot = 0
    );

    // 毎フレーム呼ぶ
    void Update();

    // 状態取得
    bool IsWaiting() const { return state_ == State::WAIT_INPUT; }
    bool IsFinished() const { return state_ == State::FINISHED; }
    bool IsCanceled() const { return state_ == State::CANCELED; }

    // 設定対象
    INPUT_INFO::ACTION GetTargetAction() const { return targetAction_; }

    // 明示的に終了
    void Cancel();

private:
    State state_;
    INPUT_INFO::ACTION targetAction_;

    DeviceType deviceType_;
    INPUT_INFO::JOYPAD_NO padNo_;

    int slot_ = 0;
    void DetectInput();
};
