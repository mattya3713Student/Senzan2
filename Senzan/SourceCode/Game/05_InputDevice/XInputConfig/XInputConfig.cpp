#include "XInputConfig.h"
#include <algorithm>
#include <sstream>

constexpr int One = 1;

XInputConfig::XInputConfig()
{
	SetupDefaultController();
	UpdateReverseMap();
}

XInputConfig::~XInputConfig()
{
}

int XInputConfig::GetController(const std::string& ActionName) const
{
	auto it = m_ControllerMap.find(ActionName);
	if (it != m_ControllerMap.end())
	{
		return it->second;
	}
	//見つからなかったら-1を返す.
	return -One;
}

std::string XInputConfig::GetActionName(int Controller) const
{
	auto it = m_ControllerReverseMap.find(Controller);
	if (it != m_ControllerReverseMap.end())
	{
		return it->second;
	}
	return "";	//割り当てなし.
}

void XInputConfig::RebindController(const std::string& ActionName, int newController)
{
	//アクション名が存在するか確認.
	if (m_ControllerMap.count(ActionName))
	{
		//コントローラーを再割り当て.
		m_ControllerMap[ActionName] = newController;
		UpdateReverseMap();
	}
	else
	{
		std::cerr << "ファイルが見つかりませんでした再度確認してください." << ActionName << std::endl;
	}
}

//-----------------------------------------------
// Jsonファイルの読み込み.
//-----------------------------------------------
bool XInputConfig::LoadData()
{
	//パスを書く.
	std::ifstream file(Json_Path);
	if (!file.is_open())
	{
		//ファイルが開けなかったとき.
		std::cerr << "エラー：ファイルが開けません." << Json_Path << "コントローラーの初期配置." << std::endl;
		SetupDefaultController();
		UpdateReverseMap();
		return false;
	}

	std::stringstream Buffer;
	Buffer << file.rdbuf();
	std::string Content = Buffer.str();

	file.close();

	if (Content.empty())
	{
		//ファイルの中身が空の場合はデフォルト設定を使用する.
		std::cerr << "ファイルの中身が空です." << Json_Path << "ファイルが空のためデフォルトの配置で行います." << std::endl;
		SetupDefaultController();
		UpdateReverseMap();
		return false;
	}

	//Jsonパース処理.
	if (!ParseJson(Content))
	{
		//パースエラーの場合もデフォルト設定を使用.
		std::cerr << "エラー: ファイルでエラーが発生しました." << Json_Path << "デフォルト設定のコントローラーでします" << std::endl;
		SetupDefaultController();
		UpdateReverseMap();
		return false;
	}

	UpdateReverseMap();

	return true;
}

bool XInputConfig::SaveData() const
{
	std::ofstream file(Json_Path);
	if (!file.is_open())
	{
		std::cerr << "ファイルが保存できません." << Json_Path << std::endl;
		return false;
	}

	//Jsonシリアライズ処理を実行.
	std::string JsonContent = SerializeJson();

	file << JsonContent;
	file.close();

	return true;
}

void XInputConfig::LoadDefaultData()
{
	SetupDefaultController();
	UpdateReverseMap();
}

void XInputConfig::UpdateReverseMap()
{
	m_ControllerReverseMap.clear();
	for (const auto& pair : m_ControllerMap)
	{
		int Controller = pair.second;
		//コントローラーコードが有効な場合.
		if (Controller != -One)
		{
			//ReverseMapを構築.
			m_ControllerReverseMap[Controller] = pair.first;
		}
	}
}

void XInputConfig::SetupDefaultController()
{
	//マップをクリアしてデフォルト値を設定.
	m_ControllerMap.clear();

	m_ControllerMap["AButton"]		= XINPUT_GAMEPAD_A;
	m_ControllerMap["BButton"]		= XINPUT_GAMEPAD_B;
	m_ControllerMap["XButton"]		= XINPUT_GAMEPAD_X;
	m_ControllerMap["YButton"]		= XINPUT_GAMEPAD_Y;

	m_ControllerMap["Up"]			= XINPUT_GAMEPAD_DPAD_UP;
	m_ControllerMap["Down"]			= XINPUT_GAMEPAD_DPAD_DOWN;
	m_ControllerMap["Left"]			= XINPUT_GAMEPAD_DPAD_LEFT;
	m_ControllerMap["Right"]		= XINPUT_GAMEPAD_DPAD_RIGHT;

	m_ControllerMap["Start"]		= XINPUT_GAMEPAD_START;
	m_ControllerMap["Back"]			= XINPUT_GAMEPAD_BACK;

	m_ControllerMap["LeftThumb"]	= XINPUT_GAMEPAD_LEFT_THUMB;
	m_ControllerMap["RightThumb"]	= XINPUT_GAMEPAD_RIGHT_THUMB;

	m_ControllerMap["LeftShoulder"] = XINPUT_GAMEPAD_LEFT_SHOULDER;
	m_ControllerMap["RightShoulder"] = XINPUT_GAMEPAD_RIGHT_SHOULDER;

	const int MY_GAMEPAD_LSTICK_UP = 0x10000;
	const int MY_GAMEPAD_LSTICK_DOWN = 0x20000;
	const int MY_GAMEPAD_LSTICK_LEFT = 0x40000;
	const int MY_GAMEPAD_LSTICK_RIGHT = 0x80000;

	const int MY_GAMEPAD_LTRIGGER = 0x100000;
	const int MY_GAMEPAD_RTRIGGER = 0x200000;

	m_ControllerMap["LStick_Up"] = MY_GAMEPAD_LSTICK_UP;
	m_ControllerMap["LStick_Down"] = MY_GAMEPAD_LSTICK_DOWN;
	m_ControllerMap["LStick_Left"] = MY_GAMEPAD_LSTICK_LEFT;
	m_ControllerMap["LStick_Right"] = MY_GAMEPAD_LSTICK_RIGHT;

	m_ControllerMap["LTrigger"] = MY_GAMEPAD_LTRIGGER;
	m_ControllerMap["RTrigger"] = MY_GAMEPAD_RTRIGGER;
}

//------------------------------------------------------
// Jsonの処理.
//------------------------------------------------------
bool XInputConfig::ParseJson(const std::string& JsonContent)
{
	//m_ControllerMapをクリアにして読み込む.
	m_ControllerMap.clear();

	try
	{
		json j = json::parse(JsonContent);
		//Jsonオブジェクトの各要素をマップに格納.
		for (auto const& [Action, Controller] : j.items())
		{
			//コントローラーキーが整数であるかを確認し、マップに設定.
			if (Controller.is_number_integer())
			{
				//第二がint型なのでgetの後ろにintをキャストする.
				m_ControllerMap[Action] = Controller.get<int>();
			}
			else
			{
				std::cerr << "警告: コントローラーのアクション" << Action << "整数じゃないのでスキップします" << std::endl;
			}
		}

		//読み込んだデータで逆引きマップ更新.
		UpdateReverseMap();
		return true;
	}
	catch (json::parse_error & e)
	{
		//パースエラーが発生したとき.
		std::cerr << "エラー: Jsonの解析が失敗しました" << e.what() << std::endl;
		return false;
	}
}

std::string XInputConfig::SerializeJson() const
{
	json j;
	for (const auto& Pair : m_ControllerMap)
	{
		j[Pair.first] = Pair.second;
	}

	return j.dump(4);
}

