#include "GameMain.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h"	// 地面Static.

#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"

#include "Game//01_GameObject//02_UIObject/UIGameMain/UIGameMain.h"

#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/LockOnCamera/LockOnCamera.h"

#include "Game/05_InputDevice/Input.h"

#include "Graphic/Shadow/Shadow.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"

#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/Debug\CollisionVisualizer\CollisionVisualizer.h"
#include "SceneManager/SceneManager.h"
#include "Singleton/PostEffectManager/PostEffectManager.h"
#include "Game/04_Time/Time.h"

#if _DEBUG
#include "System/Singleton/ImGui/CImGuiManager.h"
#endif

#include <algorithm> // std::min のために必要

// コンストラクタ.
GameMain::GameMain()
	: SceneBase		()
	, m_spCamera	( nullptr )
	, m_spLight		(std::make_shared<DirectionLight>())
	, m_upBoss		(std::make_unique<Boss>())
	, m_upPlayer	(std::make_unique<Player>())
	, m_upUI		(std::make_shared<UIGameMain>())
    , m_TimeLimit   (10800.0f*Time::GetInstance().GetDeltaTime())
{
	Initialize();
    Time::GetInstance().StartTimer(m_TimeLimit);
}

// デストラクタ.
GameMain::~GameMain()
{
	SoundManager::GetInstance().AllStop();
}

void GameMain::Initialize()
{
	// ライト設定.
	m_spLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
	LightManager::AttachDirectionLight(m_spLight);

	m_upGround = std::make_unique<Ground>();

	// カメラ設定.
	m_spCamera = std::make_shared<LockOnCamera>(std::ref(*m_upPlayer), std::ref(*m_upBoss));
	CameraManager::GetInstance().SetCamera(m_spCamera);

	SoundManager::GetInstance().Play("8-bit_Aggressive1", true);
	SoundManager::GetInstance().SetVolume("8-bit_Aggressive1", 9000);
}

void GameMain::Create()
{
}

void GameMain::Update()
{
	Input::Update();
	m_upGround->Update();
	m_upPlayer->SetTargetPos(m_upBoss->GetPosition());
	m_upBoss->SetTargetPos(m_upPlayer->GetPosition());
	m_upPlayer->Update();
	m_upBoss->Update();

	m_upUI->SetBossHP(m_upBoss->GetMaxHP(), m_upBoss->GetHP());
	m_upUI->SetCombo(m_upPlayer->GetCombo());
	m_upUI->SetPlayerHP(m_upPlayer->GetMaxHP(), m_upPlayer->GetHP());
 	m_upUI->SetPlayerUlt(m_upPlayer->GetMaxUltValue(), m_upPlayer->GetUltValue());
 	m_upUI->SetTime(Time::GetInstance().GetTimerProgress());
    m_upUI->Update();

#if _DEBUG
    ImGui::Begin("Gamemain Debug");
    bool gray = PostEffectManager::GetInstance().IsGray();
    if (ImGui::Checkbox("GrayScale", &gray)) {
        PostEffectManager::GetInstance().SetGray(gray);
    }
    ImGui::Text("Time: %.3f", Time::GetInstance().GetTimerProgress());
    if (ImGui::Button("Restart Timer")) {
        Time::GetInstance().StartTimer(m_TimeLimit);
    }
    
    ImGui::End();
#endif
}

void GameMain::LateUpdate()
{
	m_upPlayer->LateUpdate();
	m_upBoss->LateUpdate();
	CameraManager::GetInstance().LateUpdate();

	m_upUI->LateUpdate();

	CollisionDetector::GetInstance().ExecuteCollisionDetection();

}


void GameMain::Draw()
{
    Shadow::Begin();
    m_upGround->DrawDepth();
    Shadow::End();

    const bool useGray = PostEffectManager::GetInstance().IsGray();
    if (useGray) {
        PostEffectManager::GetInstance().BeginSceneRender();
    }

    m_upGround->Draw();
    m_upBoss->Draw();
    m_upPlayer->Draw();

    m_upPlayer->Draw();

    if (useGray) {
        PostEffectManager::GetInstance().DrawToBackBuffer();
    }

    m_upUI->Draw();

    CollisionVisualizer::GetInstance().Draw();
}

HRESULT GameMain::LoadData()
{
	// ここで実際のロード処理を行うか、Create()に集約されているのであればE_NOTIMPLのままでもよい
	// 現在のGameMainではCreate()でほとんどのInit/Load処理が行われているようです
	return S_OK; // 成功を返す
}
