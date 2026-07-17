#include "KeyConfigUI.h"

#include <DxLib.h>
#include "InputManager.h"
#include "InputIO.h"

namespace
{
    // UIレイアウト
    static constexpr int ROW_HEIGHT = 50;
    static constexpr int VISIBLE_ROWS = 10;

    static constexpr int START_Y = 150;

    static constexpr int KEY_START_X = 400;
    static constexpr int PAD_START_X = 800;

    static constexpr int SLOT_INTERVAL = 120;

    static constexpr int SLOT_LEFT_OFFSET = -10;
    static constexpr int SLOT_RIGHT_OFFSET = 90;
    static constexpr int SLOT_TOP_OFFSET = -10;
    static constexpr int SLOT_BOTTOM_OFFSET = 30;

    static constexpr int COLUMN_COUNT = 6;
    static constexpr int MAX_SLOT = 3;
}

void KeyConfigUI::Init()
{
    AddFontResourceEx("Data/Font/APJapanesefont.TTF", FR_PRIVATE, NULL);

    // --- 画像読み込み ---
    bgHandle_ = LoadGraph("Data/UI/KeyConfig/keyconfig_bg2.png");
    slotHandle_ = LoadGraph("Data/UI/KeyConfig/key_slot2.png");
    slotSelectHandle_ = LoadGraph("Data/UI/KeyConfig/key_slot_select2.png");
    slotWaitHandle_ = LoadGraph("Data/UI/KeyConfig/key_slot_wait2.png");

    // --- フォント作成 ---
    fontHandle_ = CreateFontToHandle(
        "あんずもじ",                   // フォント名
        16,                             // サイズ
        6,                              // 太さ
        DX_FONTTYPE_ANTIALIASING_4X4
    );

    BuildDisplayList();

    // 初期選択位置をヘッダー以外にする
    selectRow_ = 0;
    while (displayRows_[selectRow_].isHeader)
        selectRow_++;
}

void KeyConfigUI::Update()
{
    auto* input = InputManager::GetInstance();

    // 編集中は無効
    if (!keyConfig_.IsWaiting())
    {
        if (input->IsActionDown(INPUT_INFO::ACTION::OPTION))
        {
            isActive_ = !isActive_;
        }
    }

    if (!isActive_) return;

    if (keyConfig_.IsWaiting())
    {
        UpdateEditing();
        return;
    }

    UpdateSelect();
}

void KeyConfigUI::UpdateEditing()
{
    keyConfig_.Update();

    if (keyConfig_.IsFinished())
    {
        InputIO::SaveConfigCSV(
            "input.csv",
            InputManager::GetInstance()->GetActionBinds());
    }
}

bool KeyConfigUI::IsMouseHover(int mx, int my, int x, int y)
{
    int left = x + SLOT_LEFT_OFFSET;
    int right = x + SLOT_RIGHT_OFFSET;
    int top = y + SLOT_TOP_OFFSET;
    int bottom = y + SLOT_BOTTOM_OFFSET;

    return
        mx >= left && mx <= right &&
        my >= top && my <= bottom;
}

bool KeyConfigUI::CheckKeySlotHover(
    int mx, int my, int y,
    int rowIndex,
    KeyConfigUI::DisplayRow& row, 
    int& selectRow, int& selectCol,
    bool click, 
    KeyConfig& keyConfig)
{
    for (int slot = 0; slot < 3; ++slot)
    {
        int x = KEY_START_X + slot * SLOT_INTERVAL;

        if (IsMouseHover(mx, my, x, y))
        {
            selectRow = rowIndex;
            selectCol = slot;

            if (click)
            {
                keyConfig.Begin(
                    row.action,
                    KeyConfig::DeviceType::KEY_MOUSE,
                    INPUT_INFO::JOYPAD_NO::PAD1,
                    slot);
            }

            return true;
        }
    }

    return false;
}

bool KeyConfigUI::CheckPadSlotHover(
    int mx, int my, int y,
    int rowIndex,
    KeyConfigUI::DisplayRow& row,
    int& selectRow, int& selectCol,
    bool click, 
    KeyConfig& keyConfig)
{
    for (int slot = 0; slot < 3; ++slot)
    {
        int x = PAD_START_X + slot * SLOT_INTERVAL;

        if (IsMouseHover(mx, my, x, y))
        {
            selectRow = rowIndex;
            selectCol = slot + 3;

            if (click)
            {
                keyConfig.Begin(
                    row.action,
                    KeyConfig::DeviceType::PAD,
                    INPUT_INFO::JOYPAD_NO::PAD1,
                    slot);
            }

            return true;
        }
    }

    return false;
}

void KeyConfigUI::UpdateSelect()
{
    auto* input = InputManager::GetInstance();

    int maxRow = (int)displayRows_.size();

    // ヘッダーに乗っていたら次の有効行へ
    if (displayRows_[selectRow_].isHeader)
    {
        do {
            selectRow_ = (selectRow_ + 1) % maxRow;
        } while (displayRows_[selectRow_].isHeader);
    }

    // 上
    if (input->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
    {
        do {
            selectRow_ = (selectRow_ - 1 + maxRow) % maxRow;
        } while (displayRows_[selectRow_].isHeader);
    }

    // 下
    if (input->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
    {
        do {
            selectRow_ = (selectRow_ + 1) % maxRow;
        } while (displayRows_[selectRow_].isHeader);
    }

    // 横移動
    if (input->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
        selectCol_ = (selectCol_ - 1 + COLUMN_COUNT) % COLUMN_COUNT;

    if (input->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
        selectCol_ = (selectCol_ + 1) % COLUMN_COUNT;

    // スクロール制御
    const int visibleRows = VISIBLE_ROWS;

    if (selectRow_ < scrollOffset_)
        scrollOffset_ = selectRow_;

    if (selectRow_ >= scrollOffset_ + visibleRows)
        scrollOffset_ = selectRow_ - visibleRows + 1;

    // 決定
    if (input->IsActionDown(INPUT_INFO::ACTION::DECIDE))
    {
        auto& row = displayRows_[selectRow_];
        if (row.isHeader) return;

        bool isKey = selectCol_ < 3;
        int slot = selectCol_ % 3;

        keyConfig_.Begin(
            row.action,
            isKey ? KeyConfig::DeviceType::KEY_MOUSE
            : KeyConfig::DeviceType::PAD,
            INPUT_INFO::JOYPAD_NO::PAD1,
            slot
        );
    }

    // パッド操作中はマウスUI無効
    if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::PAD)
    {
        return;
    }

    // ======================
    // マウス入力取得
    // ======================
    int mx, my;
    GetMousePoint(&mx, &my);

    // ======================
    // スロットホバー判定
    // ======================
    const int baseY = START_Y;

    for (int i = scrollOffset_;
        i < (int)displayRows_.size() &&
        i < scrollOffset_ + VISIBLE_ROWS;
        ++i)
    {
        int drawIndex = i - scrollOffset_;
        int y = baseY + drawIndex * ROW_HEIGHT;

        auto& row = displayRows_[i];

        if (row.isHeader)
            continue;

        if (CheckKeySlotHover(
            mx, my, y,
            i, row,
            selectRow_, selectCol_,
            InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE),
            keyConfig_))
            return;

        if (CheckPadSlotHover(
            mx, my, y,
            i, row,
            selectRow_, selectCol_,
            InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE),
            keyConfig_))
            return;
    }
}

void KeyConfigUI::Draw()
{
    if (!isActive_) return;

    auto* input = InputManager::GetInstance();
    const auto& binds = input->GetActionBinds();

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawExtendGraph(0, 0, 1280, 720, bgHandle_, FALSE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    const int baseY = START_Y;
    const int rowHeight = ROW_HEIGHT;
    const int visibleRows = VISIBLE_ROWS;

    int drawCount = 0;

    for (int i = scrollOffset_;
        i < (int)displayRows_.size() &&
        drawCount < visibleRows;
        ++i)
    {
        int y = baseY + drawCount * rowHeight;

        auto& row = displayRows_[i];

        // ==========================================
        // ■ カテゴリーヘッダー
        // ==========================================
        if (row.isHeader)
        {
            std::string title =
                "【" +
                INPUT_INFO::CategoryToString(row.category) +
                "】";

            DrawFormatStringToHandle(
                210,
                y,
                GetColor(0, 200, 255),
                fontHandle_,
                "%s",
                title.c_str()
            );

            drawCount++;
            continue;
        }

        // ==========================================
        // ■ アクション
        // ==========================================

        bool isSelectedRow =
            (!row.isHeader && i == selectRow_);

        DrawFormatStringToHandle(
            250,
            y,
            isSelectedRow
            ? GetColor(255, 255, 0)
            : GetColor(0, 0, 0),
            fontHandle_,
            "%s",
            INPUT_INFO::ActionToString(row.action).c_str()
        );

        auto it = binds.find(row.action);
        if (it == binds.end())
        {
            drawCount++;
            continue;
        }

        const auto& bind = it->second;

        int keyStartX = KEY_START_X;
        int padStartX = PAD_START_X;

        // --- KEY ---
        for (int slot = 0; slot < MAX_SLOT; ++slot)
        {
            int x = keyStartX + slot * 120;
            bool selected = isSelectedRow && selectCol_ == slot;

            bool isWaiting =
                keyConfig_.IsWaiting() &&
                selected;

            int handle = slotHandle_;

            if (isWaiting)
                handle = slotWaitHandle_;
            else if (selected)
                handle = slotSelectHandle_;

            DrawExtendGraph(
                x - 10, y - 10,
                x + 90, y + 30,
                handle,
                TRUE
            );

            std::string text = "---";

            const auto& b = bind.keyMouse[slot];

            if (b.code != -1)
            {
                if (b.type == InputManager::BindType::KEY)
                    text = INPUT_INFO::GetKeyNameFromScanCode(b.code);
                else
                    text = INPUT_INFO::MouseToString(
                        (INPUT_INFO::MouseBtn)b.code);
            }

            DrawFormatStringToHandle(
                x,
                y,
                GetColor(0, 0, 0),
                fontHandle_,
                "%s",
                text.c_str()
            );
        }

        // --- PAD ---
        for (int slot = 0; slot < MAX_SLOT; ++slot)
        {
            int x = padStartX + slot * 120;

            bool selected =
                isSelectedRow &&
                selectCol_ == slot + 3;

            bool isWaiting =
                keyConfig_.IsWaiting() &&
                selected;

            int handle = slotHandle_;

            if (isWaiting)
                handle = slotWaitHandle_;
            else if (selected)
                handle = slotSelectHandle_;

            DrawExtendGraph(
                x - 10, y - 10,
                x + 90, y + 30,
                handle,
                TRUE);

            std::string text = "---";

            const auto& b = bind.pad[slot];

            if (b.code != -1)
            {
                switch (b.type)
                {
                case InputManager::BindType::PAD_BTN:
                    text = INPUT_INFO::PadBtnToString(
                        (INPUT_INFO::PAD_BTN)b.code);
                    break;

                case InputManager::BindType::PAD_DIR:
                    text = INPUT_INFO::PadDirToString(
                        (INPUT_INFO::PAD_DIR)b.code);
                    break;

                case InputManager::BindType::PAD_TRIGGER:
                    text = INPUT_INFO::PadTriggerToString(
                        (INPUT_INFO::PAD_TRIGGER)b.code);
                    break;

                case InputManager::BindType::PAD_STICK:
                    text = INPUT_INFO::PadStickToString(
                        (INPUT_INFO::PAD_STICK)b.code);
                    break;
                }
            }

            DrawFormatStringToHandle(
                x,
                y,
                GetColor(0, 0, 0),
                fontHandle_,
                "%s",
                text.c_str()
            );
        }

        drawCount++;
    }
}

void KeyConfigUI::Delete()
{
    RemoveFontResourceEx(
        "Data/Font/APJapanesefont.TTF",
        FR_PRIVATE,
        NULL
    );

    DeleteFontToHandle(fontHandle_);
    DeleteGraph(bgHandle_);
    DeleteGraph(slotHandle_);
    DeleteGraph(slotWaitHandle_);
    DeleteGraph(slotSelectHandle_);
}

void KeyConfigUI::BuildDisplayList()
{
    displayRows_.clear();

    using namespace INPUT_INFO;

    std::vector<ACTION_CATEGORY> categories =
    {
        ACTION_CATEGORY::PLAYER,
        ACTION_CATEGORY::UI,
        ACTION_CATEGORY::SYSTEM
    };

    for (auto cat : categories)
    {
        // ヘッダー追加
        displayRows_.push_back({ true, ACTION::MAX, cat });

        for (int i = 0; i < (int)ACTION::MAX; ++i)
        {
            ACTION a = (ACTION)i;
            if (GetActionCategory(a) == cat)
            {
                displayRows_.push_back({ false, a, cat });
            }
        }
    }
}
