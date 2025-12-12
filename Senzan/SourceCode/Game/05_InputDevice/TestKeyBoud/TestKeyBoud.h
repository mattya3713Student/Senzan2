#pragma once

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <memory>

#include "..//..//Data/Library/json/json.hpp" // パスはプロジェクトに合わせて修正してください

using json = nlohmann::json;

class TestKeyBoud
{
public:
    TestKeyBoud();
    ~TestKeyBoud() = default;

    // キーコードをアクション名で取得する
    // 設定が見つからなかった場合は -1 を返す
    int GetKeyCode(const std::string& actionName) const;

    // ★ エラー対応: キーコードからアクション名を取得する
    // 割り当てられていない場合は空文字列を返す
    std::string GetActionName(int keyCode) const;

    // ★ エラー対応: キーを再割り当てする
    // newKeyCode に -1 を指定すると、そのアクションの割り当てを解除する
    void RebindKey(const std::string& actionName, int newKeyCode);

    // ★ エラー対応: 設定ファイルをロードする
    bool LoadData();

    // ★ エラー対応: 設定ファイルを保存する
    bool SaveData() const;

    // ★ エラー対応: デフォルト設定をロードする (Rキーリセット用)
    void LoadDefaultData();

private:
    // アクション名と仮想キーコードを対応させるマップ (例: "MoveUp" -> 'W')
    std::map<std::string, int> m_KeyConfigMap;

    // 逆引き用のマップ (キーコード -> アクション名)
    void UpdateReverseMap();
    std::map<int, std::string> m_KeyConfigReverseMap;

    // デフォルトのキー設定を初期化するヘルパー関数
    void SetupDefaultKeys();

    // ファイルI/Oパス
    const std::string CONFIG_FILE_PATH = "Data\\Json\\Input\\KeyConfig.json";

    // JSON操作ヘルパー
    bool ParseJson(const std::string& jsonContent);
    std::string SerializeJson() const;
};