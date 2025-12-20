#include "Ending.h"
#include "02_UIObject/UIEnding/UIEnding.h"
#include "Game/05_InputDevice/Input.h"
#include "SceneManager/SceneManager.h"

Ending::Ending()
	: SceneBase		(  )
	, m_pUI			(std::make_shared<UIEnding>())
{
}

Ending::~Ending()
{
}

void Ending::Initialize()
{
}

void Ending::Create()
{
}

void Ending::Update()
{
	Input::Update();

	m_pUI->Update();

	if (Input::IsKeyDown(VK_SPACE)
		|| Input::IsKeyDown('C')
		|| Input::IsButtonDown(XInput::Key::B))
	{
		SceneManager::LoadScene(eList::Title);
	}
}

void Ending::LateUpdate()
{
	m_pUI->LateUpdate();
}

void Ending::Draw()
{
	m_pUI->Draw();
}

