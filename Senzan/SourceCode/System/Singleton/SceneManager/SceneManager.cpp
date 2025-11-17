#include "SceneManager.h"

#include "Graphic/DirectX/DirectX9/DirectX9.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"

#include "Game/00_Scene/00_Base/SceneBase.h"
#include "Game/00_Scene/01_Title/Title.h"
#include "Game/00_Scene/02_GameMain/GameMain.h"
#include "Game/00_Scene/03_GameOver/GameOver.h"
#include "Game/00_Scene/04_Ending/Ending.h"

#include "Game/00_Scene/Ex_Test/00_mattya3713/MattyaTestScene.h"
#include "Game/00_Scene/Ex_Test/01_memeu1101/MemeuTestScene.h"
#include "Game/00_Scene/Ex_Test/02_L/LTestScene.h"

SceneManager::SceneManager()
	: m_pScene	( std::make_unique<GameMain>() )
	, m_hWnd	()
	, m_pBuffer	( nullptr )
{

}

SceneManager::~SceneManager()
{
}

void SceneManager::Create(HWND hWnd)
{
	m_hWnd = hWnd;
	LoadData();
}


void SceneManager::LoadData()
{
	// 最初にロードするシーンを環境に応じて決定
	eList initial_scene;

	// --- 環境ごとの初期シーン設定 ---

#ifdef MATTYA_PC
	initial_scene = eList::Mattya;
#elif defined(MEMEU_PC)
	initial_scene = eList::Memeu;
#elif defined(L_PC)
	initial_scene = eList::L;
#elif _DEBUG
	// 上記の環境マクロが定義されておらず、デバッグビルドの場合
	// 通常は開発中のメインシーンから開始
	initial_scene = eList::GameMain;

#else
	// リリースビルドまたは不明な環境の場合、タイトルから開始
	initial_scene = eList::Title;

#endif 

	LoadScene(initial_scene);
}

void SceneManager::Update()
{
	SceneManager& pI = GetInstance();
	pI.m_pScene->Update();
	pI.m_pScene->LateUpdate();
}

void SceneManager::Draw()
{
	SceneManager& pI = GetInstance();
	pI.m_pScene->Draw();
}

void SceneManager::LoadScene(eList Scene)
{
	SceneManager& pI = GetInstance();
	pI.m_pScene.reset();

	//シーン作成.
	pI.MakeScene(Scene);
	pI.m_pScene->Create();
}

HWND SceneManager::GetHWND() const
{
	return m_hWnd;
}

//シーン作成.
void SceneManager::MakeScene(eList Scene)
{
	switch (Scene)
	{
		case eList::Title:
			m_pScene = std::make_unique<Title>();
			break;
		case eList::GameMain:
			m_pScene = std::make_unique<GameMain>();
			break;
		case eList::GameOver:
			m_pScene = std::make_unique<GameMain>();
			break;
		case eList::Ending:
			m_pScene = std::make_unique<GameMain>();
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
#endif // _DEBUG
		case eList::MAX:
		default:
			break;
	}
}
