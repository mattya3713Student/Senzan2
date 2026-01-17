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

#include "System/Utility/Transform/Transform.h"

#include "SceneManager/SceneManager.h"

// コンストラクタ.
MattyaTestScene::MattyaTestScene()
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

// デストラクタ.
MattyaTestScene::~MattyaTestScene()
{
	CollisionDetector::GetInstance().UnregisterCollider(m_TestAttackCollision.get());
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

	//m_TestPressCollision->SetColor(Color::eColor::Cyan);
	//m_TestPressCollision->SetHeight(60.0f);
	//m_TestPressCollision->SetRadius(20.0f);
	//m_TestPressCollision->SetPositionOffset(0.f,1.5f,0.f);
	//m_TestPressCollision->SetMyMask(eCollisionGroup::Press);
	//m_TestPressCollision->SetTarGetTargetMask(eCollisionGroup::Press);
	//CollisionDetector::GetInstance().RegisterCollider(*m_TestPressCollision);

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

    // Create external transform used only for debug visualization of collider rotation
    m_pDebugExternalTransform = std::make_unique<Transform>();
    m_pDebugExternalTransform->Position = DirectX::XMFLOAT3(0.0f, 1.5f, 50.0f);
    m_pDebugExternalTransform->Scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    m_pDebugExternalTransform->Quaternion = DirectX::XMFLOAT4(0.0f,0.0f,0.0f,1.0f);
    m_pDebugExternalTransform->UpdateRotationFromQuaternion();

    // By default set external transform pointer to the attack collider so debug rotation affects calculation
    if (m_TestAttackCollision) {
        m_TestAttackCollision->SetExternalTransformPointer(m_pDebugExternalTransform.get());
    }
}

void MattyaTestScene::Create()
{
}

void MattyaTestScene::Update()
{
    Input::Update();
	m_upGround->Update();
	//m_upPlayer->SetTargetPos(m_upBoss.get()->GetPosition());
	//m_upPlayer->Update();
    m_pCamera->Update();
	//m_upBoss->Update();
	//m_upBoss->SetTargetPos(m_upPlayer->GetPosition());

	m_upUI->SetBossHP(m_upBoss->GetMaxHP(), m_upBoss->GetHP());
	m_upUI->SetCombo(m_upPlayer->GetCombo());
	m_upUI->SetPlayerHP(m_upPlayer->GetMaxHP(), m_upPlayer->GetHP());
	m_upUI->SetPlayerUlt(m_upPlayer->GetMaxUltValue(), m_upPlayer->GetUltValue());

	m_upUI->Update();
}

void MattyaTestScene::LateUpdate()
{
	//m_upPlayer->LateUpdate();
	//m_upBoss->LateUpdate();
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
	//m_upPlayer->Draw();
	//m_upBoss->Draw();

    if (m_TestPressCollision && m_TestPressCollision->GetActive()) {
        m_TestPressCollision->SetDebugInfo();
    }
    if (m_TestAttackCollision && m_TestAttackCollision->GetActive()) {
        m_TestAttackCollision->SetDebugInfo();
    }

#if _DEBUG
    //if (ImGui::Begin(IMGUI_JP("MattyaTestScene Collider Debug"))) {
    //    ImGui::Checkbox(IMGUI_JP("Use External Transform"), &m_DebugUseExternalTransform);
    //    ImGui::DragFloat(IMGUI_JP("Debug Rot X (deg)"), &m_DebugRotDegX, 1.0f, -180.0f, 180.0f);
    //    ImGui::DragFloat(IMGUI_JP("Debug Rot Y (deg)"), &m_DebugRotDegY, 1.0f, -180.0f, 180.0f);
    //    ImGui::DragFloat(IMGUI_JP("Debug Rot Z (deg)"), &m_DebugRotDegZ, 1.0f, -180.0f, 180.0f);

    //    // Apply to external transform quaternion
    //    if (m_pDebugExternalTransform) {
    //        DirectX::XMVECTOR rotRad = DirectX::XMVectorSet(
    //            DirectX::XMConvertToRadians(m_DebugRotDegX),
    //            DirectX::XMConvertToRadians(m_DebugRotDegY),
    //            DirectX::XMConvertToRadians(m_DebugRotDegZ),
    //            0.0f);
    //        DirectX::XMVECTOR qx = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1,0,0,0), DirectX::XMVectorGetX(rotRad));
    //        DirectX::XMVECTOR qy = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,1,0,0), DirectX::XMVectorGetY(rotRad));
    //        DirectX::XMVECTOR qz = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,0,1,0), DirectX::XMVectorGetZ(rotRad));
    //        DirectX::XMVECTOR q = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionMultiply(qz, qy), qx);
    //        DirectX::XMStoreFloat4(&m_pDebugExternalTransform->Quaternion, q);
    //        m_pDebugExternalTransform->UpdateRotationFromQuaternion();

    //        if (m_DebugUseExternalTransform) {
    //            if (m_TestAttackCollision) m_TestAttackCollision->SetExternalTransformPointer(m_pDebugExternalTransform.get());
    //        }
    //        else {
    //            if (m_TestAttackCollision) m_TestAttackCollision->SetExternalTransformPointer(nullptr);
    //        }
    //    }

    //    ImGui::End();
    //}
#endif

	m_upUI->Draw();
	CollisionVisualizer::GetInstance().Draw();
}

HRESULT MattyaTestScene::LoadData()
{
	return S_OK; // 成功を返す
}
