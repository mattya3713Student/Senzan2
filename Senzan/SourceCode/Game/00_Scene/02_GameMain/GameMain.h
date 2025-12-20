#pragma once

#include "../00_Base/SceneBase.h"

class Ground;
class Player;
class MeshBase;
class Boss;
class CameraBase;
class DirectionLight;
class UIGameMain;

#include <vector>


/*********************************************
*	ゲームメイン画像クラス.
**/

class GameMain
	: public SceneBase
{
public:
	GameMain();
	~GameMain() override;

	void Initialize() override;
	void Create() override;
	void Update() override;
	void LateUpdate() override;
	void Draw() override;

	HRESULT LoadData();

private:

	std::shared_ptr<CameraBase>			m_pCamera;			// カメラ.
	std::shared_ptr<DirectionLight>		m_pLight;			// ライト

	std::unique_ptr<Ground>		m_pGround;
	
	//一時的な表示用.
	std::unique_ptr<Boss>		m_pBoss;
	std::unique_ptr<Player>		m_pPlayer;

	std::shared_ptr<UIGameMain>	m_pUI;
};