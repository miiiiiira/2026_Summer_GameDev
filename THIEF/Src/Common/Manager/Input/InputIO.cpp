#include "InputIO.h"
#include <fstream>
#include <sstream>

namespace InputIO
{
    // CSVファイルへ入力設定を保存
    void SaveConfigCSV(
        const std::string& filePath,
        const std::map<INPUT_INFO::ACTION,
        InputManager::ActionBind>& binds)
    {
        // 保存用のファイルを開く
        std::ofstream ofs(filePath);
        
        // 開けなかったので終了
        if (!ofs.is_open())
            return;

        // CSVヘッダ
        //・Action：アクション名
        //・Type：入力種別
        //・Pad：パッド番号
        //・Code：キーコード
        ofs << "Action,Type,Pad,Code\n";

        // 全てのアクションを回す
        for (const auto& [action, bind] : binds)
        {
            // enum → 文字列に変換
            std::string act = INPUT_INFO::ActionToString(action);

            // キーボード・マウスの設定保存
            for (const auto& b : bind.keyMouse)
            {
                if (b.code == -1) continue;

                ofs << act << ","           // アクション名
                    << (int)b.type << ","   // 入力種別
                    << ","                  // パッド番号なし
                    << b.code << "\n";      // キーコード
            }

            // ゲームパッドの設定保存
            for (const auto& b : bind.pad)
            {
                if (b.code == -1) continue;

                ofs << act << ","           // アクション名
                    << (int)b.type << ","   // 入力タイプ
                    << (int)b.pad << ","    // パッド番号
                    << b.code << "\n";      // キーコード
            }
        }
    }

    // CSVからキー設定を読み込み
    bool LoadConfigCSV(
        const std::string& filePath,
        std::map<INPUT_INFO::ACTION,
        InputManager::ActionBind>& outBinds)
    {
        // 読み込み用ファイルを開く
        std::ifstream ifs(filePath);

        // ファイルオープン失敗
        if (!ifs.is_open())
            return false;

        // 既存の設定をクリア
        outBinds.clear();

        std::string line;
        
        // 1行目のヘッダを飛ばす
        std::getline(ifs, line);

        // CSVを1行ずつ読み込む
        while (std::getline(ifs, line))
        {
            std::stringstream ss(line);
            std::string actionStr, typeStr, padStr, codeStr;

            // 読み込んだものをそれぞれ分解する
            std::getline(ss, actionStr, ',');
            std::getline(ss, typeStr, ',');
            std::getline(ss, padStr, ',');
            std::getline(ss, codeStr, ',');

            // 文字例→enum変換
            INPUT_INFO::ACTION action =
                INPUT_INFO::StringToAction(actionStr);

            // 数値へ変換
            int type = std::stoi(typeStr);
            int code = std::stoi(codeStr);

            // 入力情報生成
            InputManager::BindInput input{};
            input.type = static_cast<InputManager::BindType>(type);
            input.code = code;

            // パッド入力の場合のみ
            if (!padStr.empty())
                input.pad =
                static_cast<INPUT_INFO::JOYPAD_NO>(
                    std::stoi(padStr));

            // Actionに対応するBindを取得
            auto& bind = outBinds[action];

            // 保存先の配列を決定
            auto& targetArray =
                (input.type == InputManager::BindType::KEY ||
                    input.type == InputManager::BindType::MOUSE)
                ? bind.keyMouse
                : bind.pad;

            // 空いているスロットへ登録
            for (int i = 0; i < InputManager::MAX_BIND_PER_TYPE; ++i)
            {
                if (targetArray[i].code == -1)
                {
                    targetArray[i] = input;
                    break;
                }
            }
        }

        return true;
    }
}
