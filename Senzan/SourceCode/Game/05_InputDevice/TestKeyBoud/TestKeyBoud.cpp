#include "TestKeyBoud.h"
#include <fstream>
#include <iostream>

// TestKeyBoud::TestKeyBoud() コンストラクタ
TestKeyBoud::TestKeyBoud()
{
    // コンストラクタは空にして、初期設定のロードは LoadDefaultData() か LoadData() に任せます。
    // 元々コンストラクタにあった初期設定ロジックは LoadDefaultData() に移管しました。
}

TestKeyBoud::~TestKeyBoud()
{
}

// ----------------------------------------------------
// ★ 新規追加: 初期設定をロードするメソッド (リセット機能の中核)
// ----------------------------------------------------
void TestKeyBoud::LoadDefaultData()
{
    // 現在のマップをクリア
    m_SaveToKey.clear();
    m_KeyToSave.clear();

    // デフォルトのキー割り当てを設定 (WASD)
    m_SaveToKey["MoveUp"] = 87; // W
    m_SaveToKey["MoveDown"] = 83; // S
    m_SaveToKey["MoveLeft"] = 65; // A
    m_SaveToKey["MoveRight"] = 68; // D

    // KeyToSave (キーコード -> アクション名) も更新
    for (const auto& pair : m_SaveToKey) {
        m_KeyToSave[pair.second] = pair.first;
    }
}


void TestKeyBoud::LoadData()
{
    std::ifstream file("Data\\json\\Input\\KeyBoudConfig.json");
    if (!file.is_open())
    {
        std::cout << "JSONファイルが開けませんでした。初期設定を使用します。\n";
        // ファイルが開けない場合は、初期設定をロードして終了
        LoadDefaultData();
        return;
    }

    json jsonData;
    try {
        file >> jsonData;

        m_KeyToSave.clear();
        m_SaveToKey.clear();

        // KeyboardBinding から読み込む
        if (jsonData.contains("KeyboardBinding"))
        {
            for (auto& [action, keyCode] : jsonData["KeyboardBinding"].items())
            {
                int key = keyCode.get<int>();
                m_SaveToKey[action] = key;
                // キーコードが有効な場合のみKeyToSaveに追加
                if (key != -1) {
                    m_KeyToSave[key] = action;
                }
            }
        }
        std::cout << "キーコンフィグを読み込みました。\n";

    }
    catch (const json::parse_error& e) {
        std::cerr << "JSONパースエラー: " << e.what() << '\n';
        // パースエラー発生時も、初期設定をロード
        LoadDefaultData();
    }
}

void TestKeyBoud::SaveData()
{
    json jsonData;

    for (auto& [action, key] : m_SaveToKey)
    {
        // JSONにキーバインドを書き込む
        jsonData["KeyboardBinding"][action] = key;
    }

    std::ofstream file("Data\\json\\Input\\KeyBoudConfig.json");
    if (!file.is_open())
    {
        std::cout << "JSONファイルを開けませんでした。保存できません。\n";
        return;
    }

    file << jsonData.dump(4); // インデント4で整形して保存
    std::cout << "キーコンフィグを保存しました。\n";
}

// ----------------------------------------------------
// TestKeyBoud.h にインライン定義されていない場合は以下も必要です
// ----------------------------------------------------

std::string TestKeyBoud::GetActionName(int key) const
{
    auto it = m_KeyToSave.find(key);
    return (it != m_KeyToSave.end()) ? it->second : "";
}

int TestKeyBoud::GetKeyCode(const std::string& action) const
{
    auto it = m_SaveToKey.find(action);
    return (it != m_SaveToKey.end()) ? it->second : -1;
}

void TestKeyBoud::RebindKey(const std::string& action, int newKey)
{
    int oldKey = GetKeyCode(action);
    if (oldKey != -1)
        m_KeyToSave.erase(oldKey);

    m_SaveToKey[action] = newKey;
    m_KeyToSave[newKey] = action;
}