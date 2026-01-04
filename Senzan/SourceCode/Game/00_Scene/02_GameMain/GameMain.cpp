#include "GameMain.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h" 	// 地面Static.

#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"

#include "Game//01_GameObject//02_UIObject/UIGameMain/UIGameMain.h"

#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/LockOnCamera/LockOnCamera.h"

#include "Game/05_InputDevice/Input.h"

#include "Graphic/Shadow/Shadow.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"

#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/Debug\CollisionVisualizer\CollisionVisualizer.h"
#include "SceneManager/SceneManager.h"

#include <algorithm> // std::min のために必要

#if _DEBUG
// Boss state headers for debug state switching UI
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossMoveState/BossMoveState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossStompState/BossStompState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossSlashState/BossSlashState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossShoutState/BossShoutState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossSpecialState/BossSpecialState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossLaserState/BossLaserState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossDeadState/BossDeadState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeState/BossChargeState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "../../Data/ImGui/Library/imgui.h"
#endif


// コンストラクタ.
GameMain::GameMain()
	: SceneBase		()
	, m_spCamera	( nullptr )
	, m_spLight		(std::make_shared<DirectionLight>())
	, m_upBoss		(std::make_unique<Boss>())
	, m_upPlayer	(std::make_unique<Player>())
	, m_upUI		(std::make_shared<UIGameMain>())
	, TotalTime		(0.0f)
{
	Initialize();
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

	m_upUI->Update();

#if _DEBUG
	// Simple ImGui panel for boss debug: state switching + collider toggles
	ImGui::Begin("Boss Debug");

	ImGui::Text("Change Boss State:");
	if (ImGui::Button("Idol")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossIdolState>(m_upBoss.get())); }
	ImGui::SameLine();
	if (ImGui::Button("Move")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>(m_upBoss.get())); }
	if (ImGui::Button("Stomp")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossStompState>(m_upBoss.get())); }
	ImGui::SameLine();
	if (ImGui::Button("Slash")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossSlashState>(m_upBoss.get())); }
	if (ImGui::Button("ChargeSlash")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossChargeSlashState>(m_upBoss.get())); }
	ImGui::SameLine();
	if (ImGui::Button("Shout")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossShoutState>(m_upBoss.get())); }
	if (ImGui::Button("Special")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossSpecialState>(m_upBoss.get())); }
	ImGui::SameLine();
	if (ImGui::Button("Laser")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossLaserState>(m_upBoss.get())); }
	if (ImGui::Button("Charge")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossChargeState>(m_upBoss.get())); }
	ImGui::SameLine();
	if (ImGui::Button("Throwing")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossThrowingState>(m_upBoss.get())); }
	if (ImGui::Button("Dead")) { m_upBoss->GetStateMachine()->ChangeState(std::make_shared<BossDeadState>(m_upBoss.get())); }

	ImGui::Separator();
	ImGui::Text("Boss Info:");
	ImGui::Text("HP: %.1f / %.1f", m_upBoss->GetHP(), m_upBoss->GetMaxHP());
	ImGui::Text("State Ptr: %p", (void*)m_upBoss->GetStateMachine()->m_pCurrentState.get());

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
	m_upGround->Draw();

	m_upBoss->Draw();
	m_upPlayer->Draw();

	m_upUI->Draw();

	CollisionVisualizer::GetInstance().Draw();
}

HRESULT GameMain::LoadData()
{
	return S_OK; // 成功を返す
}
