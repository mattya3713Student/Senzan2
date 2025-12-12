#include "TestKeyBoud.h"
#include <Windows.h> // VK_SPACE, VK_LBUTTON などの仮想キーコードを使用
#include <sstream>
#include <algorithm> 

// --------------------------------------------------------------------------------
// コンストラクタ / デフォルト設定
// --------------------------------------------------------------------------------

TestKeyBoud::TestKeyBoud()
{
    // コンストラクタでデフォルト値を設定しておく
    SetupDefaultKeys();
    UpdateReverseMap();
}

void TestKeyBoud::SetupDefaultKeys()
{
    // マップをクリアしてデフォルト値を設定
    m_KeyConfigMap.clear();

    // WASD移動 (大文字がWindows仮想キーコードとして機能するため大文字で定義)
    m_KeyConfigMap["MoveUp"] = 'W';
    m_KeyConfigMap["MoveDown"] = 'S';
    m_KeyConfigMap["MoveLeft"] = 'A';
    m_KeyConfigMap["MoveRight"] = 'D';

    // アクション
    m_KeyConfigMap["Jump"] = VK_SPACE;
    m_KeyConfigMap["Attack"] = VK_LBUTTON; // マウス左
    m_KeyConfigMap["Parry"] = VK_RBUTTON; // マウス右
    m_KeyConfigMap["Dodge"] = VK_LSHIFT;
    m_KeyConfigMap["SpecialAttack"] = 'Q';
    m_KeyConfigMap["Pause"] = VK_ESCAPE;
}

// --------------------------------------------------------------------------------
// パブリックインターフェース
// --------------------------------------------------------------------------------

int TestKeyBoud::GetKeyCode(const std::string& actionName) const
{
    auto it = m_KeyConfigMap.find(actionName);
    if (it != m_KeyConfigMap.end())
    {
        return it->second;
    }
    // 見つからない場合はエラー値として -1 を返す
    return -1;
}

std::string TestKeyBoud::GetActionName(int keyCode) const
{
    // ReverseMapを使ってキーコードからアクション名を効率的に検索
    auto it = m_KeyConfigReverseMap.find(keyCode);
    if (it != m_KeyConfigReverseMap.end())
    {
        return it->second;
    }
    return ""; // 割り当てられていない
}

void TestKeyBoud::RebindKey(const std::string& actionName, int newKeyCode)
{
    // アクション名が存在するか確認
    if (m_KeyConfigMap.count(actionName))
    {
        // キーを再割り当て
        m_KeyConfigMap[actionName] = newKeyCode;
        // std::cout << "Rebind: Action '" << actionName << "' set to KeyCode " << newKeyCode << std::endl;

        // 逆引きマップを再構築
        UpdateReverseMap();
    }
    else
    {
        std::cerr << "Error: Attempted to rebind non-existent action: " << actionName << std::endl;
    }
}

void TestKeyBoud::UpdateReverseMap()
{
    m_KeyConfigReverseMap.clear();
    for (const auto& pair : m_KeyConfigMap)
    {
        int keyCode = pair.second;
        // キーコードが有効な場合（-1 などの解除済みキーコードではない場合）にのみ追加
        if (keyCode != -1)
        {
            // ReverseMapを構築
            m_KeyConfigReverseMap[keyCode] = pair.first;
        }
    }
}

// --------------------------------------------------------------------------------
// ファイル操作
// --------------------------------------------------------------------------------

bool TestKeyBoud::LoadData()
{
    std::ifstream file(CONFIG_FILE_PATH);
    if (!file.is_open())
    {
        // ファイルが開けない場合はデフォルト設定を使用
        std::cerr << "Error: Could not open config file: " << CONFIG_FILE_PATH << ". Using default keys." << std::endl;
        SetupDefaultKeys();
        UpdateReverseMap();
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    file.close();

    if (content.empty())
    {
        // ファイルが空の場合はデフォルト設定を使用
        std::cerr << "Warning: Config file " << CONFIG_FILE_PATH << " is empty. Using default keys." << std::endl;
        SetupDefaultKeys();
        UpdateReverseMap();
        return false;
    }

    // JSONパース処理を実行
    if (!ParseJson(content))
    {
        // パースエラーの場合もデフォルト設定を使用
        std::cerr << "Error: Failed to parse JSON content from " << CONFIG_FILE_PATH << ". Using default keys." << std::endl;
        SetupDefaultKeys();
        UpdateReverseMap();
        return false;
    }

    UpdateReverseMap();
    // std::cout << "Key config loaded successfully from " << CONFIG_FILE_PATH << std::endl;
    return true;
}

void TestKeyBoud::LoadDefaultData()
{
    // SetupDefaultKeys() を呼び出し、デフォルト設定を適用
    SetupDefaultKeys();
    UpdateReverseMap();
    // std::cout << "Default key configuration loaded." << std::endl;
}

bool TestKeyBoud::SaveData() const
{
    std::ofstream file(CONFIG_FILE_PATH);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for saving: " << CONFIG_FILE_PATH << std::endl;
        return false;
    }

    // JSONシリアライズ処理を実行
    std::string jsonContent = SerializeJson();

    file << jsonContent;
    file.close();

    // std::cout << "Key config saved successfully to " << CONFIG_FILE_PATH << std::endl;
    return true;
}

// --------------------------------------------------------------------------------
// JSON処理 (実装)
// --------------------------------------------------------------------------------

bool TestKeyBoud::ParseJson(const std::string& jsonContent)
{
    // JSONライブラリを使用するため、m_KeyConfigMap をクリアしてから読み込む
    m_KeyConfigMap.clear();

    try
    {
        // 1. JSON文字列をパース
        json j = json::parse(jsonContent);

        // 2. JSONオブジェクトの各要素をマップに格納
        for (auto const& [action, keyCode] : j.items())
        {
            // キーコードが整数であることを確認し、マップに設定
            if (keyCode.is_number_integer())
            {
                m_KeyConfigMap[action] = keyCode.get<int>();
            }
            else
            {
                std::cerr << "Warning: KeyCode for action '" << action << "' is not an integer. Skipped." << std::endl;
            }
        }

        // 読み込んだデータで逆引きマップを更新
        UpdateReverseMap();
        return true;
    }
    catch (json::parse_error& e)
    {
        // パースエラーが発生した場合
        std::cerr << "Error: JSON parse failed in TestKeyBoud. Message: " << e.what() << std::endl;
        return false;
    }
}

// [TestKeyBoud::SerializeJson() の修正案]
std::string TestKeyBoud::SerializeJson() const
{
    json j;
    // std::map の内容を JSON オブジェクトにコピー
    for (const auto& pair : m_KeyConfigMap)
    {
        j[pair.first] = pair.second;
    }

    // インデント（字下げ）付きで文字列化し、保存
    return j.dump(4);
}