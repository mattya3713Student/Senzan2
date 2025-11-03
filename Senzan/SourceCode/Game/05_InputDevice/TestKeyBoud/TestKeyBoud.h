#pragma once
#include "..//Data//Library//json//json.hpp"
#include <map>
#include <string>


//nlohmann::jsonを使用.
using json = nlohmann::json;

/*********************************************************************
*	キーボードでのキーコンフィグテストクラス.
**/

class TestKeyBoud
{
public:
	TestKeyBoud();
	~TestKeyBoud();

	void LoadData();
	void SaveData();
	// ★ リセット機能: 初期設定をロードするメソッド
	void LoadDefaultData();

	// キーコードからアクション名を取得
	std::string GetActionName(int key) const;

	// ★ 追加: アクション名からキーコードを取得 (コンソール表示やリバインドロジックに必要)
	int GetKeyCode(const std::string& action) const;

	// キーを再割り当て
	void RebindKey(const std::string& action, int newKey);

private:
	std::map<int, std::string> m_KeyToSave;    // キーコード -> アクション名 (例: 87 -> "MoveUp")
	std::map<std::string, int> m_SaveToKey;    // アクション名 -> キーコード (例: "MoveUp" -> 87)
};