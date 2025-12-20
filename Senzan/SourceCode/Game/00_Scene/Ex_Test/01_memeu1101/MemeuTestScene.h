#pragma once

#include "../../00_Base/SceneBase.h"
#include "02_UIObject/UIObject.h"

class Ground;
class Player;
class MeshBase;
class Boss;
class CameraBase;
class DirectionLight;

#include <vector>


/*********************************************
*	ゲームメイン画像クラス.
**/

class MemeuTestScene
	: public SceneBase
{
public:
	MemeuTestScene();
	~MemeuTestScene() override;

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

	std::shared_ptr<UIObject> m_TestSprite;
};