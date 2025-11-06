#include "GameMain.h"

#include "System/Singleton/CameraManager/CameraManager.h"
#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/ThirdPersonCamera/ThirdPersonCamera.h"

#include "Graphic/Shadow/Shadow.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h"	// 地面Static.

#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"


#include <algorithm> // std::min のために必要

// コンストラクタ.
GameMain::GameMain()
	: SceneBase()
	, m_pCamera(std::make_shared<ThirdPersonCamera>())
	, m_pLight(std::make_shared<DirectionLight>())
{
	Initialize();
}

// デストラクタ.
GameMain::~GameMain()
{
}

void GameMain::Initialize()
{
	// カメラ設定.
	m_pCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, -5.0f));
	m_pCamera->SetLook(DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f));
	CameraManager::GetInstance().SetCamera(m_pCamera);

	// ライト設定.
	m_pLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
	LightManager::AttachDirectionLight(m_pLight);

	m_pGround = std::make_unique<Ground>();

	m_pBoss = std::make_unique<Boss>();
	m_pPlayer = std::make_unique<Player>();

}

void GameMain::Create()
{
}

void GameMain::Update()
{
	m_pGround->Update();
	m_pPlayer->Update();

	m_pBoss->SetTargetPos(m_pPlayer->GetPosition());
	//ターゲットの座標.
	m_pBoss->Update();
}

void GameMain::LateUpdate()
{
	CameraManager::GetInstance().LateUpdate();
}


void GameMain::Draw()
{

	Shadow::Begin();
	m_pGround->DrawDepth();
	Shadow::End();
	m_pGround->Draw();

	m_pBoss->Draw();
	m_pPlayer->Draw();


}

HRESULT GameMain::LoadData()
{
	// ここで実際のロード処理を行うか、Create()に集約されているのであればE_NOTIMPLのままでもよい
	// 現在のGameMainではCreate()でほとんどのInit/Load処理が行われているようです
	return S_OK; // 成功を返す
}
