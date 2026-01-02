#include "SceneManager.h"

#include "Graphic/DirectX/DirectX9/DirectX9.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "02_UIObject/Fade/FadeManager.h"

#include "Game/00_Scene/00_Base/SceneBase.h"
#include "Game/00_Scene/01_Title/Title.h"
#include "Game/00_Scene/02_GameMain/GameMain.h"
#include "Game/00_Scene/03_GameOver/GameOver.h"
#include "Game/00_Scene/04_Ending/Ending.h"

#include "Game/00_Scene/Ex_Test/00_mattya3713/MattyaTestScene.h"
#include "Game/00_Scene/Ex_Test/01_memeu1101/MemeuTestScene.h"
#include "Game/00_Scene/Ex_Test/02_L/LTestScene.h"
#include "Game/00_Scene/Ex_Test/03_UIEditor/UIEditor.h"

#if _DEBUG
#include "ImGui/CImGuiManager.h"
#endif // _DEBUG


SceneManager::SceneManager()
	: m_pScene	( std::make_unique<GameMain>() )
	, m_pBuffer	( nullptr )

#if _DEBUG
	, m_DebugFirstScene()
#endif // _DEBUG
{

}

SceneManager::~SceneManager()
{
}

void SceneManager::LoadData()
{
	// 最初にロードするシーンを環境に応じて決定
	eList initial_scene = eList::Title;

	// --- 環境ごとの初期シーン設定 ---

#if _DEBUG
#ifdef MATTYA_PC
	initial_scene = eList::Mattya;
#elif defined(MEMEU_PC)
	initial_scene = eList::Memeu;
#elif defined(L_PC)
	initial_scene = eList::L;
#elif _DEBUG
	// 上記の環境マクロが定義されておらず、デバッグビルドの場合
	// 通常は開発中のメインシーンから開始
	//initial_scene = eList::GameMain;

#else
	// リリースビルドまたは不明な環境の場合、タイトルから開始
	initial_scene = eList::Title;

#endif 
#endif // _DEBUG.

	LoadScene(initial_scene);
}

void SceneManager::Update()
{
	SceneManager& pI = GetInstance();
	pI.m_pScene->Update();
	pI.m_pScene->LateUpdate();

#if _DEBUG
	ImGui::Begin("Scene");
	ImGui::Text(pI.GetSceneName(pI.m_CurrentSceneID));

	if (ImGui::Button("Title")) { LoadScene(eList::Title); }
	if (ImGui::Button("GameMain")) { LoadScene(eList::GameMain); }
	if (ImGui::Button("Ending")) { LoadScene(eList::Ending); }
	if (ImGui::Button("GameOver")) { LoadScene(eList::GameOver); }

	ImGui::Separator();
	if (ImGui::Button("Mattya")) { LoadScene(eList::Mattya); }
	if (ImGui::Button("Memeu")) { LoadScene(eList::Memeu); }
	if (ImGui::Button("L")) { LoadScene(eList::L); }

	ImGui::Separator();
	if (ImGui::Button("UIEditor")) { LoadScene(eList::UIEditor); }

	ImGui::End();
#endif // _DEBUG

	if (FadeManager::GetInstance().IsFadeCompleted(Fade::FadeType::FadeOut)) 
	{
		pI.MakeScene(pI.m_NextScene);
	}


	FadeManager::GetInstance().Update();
}

void SceneManager::Draw()
{
	SceneManager& pI = GetInstance();
	pI.m_pScene->Draw();
	FadeManager::GetInstance().Draw();
}

void SceneManager::LoadScene(eList Scene)
{
	SceneManager& pI = GetInstance();
	pI.m_NextScene = Scene;

	FadeManager::GetInstance().StartFade(Fade::FadeType::FadeOut);
}

//シーン作成.
void SceneManager::MakeScene(eList Scene)
{
	m_pScene.reset();

#if _DEBUG
	Time::GetInstance().SetWorldTimeScale(1.0f);
	m_CurrentSceneID = Scene;
#endif
	switch (Scene)
	{
		case eList::Title:
			m_pScene = std::make_unique<Title>();
			break;
		case eList::GameMain:
			m_pScene = std::make_unique<GameMain>();
			break;
		case eList::GameOver:
			m_pScene = std::make_unique<GameOver>();
			break;
		case eList::Ending:
			m_pScene = std::make_unique<Ending>();
			break;

			
#if _DEBUG
			// テスト用シーン.
		case eList::Mattya:
			m_pScene = std::make_unique<MattyaTestScene>();
			break;
		case eList::Memeu:
			m_pScene = std::make_unique<MemeuTestScene>();
			break;
		case eList::L:
			m_pScene = std::make_unique<LTestScene>();
			break;
		case eList::UIEditor:
			m_pScene = std::make_unique<UIEditor>();
			break;
#endif // _DEBUG
		case eList::MAX:
		default:
			break;
	}
	m_pScene->Create();
	FadeManager::GetInstance().StartFade(Fade::FadeType::FadeIn);
}
