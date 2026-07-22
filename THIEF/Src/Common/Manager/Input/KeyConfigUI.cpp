#include "KeyConfigUI.h"

#include <DxLib.h>
#include "../../../Application.h"
#include "../Audio/AudioManager.h"
#include "../../Collision/Collision.h"
#include "InputManager.h"
#include "InputIO.h"

namespace
{
    // UIレイアウト
    static constexpr int ROW_HEIGHT = 50;
    static constexpr int VISIBLE_ROWS = 10;

    static constexpr int START_Y = 120;

    static constexpr int HEADER_X = 300; // カテゴリーヘッダーのX座標
    static constexpr int ACTION_X = 320; // アクション名のX座標

    static constexpr int START_X = 500;

    static constexpr int SLOT_INTERVAL = 260;
    
    static constexpr int SLOT_LEFT_OFFSET = -10;
    static constexpr int SLOT_RIGHT_OFFSET = 235;
    static constexpr int SLOT_TOP_OFFSET = -10;
    static constexpr int SLOT_BOTTOM_OFFSET = 30;

    static constexpr int COLUMN_COUNT = 2;
    static constexpr int MAX_SLOT = 2;

    static constexpr int TAB_POS_X = 520;
    static constexpr int TAB_POS_Y = 25;

    static constexpr int TAB_SIZE_X = 303;
    static constexpr int TAB_SIZE_Y = 55;
    static constexpr int TAB_COL_SIZE_X = 151;
    static constexpr int TAB_COL_SIZE_Y = TAB_SIZE_Y;
}

void KeyConfigUI::Init()
{
    // 初期選択位置をヘッダー以外にする
    selectRow_ = 0;
    while (displayRows_[selectRow_].isHeader)
        selectRow_++;

    currentTab_ = TabType::KEY_MOUSE;
}

void KeyConfigUI::Load()
{
    // --- 画像読み込み ---
    slotHandle_ = LoadGraph("Data/Image/KeyConfig/key_slot.png");
    slotSelectHandle_ = LoadGraph("Data/Image/KeyConfig/key_slot_select.png");
    slotWaitHandle_ = LoadGraph("Data/Image/KeyConfig/key_slot_wait.png");

    LoadDivGraph("Data/Image/KeyConfig/KeyConfigTab.png",
        2, 1, 2,
        TAB_SIZE_X, TAB_SIZE_Y,
        tabHandle_);

    // --- フォント作成 ---
    fontHandle_ = CreateFontToHandle(
        "Shikakufuto_Free",                   // フォント名
        16,                             // サイズ
        1,                              // 太さ
        DX_FONTTYPE_ANTIALIASING_4X4
    );

    BuildDisplayList();

    tabButtons_.push_back({ TabType::KEY_MOUSE,TAB_POS_X ,TAB_POS_Y ,
        TAB_COL_SIZE_X ,TAB_COL_SIZE_Y });

    tabButtons_.push_back({ TabType::PAD,TAB_POS_X+ TAB_COL_SIZE_X ,TAB_POS_Y+ TAB_COL_SIZE_Y ,
    TAB_COL_SIZE_X, TAB_COL_SIZE_Y });
}

void KeyConfigUI::LoadEnd()
{
    Init();
}

void KeyConfigUI::Update()
{
    if (keyConfig_.IsWaiting())
    {
        UpdateEditing();
        return;
    }

    UpdateSelect();

    // マウスを左クリックされなかったら、処理しない
    if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE) ||
        InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::TAB_LEFT) ||
        InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::TAB_RIGHT))
    {
        UpdateTabSelect();

    }

}

void KeyConfigUI::UpdateEditing()
{
    keyConfig_.Update();

    if (keyConfig_.IsFinished())
    {
        InputIO::SaveConfigCSV(
            "Data/Csv/input.csv",
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

bool KeyConfigUI::CheckSlotHover(int mx, int my, int y, int rowIndex, KeyConfigUI::DisplayRow& row, int& selectRow, int& selectCol, bool click, KeyConfig& keyConfig)
{
    // 現在のタブに応じてデバイスタイプとスロット数を決定
    KeyConfig::DeviceType deviceType =
        (currentTab_ == TabType::KEY_MOUSE)
        ? KeyConfig::DeviceType::KEY_MOUSE
        : KeyConfig::DeviceType::PAD;

    // タブ切り替えによりX座標は共通（START_X）
    int startX = START_X;

    for (int slot = 0; slot < MAX_SLOT; ++slot)
    {
        int x = startX + slot * SLOT_INTERVAL;

        if (IsMouseHover(mx, my, x, y))
        {
            selectRow = rowIndex;
            selectCol = slot; // タブ分けしたため +3 オフセットは不要！

            if (click)
            {
                keyConfig.Begin(
                    row.action,
                    deviceType,
                    INPUT_INFO::JOYPAD_NO::PAD1,
                    slot);
            }

            return true;
        }
    }

    return false;
}

void KeyConfigUI::UpdateTabSelect(void)
{
    // 前回の選択物を入れておく
    TabType prevSelect = currentTab_;

    if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE)
    {
        // マウス選択
        MouseSelect();

        KeySelect();
    }
    else
    {
        // パッド選択
        PadSelect();
    }

    // 中身がNONじゃないかつ、選択物が変わっていたら
    if (currentTab_ != TabType::NONE
        && currentTab_ != prevSelect)
    {
        // ボタンに乗ったサウンドを出す
        AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
    }
}

void KeyConfigUI::MouseSelect(void)
{
    for (const auto& button : tabButtons_)
    {
        if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
            static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
        {
            ChangeTabType(button.type);
            break;
        }
    }
}

void KeyConfigUI::KeySelect(void)
{
    switch (currentTab_)
    {
    case KeyConfigUI::TabType::KEY_MOUSE:

        if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::TAB_RIGHT))
        {
            ChangeTabType(TabType::PAD);
        }
        break;
    case KeyConfigUI::TabType::PAD:

        if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::TAB_LEFT))
        {
            ChangeTabType(TabType::KEY_MOUSE);
        }
        break;
    default:
        break;
    }
}

void KeyConfigUI::PadSelect(void)
{
    switch (currentTab_)
    {
    case KeyConfigUI::TabType::KEY_MOUSE:

        if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::TAB_RIGHT))
        {
            ChangeTabType(TabType::PAD);
        }
        break;
    case KeyConfigUI::TabType::PAD:

        if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::TAB_LEFT))
        {
            ChangeTabType(TabType::KEY_MOUSE);
        }
        break;
    default:
        break;
    }
}

void KeyConfigUI::ChangeTabType(TabType type)
{
    currentTab_ = type;
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
        int nextRow = selectRow_;
        while (nextRow > 0)
        {
            nextRow--;
            if (!displayRows_[nextRow].isHeader)
            {
                selectRow_ = nextRow;
                break;
            }
        }
    }

    // 下
    if (input->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
    {
        int nextRow = selectRow_;
        while (nextRow < maxRow - 1)
        {
            nextRow++;
            if (!displayRows_[nextRow].isHeader)
            {
                selectRow_ = nextRow;
                break;
            }
        }
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

    // 一番上に行ったらPLAYERを表示するようにする
    if (selectRow_ == 1 && displayRows_[0].isHeader)
    {
        scrollOffset_ = 0;
    }

    if (selectRow_ >= scrollOffset_ + visibleRows)
        scrollOffset_ = selectRow_ - visibleRows + 1;

    // 決定
    if (input->IsActionDown(INPUT_INFO::ACTION::DECIDE))
    {
        auto& row = displayRows_[selectRow_];
        if (!row.isHeader)
        {
            bool isKeyMouse = (input->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE);
            int mx, my;
            GetMousePoint(&mx, &my);
            bool isInsideBox = (mx >= 290 && mx <= 1010 && my >= 90 && my <= 630);

            if (!isKeyMouse || isInsideBox)
            {

                bool isKey = selectCol_ < 2;
                int slot = selectCol_ % 2;

                keyConfig_.Begin(
                    row.action,
                    isKey ? KeyConfig::DeviceType::KEY_MOUSE
                    : KeyConfig::DeviceType::PAD,
                    INPUT_INFO::JOYPAD_NO::PAD1,
                    slot
                );
            }

        }
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

    if (!(mx >= 290 && mx <= 1010 && my >= 90 && my <= 630))
    {
        return;
    }

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

        if (CheckSlotHover(mx, my, y,
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

    bool isKeyTab = (currentTab_ == TabType::KEY_MOUSE);
    DrawGraph(TAB_POS_X, TAB_POS_Y, tabHandle_[isKeyTab ? 0 : 1], true);

    DrawBoxAA(290, 90, 1010, 630, 0x000000, false, 3.0f);

    auto* input = InputManager::GetInstance();
    const auto& binds = input->GetActionBinds();

    const int baseY = START_Y;
    const int rowHeight = ROW_HEIGHT;
    const int visibleRows = VISIBLE_ROWS;
    const int startX = START_X;

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
                HEADER_X,
                y,
                GetColor(0, 200, 255),
                Application::GetInstance()->GetFont(),
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
            ACTION_X,
            y,
            isSelectedRow
            ? GetColor(200, 0, 100)
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

        if (currentTab_ == TabType::KEY_MOUSE)
        {
            // --- KEY ---
            for (int slot = 0; slot < MAX_SLOT; ++slot)
            {
                int x = startX + slot * SLOT_INTERVAL;
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
                    x + SLOT_LEFT_OFFSET, y + SLOT_TOP_OFFSET,
                    x + SLOT_RIGHT_OFFSET, y + SLOT_BOTTOM_OFFSET,
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
        }
        else
        {
            // --- PAD ---
            for (int slot = 0; slot < MAX_SLOT; ++slot)
            {
                int x = startX + slot * SLOT_INTERVAL;

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
                    x + SLOT_LEFT_OFFSET, y + SLOT_TOP_OFFSET,
                    x + SLOT_RIGHT_OFFSET, y + SLOT_BOTTOM_OFFSET,
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
        }

        drawCount++;
    }

#ifdef _DEBUG

    DrawBox(TAB_POS_X, TAB_POS_Y, TAB_POS_X + TAB_COL_SIZE_X, TAB_POS_Y +TAB_COL_SIZE_Y, 0x00ff00, false);
    DrawBox(TAB_POS_X + TAB_COL_SIZE_X, TAB_POS_Y + TAB_COL_SIZE_Y, TAB_POS_X + TAB_COL_SIZE_X * 2, TAB_POS_Y + TAB_COL_SIZE_Y, 0x00ff00, false);

#endif //_DEBUG
}

void KeyConfigUI::Delete()
{
    DeleteFontToHandle(fontHandle_);
    DeleteGraph(slotHandle_);
    DeleteGraph(slotWaitHandle_);
    DeleteGraph(slotSelectHandle_);

    for (int i = 0; i < 2; i++)
    {
        DeleteGraph(tabHandle_[i]);
    }
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
