#include "MattyaTestScene_Parry.h"

#include "Game/05_InputDevice/Input.h"

#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/ThirdPersonCamera/PlayerThirdPersonCamera.h"
#include "Game/02_Camera/LockOnCamera/LockOnCamera.h"

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

// コンストラクタ.
MattyaTestScene_Parry::MattyaTestScene_Parry()
	: SceneBase()
	, m_pCamera()
	, m_pLight(std::make_shared<DirectionLight>())
	, m_TestPressCollision(std::make_unique<CapsuleCollider>())
	, m_TestAttackCollision(std::make_unique<CapsuleCollider>())
	, m_upPlayer(std::make_unique<Player>())
	, m_upBoss(std::make_unique<Boss>())
	, m_upGround(std::make_unique<Ground>())
{
	Initialize();
}

// デストラクタ.
MattyaTestScene_Parry::~MattyaTestScene_Parry()
{
}

void MattyaTestScene_Parry::Initialize()
{
	m_pCamera = std::make_shared<PlayerThirdPersonCamera>(std::ref(*m_upPlayer));
	// カメラ設定.
	m_pCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, -50.0f));
	m_pCamera->SetLook(DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f));
	CameraManager::GetInstance().SetCamera(m_pCamera);

	// ライト設定.
	m_pLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
	LightManager::AttachDirectionLight(m_pLight);

	m_upGround = std::make_unique<Ground>();
}

void MattyaTestScene_Parry::Create()
{
}

void MattyaTestScene_Parry::Update()
{
	// ウィンドウとして独立させる
	ImGui::Begin("Performance Monitor");

	// 1. 基本的なFPS表示
	float fps = ImGui::GetIO().Framerate;
	float ms = 1000.0f / fps;

	ImGui::Text("Average: %.1f FPS (%.3f ms/frame)", fps, ms);

	// 2. 状態に応じた警告表示
	if (fps < 50.0f) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: Low FPS!");
	}
	else {
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: Stable");
	}

	ImGui::Separator();
	ImGui::End();

	Input::Update();
	m_upGround->Update();
	m_upPlayer->SetTargetPos(m_TestAttackCollision.get()->GetPosition());
	m_upPlayer->Update();
	m_upBoss->Update();
}

void MattyaTestScene_Parry::LateUpdate()
{
	m_upPlayer->LateUpdate();
	m_upBoss->LateUpdate();
	CameraManager::GetInstance().LateUpdate();

	static float s_TimeCounter = 0.0f;
	float deltaTime = Time::GetInstance().GetDeltaTime();
	s_TimeCounter += deltaTime;
	const float PERIOD = 200.0f;		// 周期.
	const float AMPLITUDE = 3.5f;	// 揺れ幅.
	float sine_y = AMPLITUDE * sinf(DirectX::XM_2PI * (s_TimeCounter / PERIOD));
	const float BASE_OFFSET_X = 0.0f;
	const float BASE_OFFSET_Y = 1.5f;
	const float BASE_OFFSET_Z = 10.0f;
	float final_offset_y = BASE_OFFSET_Y + sine_y;

	// オフセット位置を設定
	m_TestAttackCollision->SetPositionOffset(
		BASE_OFFSET_X,
		final_offset_y,
		BASE_OFFSET_Z
	);

	CollisionDetector::GetInstance().ExecuteCollisionDetection();
}


void MattyaTestScene_Parry::Draw()
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

	CollisionVisualizer::GetInstance().Draw();
}

HRESULT MattyaTestScene_Parry::LoadData()
{
	// ここで実際のロード処理を行うか、Create()に集約されているのであればE_NOTIMPLのままでもよい
	// 現在のGameMainではCreate()でほとんどのInit/Load処理が行われているようです
	return S_OK; // 成功を返す
}
