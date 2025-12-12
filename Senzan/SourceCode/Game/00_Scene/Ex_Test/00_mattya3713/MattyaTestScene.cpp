#include "MattyaTestScene.h"

#include "Game/05_InputDevice/Input.h"

#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/ThirdPersonCamera/PlayerThirdPersonCamera.h"

#include "Graphic/Shadow/Shadow.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h"	// 地面Static.
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"	// プレイヤー.

#include "Game/03_Collision/00_Core/ColliderBase.h"	
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"

#include "Game/04_Time/Time.h"

#include "System/Singleton/Debug/CollisionVisualizer/CollisionVisualizer.h"

#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/ImGui/CImGuiManager.h"

// コンストラクタ.
MattyaTestScene::MattyaTestScene()
	: SceneBase()
	, m_pCamera()
	, m_pLight(std::make_shared<DirectionLight>())
	, m_TestPressCollision(std::make_unique<CapsuleCollider>())
	, m_TestAttackCollision(std::make_unique<CapsuleCollider>())
	, m_pPlayer(std::make_unique<Player>())
	, m_pGround(std::make_unique<Ground>())
{
	Initialize();
}

// デストラクタ.
MattyaTestScene::~MattyaTestScene()
{
}

void MattyaTestScene::Initialize()
{
	m_pCamera = std::make_shared<PlayerThirdPersonCamera>(std::ref(*m_pPlayer));
	// カメラ設定.
	m_pCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, -50.0f));
	m_pCamera->SetLook(DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f));
	CameraManager::GetInstance().SetCamera(m_pCamera);

	// ライト設定.
	m_pLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
	LightManager::AttachDirectionLight(m_pLight);

	m_pGround = std::make_unique<Ground>();

	m_TestPressCollision->SetColor(Color::eColor::Cyan);
	m_TestPressCollision->SetHeight(2.0f);
	m_TestPressCollision->SetRadius(0.5f);
	m_TestPressCollision->SetPositionOffset(0.f,1.5f,0.f);
	m_TestPressCollision->SetMyMask(eCollisionGroup::Press);
	m_TestPressCollision->SetTarGetTargetMask(eCollisionGroup::Press);
	CollisionDetector::GetInstance().RegisterCollider(*m_TestPressCollision);

	m_TestAttackCollision->SetColor(Color::eColor::Red);
	m_TestAttackCollision->SetHeight(2.0f);
	m_TestAttackCollision->SetRadius(0.5f);
	m_TestAttackCollision->SetPositionOffset(0.f,1.5f,3.f);
	m_TestAttackCollision->SetMyMask(eCollisionGroup::Enemy_Attack);

	m_TestAttackCollision->SetTarGetTargetMask(eCollisionGroup::Player_Damage 
		| eCollisionGroup::Player_Dodge
		| eCollisionGroup::Player_JustDodge);

	CollisionDetector::GetInstance().RegisterCollider(*m_TestAttackCollision);
}

void MattyaTestScene::Create()
{
}

void MattyaTestScene::Update()
{
	Input::Update();
	m_pGround->Update();
	m_pPlayer->SetTargetPos(m_TestAttackCollision.get()->GetPosition());
	m_pPlayer->Update();
}

void MattyaTestScene::LateUpdate()
{
	m_pPlayer->LateUpdate();
	CameraManager::GetInstance().LateUpdate();

	static float s_TimeCounter = 0.0f;
	float deltaTime = Time::GetInstance().GetDeltaTime();
	s_TimeCounter += deltaTime;
	const float PERIOD = 200.0f;		// 周期.
	const float AMPLITUDE = 3.5f;	// 揺れ幅.
	float sine_y = AMPLITUDE * sinf(DirectX::XM_2PI * (s_TimeCounter / PERIOD));
	const float BASE_OFFSET_X = 0.0f;
	const float BASE_OFFSET_Y = 1.5f;
	const float BASE_OFFSET_Z = 3.0f;
	float final_offset_y = BASE_OFFSET_Y + sine_y;

	// オフセット位置を設定
	m_TestAttackCollision->SetPositionOffset(
		BASE_OFFSET_X,
		final_offset_y,
		BASE_OFFSET_Z
	);

	CollisionDetector::GetInstance().ExecuteCollisionDetection();
}


void MattyaTestScene::Draw()
{
	Shadow::Begin();
	m_pGround->DrawDepth();
	Shadow::End();
	m_pGround->Draw();
	m_pPlayer->Draw();

	m_TestPressCollision->SetDebugInfo();
	m_TestAttackCollision->SetDebugInfo();

	CollisionVisualizer::GetInstance().Draw();
}

HRESULT MattyaTestScene::LoadData()
{
	// ここで実際のロード処理を行うか、Create()に集約されているのであればE_NOTIMPLのままでもよい
	// 現在のGameMainではCreate()でほとんどのInit/Load処理が行われているようです
	return S_OK; // 成功を返す
}
