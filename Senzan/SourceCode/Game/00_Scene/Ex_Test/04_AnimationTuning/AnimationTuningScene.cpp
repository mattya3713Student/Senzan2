#include "AnimationTuningScene.h"

#include "Game/05_InputDevice/Input.h"
#include "Game/01_GameObject/02_UIObject/UIGameMain/UIGameMain.h"
#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/ThirdPersonCamera/PlayerThirdPersonCamera.h"
#include "Game/02_Camera/LockOnCamera/LockOnCamera.h"

#include "Graphic/Shadow/Shadow.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"

#include "Game/03_Collision/00_Core/ColliderBase.h"
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"

#include "Game/04_Time/Time.h"

#include "System/Singleton/Debug/CollisionVisualizer/CollisionVisualizer.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Singleton/Debug/AnimationTuningManager/AnimationTuningManager.h"

#include "SceneManager/SceneManager.h"

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

AnimationTuningScene::AnimationTuningScene()
    : SceneBase()
    , m_pCamera()
    , m_pLight(std::make_shared<DirectionLight>())
    , m_TestPressCollision(std::make_unique<CapsuleCollider>())
    , m_TestAttackCollision(std::make_unique<CapsuleCollider>())
    , m_upPlayer(std::make_unique<Player>())
    , m_upBoss(std::make_unique<Boss>())
    , m_upGround(std::make_unique<Ground>())
    , m_upUI(std::make_shared<UIGameMain>())
{
    Initialize();
}

AnimationTuningScene::~AnimationTuningScene()
{
    CollisionDetector::GetInstance().UnregisterCollider(m_TestAttackCollision.get());
}

void AnimationTuningScene::Initialize()
{
    m_pCamera = std::make_shared<LockOnCamera>(std::ref(*m_upPlayer), std::ref(*m_upBoss));
    m_pCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, -50.0f));
    m_pCamera->SetLook(DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f));
    CameraManager::GetInstance().SetCamera(m_pCamera);

    m_pLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
    LightManager::AttachDirectionLight(m_pLight);

    m_upGround = std::make_unique<Ground>();

    m_TestAttackCollision->SetColor(Color::eColor::Red);
    m_TestAttackCollision->SetAttackAmount(50.0f);
    m_TestAttackCollision->SetHeight(2.0f);
    m_TestAttackCollision->SetRadius(0.5f);
    m_TestAttackCollision->SetPositionOffset(0.f,1.5f,50.f);
    m_TestAttackCollision->SetMyMask(eCollisionGroup::Enemy_Attack);

    m_TestAttackCollision->SetTarGetTargetMask(eCollisionGroup::Player_Damage 
        | eCollisionGroup::Player_Dodge
        | eCollisionGroup::Player_JustDodge);

    CollisionDetector::GetInstance().RegisterCollider(*m_TestAttackCollision);
}

void AnimationTuningScene::Create()
{
}

void AnimationTuningScene::Update()
{
    // Performance monitor
    ImGui::Begin("Performance Monitor");
    float fps = ImGui::GetIO().Framerate;
    float ms = 1000.0f / fps;
    ImGui::Text("Average: %.1f FPS (%.3f ms/frame)", fps, ms);
    if (fps < 50.0f) ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f), "Warning: Low FPS!");
    else ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f), "Status: Stable");
    ImGui::Separator();
    ImGui::End();

    Input::Update();
    m_upGround->Update();
    m_upPlayer->SetTargetPos(m_TestAttackCollision.get()->GetPosition());
    m_upPlayer->Update();
    m_upBoss->Update();

    m_upUI->SetBossHP(m_upBoss->GetMaxHP(), m_upBoss->GetHP());
    m_upUI->SetCombo(m_upPlayer->GetCombo());
    m_upUI->SetPlayerHP(m_upPlayer->GetMaxHP(), m_upPlayer->GetHP());
    m_upUI->SetPlayerUlt(m_upPlayer->GetMaxUltValue(), m_upPlayer->GetUltValue());

    m_upUI->Update();

#if _DEBUG
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

    // Draw Debug Tuning panel via manager
    AnimationTuningManager::GetInstance().UpdateImGui();
}

void AnimationTuningScene::LateUpdate()
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

void AnimationTuningScene::Draw()
{
    Shadow::Begin();
    m_upGround->DrawDepth();
    Shadow::End();
    m_upGround->Draw();
    m_upPlayer->Draw();
    m_upBoss->Draw();

    m_TestPressCollision->SetDebugInfo();
    m_TestAttackCollision->SetDebugInfo();

    m_TestPressCollision->SetDebugInfo();

    m_upUI->Draw();
    CollisionVisualizer::GetInstance().Draw();
}

HRESULT AnimationTuningScene::LoadData()
{
    return S_OK;
}
