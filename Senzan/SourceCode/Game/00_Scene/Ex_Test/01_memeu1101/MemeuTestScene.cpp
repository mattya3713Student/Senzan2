#include "MemeuTestScene.h"

#include "System/Singleton/CameraManager/CameraManager.h"
#include "Game/02_Camera/CameraBase.h"
#include "Game/02_Camera/ThirdPersonCamera/ThirdPersonCamera.h"
#include "ResourceManager/SpriteManager/SpriteManager.h"

#include "Graphic/Shadow/Shadow.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/00_Ground/Ground.h"	// 地面Static.
#include "Singleton/ResourceManager/EffectManager/EffekseerManager.h"

#include "ImGui/CImGuiManager.h"

#include <algorithm> // std::min のために必要

namespace {
	::Effekseer::Handle m_EffectHandle = -1;
}

// コンストラクタ.
MemeuTestScene::MemeuTestScene()
	: SceneBase()
	, m_pCamera(std::make_shared<ThirdPersonCamera>())
	, m_pLight(std::make_shared<DirectionLight>())
	, m_TestSprite(std::make_shared<UIObject>())
	, m_Timer		(0.f)
	, m_Interval	(180.f*Time::GetInstance().GetDeltaTime())
{
	Initialize();
}

// デストラクタ.
MemeuTestScene::~MemeuTestScene()
{
}

void MemeuTestScene::Initialize()
{
	// カメラ設定.
	m_pCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, -5.0f));
	m_pCamera->SetLook(DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f));
	CameraManager::GetInstance().SetCamera(m_pCamera);

	// ライト設定.
	m_pLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
	LightManager::AttachDirectionLight(m_pLight);

	m_pGround = std::make_unique<Ground>();

	SpriteManager::LoadSprites();
}

void MemeuTestScene::Create()
{
}

void MemeuTestScene::Update()
{
	m_pGround->Update();

	// ----- エフェクト繰り返し処理 -----
	

	ImGui::Begin("effectpos");
	ImGui::DragFloat3("pos",&effekpos.x);
	ImGui::End();


	// 時間加算
	m_Timer += Time::GetInstance().GetDeltaTime();

	// 一定時間ごとに再生
	if (m_Timer >= m_Interval)
	{
		m_Timer = 0.f;

		auto effect = EffectResource::GetResource("test");

		m_EffectHandle =
			EffekseerManager::GetInstance().GetManager()
			->Play(effect, effekpos.x, effekpos.y, effekpos.z);

	}

	EffekseerManager::GetInstance().UpdateHandle(m_EffectHandle);
	
}

void MemeuTestScene::LateUpdate()
{
	CameraManager::GetInstance().LateUpdate();

}


void MemeuTestScene::Draw()
{
	Shadow::Begin();
	m_pGround->DrawDepth();
	Shadow::End();
	m_pGround->Draw();
	EffekseerManager::GetInstance().RenderHandle(m_EffectHandle, m_pCamera.get());
}

HRESULT MemeuTestScene::LoadData()
{
	// ここで実際のロード処理を行うか、Create()に集約されているのであればE_NOTIMPLのままでもよい
	// 現在のGameMainではCreate()でほとんどのInit/Load処理が行われているようです
	return S_OK; // 成功を返す
}
