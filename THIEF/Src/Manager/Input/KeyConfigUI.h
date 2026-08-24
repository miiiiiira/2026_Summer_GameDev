#pragma once
#include "KeyConfig.h"
#include <vector>
#include <memory>

class Confirm;

class KeyConfigUI
{
public:
    void Init(void);    // 初期化
    void Load(void);
    void LoadEnd(void);
    void Update(void);  // 更新
    void Draw(void);    // 描画
    void Delete(void);  // 削除処理

    // UI情愛
    bool IsActive(void) const { return isActive_; }
    void SetActive(bool flag) { isActive_ = flag; }

    // 表示リスト生成
    void BuildDisplayList();

    // キー入力待機中かどうか
    bool IsWaiting(void) const { return keyConfig_.IsWaiting(); }

    // 表示行データ
    struct DisplayRow
    {
        bool isHeader;                          // カテゴリのヘッダか?
        INPUT_INFO::ACTION action;              // アクション
        INPUT_INFO::ACTION_CATEGORY category;   // カテゴリ
    };

    // タブ
    enum class TabType
    {
        NONE =-1,
        KEY_MOUSE,
        PAD,
    };

    enum class FocusArea
    {
        NONE = -1,
        LIST,         // キーバインドリスト選択中
        RESET_BUTTON  // リセットボタン選択中
    };


private:
    // 表示データ
    std::vector<DisplayRow> displayRows_;   // 表示行リスト
    int scrollOffset_ = 0;                  // スクロール位置

    std::shared_ptr<Confirm> confirm_;

    TabType currentTab_;

    FocusArea currentFocus_;    // 現在のフォーカス領域

    // UI状態
    bool isActive_ = false;     // UI表示状態
    int selectRow_ = 0;         // 選択行
    int selectCol_ = 0;         // 選択列

    // キーコンフィグ
    KeyConfig keyConfig_;

    // 描画
    int slotHandle_ = -1;       // 通常スロット
    int slotWaitHandle_ = -1;   // 入力待ちスロット
    int slotSelectHandle_ = -1; // 選択スロット
    int tabHandle_[2] = { -1, -1 }; // タブ
    int resetHandle_ = -1;           // リセット画像

    // 更新処理
    void UpdateSelect();
    void UpdateEditing();

    // マウス判定
    bool IsMouseHover(int mx, int my, int x, int y);

    bool CheckSlotHover(
        int mx, int my, int y,
        int rowIndex,
        KeyConfigUI::DisplayRow& row,
        int& selectRow, int& selectCol,
        bool click,
        KeyConfig& keyConfig);

    void UpdateTabSelect(void);
    void ChangeTabType(TabType type);

    void ChangeFocus(FocusArea focus);
};
