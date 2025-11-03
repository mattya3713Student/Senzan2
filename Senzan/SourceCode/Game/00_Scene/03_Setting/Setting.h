#pragma once
#include "../00_Base/SceneBase.h"

#include "Game/02_Camera/CameraBase.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h"	// 地面Static.

// XInputConfigのヘッダーをインクルード
#include "Game/05_InputDevice/XInputConfig/XInputConfig.h" // パスは仮定です。プロジェクトに合わせて修正してください。

#include "Game/05_InputDevice/TestKeyBoud/TestKeyBoud.h"

class Key;

class DirectionLight;

/*********************************************
*	設定画面クラス. (コメント修正推奨)
**/

class Setting
	: public SceneBase
{
public:
	Setting();
	~Setting() override;

	void Initialize() override;
	void Create() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;

private:
	std::shared_ptr<DirectionLight>		m_pLight;			// ライト
	std::shared_ptr<CameraBase>			m_pCamera;			// カメラ.
	std::unique_ptr<Key> m_pKey;
	std::unique_ptr<Ground>		m_pGround;

	std::unique_ptr<TestKeyBoud> m_pKeyboardConfig; // キーボード用キーコンフィグ

	DWORD m_prevButtons = 0;

	bool isDisplaying = false;
};