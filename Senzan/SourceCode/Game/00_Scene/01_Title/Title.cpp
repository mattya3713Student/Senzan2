#include "Title.h"
#include "02_UIObject/UIObject.h"
#include "02_UIObject/UITitle/UITitle.h"
#include "Game/05_InputDevice/Input.h"
#include "Singleton/ResourceManager/ResourceManager.h"
#include "SceneManager/SceneManager.h"

Title::Title()
	: SceneBase		(  )
	, m_pUI			(std::make_shared<UITitle>())
{
}

Title::~Title()
{
}

void Title::Initialize()
{
}

void Title::Create()
{
}

void Title::Update()
{
	Input::Update();

	m_pUI->Update();


	if (Input::IsKeyDown(VK_SPACE)
	||	Input::IsKeyDown('C')
	||	Input::IsButtonDown(XInput::Key::B))
	{
		if (m_pUI->GetSelected() == m_pUI->TitleItems::Start) {
			SceneManager::LoadScene(eList::GameMain);
		}
		else if (m_pUI->GetSelected() == m_pUI->TitleItems::End) {
			DestroyWindow(ResourceManager::GethWnd());
		}
	}
}

void Title::LateUpdate()
{
	m_pUI->LateUpdate();
}

void Title::Draw()
{
	m_pUI->Draw();
}

