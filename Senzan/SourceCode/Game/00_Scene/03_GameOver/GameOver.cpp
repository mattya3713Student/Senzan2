#include "GameOver.h"
#include "02_UIObject/UIGameOver/UIGameOver.h"
#include "Game/05_InputDevice/Input.h"
#include "Singleton/ResourceManager/ResourceManager.h"
#include "SceneManager/SceneManager.h"

GameOver::GameOver()
	: SceneBase		(  )
	, m_pUI(std::make_shared<UIGameOver>())
{
	Initialize();
}

GameOver::~GameOver()
{
	SoundManager::GetInstance().AllStop();
}

void GameOver::Initialize()
{
	SoundManager::GetInstance().Play("Over", true);
	SoundManager::GetInstance().SetVolume("Over", 8000);
}

void GameOver::Create()
{
}

void GameOver::Update()
{
	Input::Update();

	m_pUI->Update();

	if (Input::IsKeyDown(VK_SPACE)
		|| Input::IsKeyDown('C')
		|| Input::IsButtonDown(XInput::Key::B))
	{
		if (m_pUI->GetSelected() == m_pUI->Items::Continue) {
			SceneManager::LoadScene(eList::GameMain);
			return;
		}
		else if (m_pUI->GetSelected() == m_pUI->Items::End) {
			SceneManager::LoadScene(eList::Title);
			return;
		}
	}
}

void GameOver::LateUpdate()
{
	m_pUI->LateUpdate();
}

void GameOver::Draw()
{
	m_pUI->Draw();
}

