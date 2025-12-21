#include "Ending.h"
#include "02_UIObject/UIEnding/UIEnding.h"
#include "Game/05_InputDevice/Input.h"
#include "Singleton/ResourceManager/ResourceManager.h"
#include "SceneManager/SceneManager.h"

Ending::Ending()
	: SceneBase		(  )
	, m_pUI			(std::make_shared<UIEnding>())
{
	Initialize();
}

Ending::~Ending()
{
	SoundManager::GetInstance().AllStop();
}

void Ending::Initialize()
{
	SoundManager::GetInstance().Play("‰F’ˆ”òsŽm‚ªÅŒã‚ÉŒ©‚½‚à‚Ì", true);
	SoundManager::GetInstance().SetVolume("‰F’ˆ”òsŽm‚ªÅŒã‚ÉŒ©‚½‚à‚Ì", 9000);
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
		return;
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

