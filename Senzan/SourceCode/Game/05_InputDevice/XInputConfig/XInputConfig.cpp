#include "XInputConfig.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

XInputConfig::XInputConfig()
{
}

XInputConfig::~XInputConfig()
{
}

void XInputConfig::Update()
{
}

bool XInputConfig::LoadConfig(const std::string& filename)
{
	return false;
}

bool XInputConfig::SaveConfig(const std::string& filename) const
{
	return false;
}

std::string XInputConfig::GetActionName(DWORD xinput_button_code) const
{
	return std::string();
}

DWORD XInputConfig::GetButtonCode(const std::string& action_name) const
{
	return 0;
}

void XInputConfig::LoadDefaultConfig()
{
}

const json* XInputConfig::findConfigObject(const json& root_json) const
{
	return nullptr;
}
