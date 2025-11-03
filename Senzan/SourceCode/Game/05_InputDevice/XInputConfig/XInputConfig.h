#pragma once
#include <XInput.h>	//ボタン配置変更のため.
#include "../../Data/Library/json/json.hpp"
#include <string>
#include <map>
#include <vector>

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

	void Update();

	//設定ファイルからキー設定を読み込む.
	bool LoadConfig(const std::string& filename = "InputConfig.json");

	//現在のキー設定をファイルに保存する.
	bool SaveConfig(const std::string& filename = "InputConfig.json") const;

	std::string GetActionName(DWORD xinput_button_code) const;

	DWORD GetButtonCode(const std::string& action_name) const;

	void LoadDefaultConfig();

private:
	std::map<DWORD, std::string> m_ConfigMap;

	//JSONのファイル名.
	const std::string DEFAULT_FILENAME = "InputConfig.json";

	//
	const std::vector<std::string> JSON_PATH =
	{ "Data","Json","Input","InputConfig" };

	const json* findConfigObject(const json& root_json) const;
};