#pragma once
#include <string>
#include <map>
#include "InputManager.h"

namespace InputIO
{
    // CSVファイルへ入力設定を保存
    void SaveConfigCSV(
        const std::string& filePath, 
        const std::map<INPUT_INFO::ACTION, 
        InputManager::ActionBind>& binds
    );

    // CSVからキー設定を読み込み
    bool LoadConfigCSV(
        const std::string& filePath,
        std::map<INPUT_INFO::ACTION, 
        InputManager::ActionBind>& outBinds
    );
}
