#include "MattyaTestScene.h"

#include "Game/05_InputDevice/Input.h"

#include "Game//01_GameObject//02_UIObject/UIGameMain/UIGameMain.h"
#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/ThirdPersonCamera/PlayerThirdPersonCamera.h"
#include "Game/02_Camera/LockOnCamera/LockOnCamera.h"
#include "Game/02_Camera/FirstPersonCamera/FirstPersonCamera.h"

#include "Graphic/Shadow/Shadow.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h"	// 地面Static.
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"	// プレイヤー.
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"	// ボス.

#include "Game/03_Collision/00_Core/ColliderBase.h"	
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"

#include "Game/04_Time/Time.h"

#include "System/Singleton/Debug/CollisionVisualizer/CollisionVisualizer.h"

#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/ImGui/CImGuiManager.h"

#include "SceneManager/SceneManager.h"

// コンストラクタ.
MattyaTestScene::MattyaTestScene()
	: SceneBase()
	, m_pCamera()
	, m_pLight(std::make_shared<DirectionLight>())
	, m_upPlayer(std::make_unique<Player>())
	, m_upBoss(std::make_unique<Boss>())
	, m_upGround(std::make_unique<Ground>())
	, m_upUI(std::make_shared<UIGameMain>())
{
	Initialize();
}

// デストラクタ.
MattyaTestScene::~MattyaTestScene()
{
}

void MattyaTestScene::Initialize()
{
	m_pCamera = std::make_shared<FirstPersonCamera>();
	// カメラ設定.
	m_pCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, 0.0f));
	m_pCamera->SetLook(DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f));
	CameraManager::GetInstance().SetCamera(m_pCamera);

	// ライト設定.
	m_pLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
	LightManager::AttachDirectionLight(m_pLight);

	m_upGround = std::make_unique<Ground>();

}

void MattyaTestScene::Create()
{
}

void MattyaTestScene::Update()
{
    Input::Update();
	m_upGround->Update();
	m_upPlayer->SetTargetPos(m_upBoss.get()->GetPosition());
	m_upPlayer->Update();
    m_pCamera->Update();
	m_upBoss->Update();
	m_upBoss->SetTargetPos(m_upPlayer->GetPosition());


    m_upUI->SetBossHP(m_upBoss->GetMaxHP(), m_upBoss->GetHP());
	m_upUI->SetCombo(m_upPlayer->GetCombo());
	m_upUI->SetPlayerHP(m_upPlayer->GetMaxHP(), m_upPlayer->GetHP());
	m_upUI->SetPlayerUlt(m_upPlayer->GetMaxUltValue(), m_upPlayer->GetUltValue());

	m_upUI->Update();
}

void MattyaTestScene::LateUpdate()
{
	m_upPlayer->LateUpdate();
	m_upBoss->LateUpdate();
	CameraManager::GetInstance().LateUpdate();
	
	if (m_upPlayer->GetHP() <= 0.f)
	{
		SceneManager::GetInstance().LoadScene(eList::GameOver);
		return;
	}

	if (m_upBoss->GetHP() <= 0.f)
	{
		SceneManager::GetInstance().LoadScene(eList::Ending);
		return;
	}

	CollisionDetector::GetInstance().ExecuteCollisionDetection();
}


void MattyaTestScene::Draw()
{
	Shadow::Begin();
	m_upGround->DrawDepth();
	Shadow::End();
	m_upGround->Draw();
	m_upPlayer->Draw();
	m_upBoss->Draw();

	m_upUI->Draw();
	CollisionVisualizer::GetInstance().Draw();
}

HRESULT MattyaTestScene::LoadData()
{
	return S_OK; // 成功を返す
}
