#pragma once

#include "../../00_Base/SceneBase.h"

class Ground;
class Player;
class MeshBase;
class Boss;
class CameraBase;
class DirectionLight;
class CapsuleCollider;


#include <vector>

#include "Game/05_InputDevice/TestKeyBoud/TestKeyBoud.h"
#include "Game/05_InputDevice/XInputConfig/XInputConfig.h"
#include "00_MeshObject//03_SnowBall//SnowBall.h"

/*********************************************
*	ゲームメイン画像クラス.
**/

class LTestScene
	: public SceneBase
{
public:
	LTestScene();
	~LTestScene() override;

	void Initialize() override;
	void Create() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;

	HRESULT LoadData();

private:

	std::shared_ptr<DirectionLight>		m_pLight;			// ライト
	std::shared_ptr<CameraBase>			m_pCamera;			// カメラ.

	std::unique_ptr<Ground>		m_pGround;

	std::unique_ptr<Boss>		m_pBoss;
	std::unique_ptr<Player>		m_pPlayer;

	std::shared_ptr<Transform> m_ptransform;


	float m_Radius; // 半径.
	float m_Height;	// 高さ.

	std::unique_ptr<TestKeyBoud>		m_pKeyboardConfig;
	std::unique_ptr<XInputConfig>		m_pCConfig;

	//雪玉表示用のクラス.
	std::shared_ptr<SnowBall> m_pSnowBall;
};