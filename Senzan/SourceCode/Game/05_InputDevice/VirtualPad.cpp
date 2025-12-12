#include "VirtualPad.h"
#include <iostream>
#include <cmath>

#include "Game/05_InputDevice/TestKeyBoud/TestKeyBoud.h"
#include "Game/05_InputDevice/Input.h" // Input::GetInstance() のために必要
#include "Game//05_InputDevice//XInputConfig//XInputConfig.h"

using namespace DirectX; // XMFLOAT2のために必要

namespace
{
	// キーコードが設定ファイルで見つからない場合のフォールバック値
	const int DEFAULT_KEY_W = 'W';
	const int DEFAULT_KEY_A = 'A';
	const int DEFAULT_KEY_S = 'S';
	const int DEFAULT_KEY_D = 'D';
	const int DEFAULT_KEY_SPACE = VK_SPACE;
	const int DEFAULT_KEY_Q = 'Q';
	const int DEFAULT_KEY_LSHIFT = VK_LSHIFT;
	const int DEFAULT_KEY_ESC = VK_ESCAPE;
}

// --------------------------------------------------------------------------------
// コンストラクタ
// --------------------------------------------------------------------------------

VirtualPad::VirtualPad()
	: m_pKeyConfig(nullptr) // m_pKeyConfig を nullptr で明示的に初期化
	, m_pControllerConfig(nullptr) // m_pKeyConfig を nullptr で明示的に初期化
{
	// SetKeyConfig が呼ばれるまでキーバインド構築を遅延させるため、SetupDefaultBindings は呼ばない
	SetupDefaultBindings();
}

// --------------------------------------------------------------------------------
// SetKeyConfig の実装
// --------------------------------------------------------------------------------
void VirtualPad::SetKeyConfig(TestKeyBoud* config)
{
	m_pKeyConfig = config;
	// 設定を受け取った直後にキーマップを再構築する
	SetupDefaultBindings();
}

void VirtualPad::SetControllerConfig(XInputConfig* Config)
{
	m_pControllerConfig = Config;
	//再構築する.
	SetupDefaultBindings();
}


// --------------------------------------------------------------------------------
// プライベート テンプレートヘルパーの実装 (checkActionState)
// --------------------------------------------------------------------------------

// テンプレート関数はヘッダーにインライン展開するのが理想ですが、ここでは.cppに記述
template <typename KeyCheckFunc, typename ButtonCheckFunc>
bool VirtualPad::checkActionState(eGameAction action,
	KeyCheckFunc&& keyCheck,
	ButtonCheckFunc&& buttonCheck) const
{
	// アクションのバインディングを取得.
	auto it = m_KeyMap.find(action);
	if (it == m_KeyMap.end() || it->second.Type != eActionType::Button)
	{
		// 軸アクションが間違って渡された場合のログ (Debug::Warning が定義されている前提)
		// if (it != m_KeyMap.end() && it->second.type == eActionType::Axis) { Debug::Warning("Button check called for Axis action."); }
		return false;
	}

	const ActionBinding& binding = it->second;

	for (const auto& source : binding.Sources)
	{
		switch (source.Type)
		{
		case InputSource::eSourceType::KeyBorad:
		case InputSource::eSourceType::MouseButton:
			if (keyCheck(source.KeyCode))
			{
				return true;
			}
			break;

		case InputSource::eSourceType::ControllerButton:
			if (buttonCheck(source.ControllerKey, source.KeyCode))
			{
				return true;
			}
			break;

			// コントローラートリガー軸をボタンとして扱う場合の特殊なケース.
		case InputSource::eSourceType::ControllerTriggerAxis:
		{
			Input& input = Input::GetInstance();
			float triggerValue = 0.0f;
			if (source.StickTarget == InputSource::eStickTarget::LeftTrigger) {
				triggerValue = input.GetLTrigger();
			}
			else if (source.StickTarget == InputSource::eStickTarget::RightTrigger) {
				triggerValue = input.GetRTrigger();
			}

			// トリガーが閾値 (0.1f) 以上であれば押されていると判定.
			if (triggerValue >= 0.1f) return true;
			break;
		}

		default:
			break;
		}
	}

	return false;
}

// 押され続けているか.
bool VirtualPad::IsActionPress(eGameAction action) const
{
	Input& input = Input::GetInstance();

	// KeyBorad/MouseButton.
	auto keyCheck = [&input](const int& code) {
		return input.IsKeyRepeat(code);
	};

	// ControllerButton.
	auto buttonCheck = [&input](XInput::Key key, const int code) {
		return input.IsButtonRepeat(key);
	};

	return checkActionState(action, keyCheck, buttonCheck);
}

// 押された瞬間か.
bool VirtualPad::IsActionDown(eGameAction action, float inputBufferTime) const
{
	Input& input = Input::GetInstance();

	// KeyBorad/MouseButton.
	auto keyCheck = [&input](const int& code) {
		// TODO : inputBufferTime 入れるならここに追加.
		return input.IsKeyDown(code);
	};

	// ControllerButton.
	auto buttonCheck = [&input](XInput::Key key, const int code) {
		return input.IsButtonDown(key);
	};

	return checkActionState(action, keyCheck, buttonCheck);
}

// 離された瞬間か.
bool VirtualPad::IsActionUp(eGameAction action) const
{
	Input& input = Input::GetInstance();

	// KeyBorad/MouseButton.
	auto keyCheck = [&input](const int& code) {
		return input.IsKeyUp(code);
	};

	// ControllerButton.
	auto buttonCheck = [&input](XInput::Key key, const int code) {
		return input.IsButtonUp(key);
	};

	return checkActionState(action, keyCheck, buttonCheck);
}

// 軸アクションの合計値を取得.
float VirtualPad::GetSingleAxisValue(eGameAction componentAction) const
{
	auto it = m_KeyMap.find(componentAction);
	if (it == m_KeyMap.end() || it->second.Type != eActionType::Axis)
	{
		return 0.0f;
	}

	const ActionBinding& binding = it->second;
	Input& input = Input::GetInstance();
	float total_value = 0.0f;

	for (const auto& source : binding.Sources)
	{
		float value = 0.0f;

		switch (source.Type)
		{
		case InputSource::eSourceType::KeyBorad:
			// キーボード入力: 押されていれば1.0fそうでなければ1.0fを値とする
			if (input.IsKeyRepeat(source.KeyCode))
			{
				value = 1.0f;
			}
			break;

		case InputSource::eSourceType::ControllerStickAxis:
		case InputSource::eSourceType::ControllerTriggerAxis:
			// コントローラースティック/トリガーの軸入力.
			if (source.StickTarget == InputSource::eStickTarget::Left)
			{
				// Move_Axis_X/Camera_X の場合 x, Move_Axis_Y/Camera_Y の場合 y を取得
				value = (componentAction == eGameAction::Move_Axis_X || componentAction == eGameAction::Camera_X) ?
					input.GetLStickDirection().x : input.GetLStickDirection().y;
			}
			else if (source.StickTarget == InputSource::eStickTarget::Right)
			{
				value = (componentAction == eGameAction::Move_Axis_X || componentAction == eGameAction::Camera_X) ?
					input.GetRStickDirection().x : input.GetRStickDirection().y;
			}
			else if (source.StickTarget == InputSource::eStickTarget::LeftTrigger)
			{
				value = input.GetLTrigger();
			}
			else if (source.StickTarget == InputSource::eStickTarget::RightTrigger)
			{
				value = input.GetRTrigger();
			}
			break;

		case InputSource::eSourceType::MouseMove:

			DirectX::XMFLOAT2 currentMousePos = Input::GetClientCursorPosition();
			DirectX::XMVECTOR mousePosXM = XMLoadFloat2(&currentMousePos);
			DirectX::XMFLOAT2 oldMousePos = Input::GetPastClientCursorPosition();

			// マウス移動量.
			if (componentAction == eGameAction::Camera_X)
			{
				value = input.GetClientCursorDelta().x;
			}
			else if (componentAction == eGameAction::Camera_Y)
			{
				value = input.GetClientCursorDelta().y;
			}
			break;
		default:
			continue;
		}

		// 入力値に倍率をかけて合計に加える.
		total_value += value * source.Scale;
	}

	// 軸の値の最大値を 1.0f に制限 (キーボード入力の合成などで発生する可能性があるため)
	return std::min(1.0f, std::max(-1.0f, total_value));
}

//  複合軸取得.
DirectX::XMFLOAT2 VirtualPad::GetAxisInput(eGameAxisAction axisType) const
{
	DirectX::XMFLOAT2 result = { 0.0f, 0.0f };

	// それぞれのAxisを取得.
	if (axisType == eGameAxisAction::Move)
	{
		result.x = GetSingleAxisValue(eGameAction::Move_Axis_X);
		result.y = GetSingleAxisValue(eGameAction::Move_Axis_Y);
	}
	else if (axisType == eGameAxisAction::CameraMove)
	{
		result.x = GetSingleAxisValue(eGameAction::Camera_X);
		result.y = GetSingleAxisValue(eGameAction::Camera_Y);
	}
	else
	{
		return { 0.0f, 0.0f };
	}

	// 正規化.
	float lengthSq = result.x * result.x + result.y * result.y;

	if (lengthSq > 1.0f)
	{
		float length = std::sqrt(lengthSq);
		result.x /= length;
		result.y /= length;
	}

	return result;
}

// キーバインドの初期化/再構築 (設定ファイルの内容を反映)
void VirtualPad::SetupDefaultBindings()
{
	using EKey = XInput::Key;
	using EStickState = XInput::StickState;
	using ESource = InputSource::eSourceType;
	using ETarget = InputSource::eStickTarget;
	using Action = eGameAction;

	// キーコードを TestKeyBoud から取得するヘルパーラムダ
	auto getKeyCode = [&](const std::string& actionName, int defaultKey) -> int {
		if (m_pKeyConfig) {
			int key = m_pKeyConfig->GetKeyCode(actionName);
			// GetKeyCode が設定が見つからない場合に -1 を返す場合を考慮
			return (key != -1) ? key : defaultKey;
		}
		return defaultKey;
	};

	//コントローラーコードをXInputConfigから取得するヘルパーラムダ.
	auto GetController = [&](const std::string& ActionName, int DefaultController) -> int
	{
		if (m_pControllerConfig)
		{
			int Controller = m_pControllerConfig->GetController(ActionName);
			return Controller;
		}
		return DefaultController;
	};

	// マップをクリアして再構築
	m_KeyMap.clear();

	// MoveForward: 'W' または 左スティック上 (スティック方向).
	m_KeyMap[Action::MoveForward] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("MoveUp", DEFAULT_KEY_W) }, // 設定を反映
			{ ESource::ControllerStickDir, 0, EKey::None, EStickState::Up, ETarget::Left }
		}
	};

	// MoveBackward: 'S' または 左スティック下.
	m_KeyMap[Action::MoveBackward] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("MoveDown", DEFAULT_KEY_S) }, // 設定を反映
			{ ESource::ControllerStickDir, 0, EKey::None, EStickState::Down, ETarget::Left }
		}
	};

	// MoveRight: 'D' または 右矢印キー または 左スティック右.
	m_KeyMap[Action::MoveRight] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("MoveRight", DEFAULT_KEY_D) }, // 設定を反映
			// { ESource::KeyBorad, VK_RIGHT }, // デフォルトバインディングから削除（設定ファイルに任せる）
			{ ESource::ControllerStickDir, 0, EKey::None, EStickState::Right, ETarget::Left }
		}
	};

	// MoveLeft: 'A' または 左矢印キー または 左スティック左 (追加).
	m_KeyMap[Action::MoveLeft] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("MoveLeft", DEFAULT_KEY_A) }, // 設定を反映
			// { ESource::KeyBorad, VK_LEFT }, // デフォルトバインディングから削除（設定ファイルに任せる）
			{ ESource::ControllerStickDir, 0, EKey::None, EStickState::Left, ETarget::Left }
		}
	};


	// Jump: スペース または Aボタン.
	m_KeyMap[Action::Jump] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("Jump", DEFAULT_KEY_SPACE) }, // 設定を反映
			{ ESource::ControllerButton, 0, EKey::A }
		}
	};

	// Attack: マウス左ボタン または 右トリガー.
	m_KeyMap[Action::Attack] = {
		eActionType::Button,
		{
			{ ESource::MouseButton, getKeyCode("Attack", VK_LBUTTON) }, // 設定を反映
			// ControllerTriggerAxis の処理は checkActionState の特殊ケースで処理されます.
			{ ESource::ControllerTriggerAxis, 0, EKey::None, EStickState::None, ETarget::RightTrigger, 1.0f }
		}
	};

	// Parry.
	m_KeyMap[Action::Parry] = {
		eActionType::Button,
		{
			{ ESource::MouseButton, getKeyCode("Parry", VK_RBUTTON) }, // 設定を反映
			{ ESource::ControllerButton, 0, EKey::B }
		}
	};

	// Dodge.
	m_KeyMap[Action::Dodge] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("Dodge", DEFAULT_KEY_LSHIFT) }, // 設定を反映
			{ ESource::ControllerButton, 0, EKey::LB }
		}
	};

	// SpecialAttack.
	m_KeyMap[Action::SpecialAttack] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("SpecialAttack", DEFAULT_KEY_Q) }, // 設定を反映
			{ ESource::ControllerButton, 0, EKey::RB }
		}
	};


	// Pause.
	m_KeyMap[Action::Pause] = {
		eActionType::Button,
		{
			{ ESource::KeyBorad, getKeyCode("Pause", DEFAULT_KEY_ESC) }, // 設定を反映
			{ ESource::ControllerButton, 0, EKey::Start }
		}
	};

	// ----- [ Axis Actions: 軸入力の内部コンポーネント ] -----

	// 【Move Axis X】: 左スティックX軸の生の値とキーボードA/Dを合成.
	m_KeyMap[Action::Move_Axis_X] = {
		eActionType::Axis,
		{
			// 左スティックX軸
			// 修正前: EStickState::Right -> 修正後: EStickState::None
			{ ESource::ControllerStickAxis, 0, EKey::None, EStickState::None, ETarget::Left, 1.0f },
			// キーボード 'D' (正方向)
			{ ESource::KeyBorad, getKeyCode("MoveRight", DEFAULT_KEY_D), EKey::None, EStickState::None, ETarget::None, 1.0f }, // 設定を反映
			// キーボード 'A' (負方向)
			{ ESource::KeyBorad, getKeyCode("MoveLeft", DEFAULT_KEY_A), EKey::None, EStickState::None, ETarget::None, -1.0f },// 設定を反映
		}
	};

	// 【Move Axis Y】: 左スティックY軸の生の値とキーボードW/Sを合成.
	m_KeyMap[Action::Move_Axis_Y] = {
		eActionType::Axis,
		{
			{ ESource::ControllerStickAxis, 0, EKey::None, EStickState::None, ETarget::Left, 1.0f },
			// キーボード 'W' (正方向)
			{ ESource::KeyBorad, getKeyCode("MoveUp", DEFAULT_KEY_W), EKey::None, EStickState::None, ETarget::None, 1.0f },// 設定を反映
			// キーボード 'S' (負方向)
			{ ESource::KeyBorad, getKeyCode("MoveDown", DEFAULT_KEY_S), EKey::None, EStickState::None, ETarget::None, -1.0f }, // 設定を反映
		}
	};

	// 【Camera Axis X】: 右スティックX軸とマウス移動X軸を合成.
	m_KeyMap[Action::Camera_X] = {
		eActionType::Axis,
		{
			// 修正前: EStickState::Right -> 修正後: EStickState::None
			{ ESource::ControllerStickAxis, 0, EKey::None, EStickState::None, ETarget::Right, 1.0f },
			// マウス移動X軸 (感度調整のためのスケール)
			{ ESource::MouseMove, 0, EKey::None, EStickState::None, ETarget::None, 0.5f }
		}
	};

	// 【Camera Axis Y】: 右スティックY軸とマウス移動Y軸を合成 (Y軸は反転).
	m_KeyMap[Action::Camera_Y] = {
		eActionType::Axis,
		{
			// 右スティックY軸 (カメラでは通常反転)
			// 修正前: EStickState::Up -> 修正後: EStickState::None
			{ ESource::ControllerStickAxis, 0, EKey::None, EStickState::None, ETarget::Right, -1.0f },
			// マウス移動Y軸 (反転させてスケールを設定)
			{ ESource::MouseMove, 0, EKey::None, EStickState::None, ETarget::None, -0.5f }
		}
	};
}