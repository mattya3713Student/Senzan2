#pragma once
#include <XInput.h>	//ボタン配置変更のため.
#include <string>
#include <map>
#include <vector>
#include <fstream>

#include "../../Data/Library/json/json.hpp"
//nlohmann::jsonを使用.
using json = nlohmann::json;

/*****************************************************************************
*	キーコンフィグ用クラス.
**/

class XInputConfig
{
public:
	XInputConfig();
	~XInputConfig();

	//コントローラー情報をアクション名で取得.
	//見つからなかったら-1で返す.
	int GetController(const std::string& ActionName) const;

	//コントローラーからアクション名を取得.
	//割り当てられていない場合は空文字列を返す.
	std::string GetActionName(int Controller) const;

	//コントローラーの再割り当て.
	//第二引数を-1にしたら割り当てを解除できます.
	void RebindController(const std::string& ActionName, int newController);

	//ロードデータ.
	bool LoadData();

	//セーブデータ(保存用).
	//変更できないようにしておく.
	bool SaveData() const;

	//コントローラーの配置のリセット.
	void LoadDefaultData();
private:
	//アクション名から仮想コントローラーの対応.
	std::map<std::string, int> m_ControllerMap;

	//仮想コントローラーからアクション名.
	void UpdateReverseMap();
	std::map<int, std::string> m_ControllerReverseMap;

	//デフォルトのコントローラー設定を初期化する関数.
	void SetupDefaultController();

	//ファイルのパス.
	const std::string Json_Path = "Data\\Json\\Input\\XInputConfig.json";

	//Json操作関数.
	bool ParseJson(const std::string& JsonContent);
	std::string SerializeJson() const;
};