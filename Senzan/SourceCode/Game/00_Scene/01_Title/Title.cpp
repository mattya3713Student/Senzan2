#include "Title.h"
#include "02_UIObject/UIObject.h"
#include "02_UIObject/UITitle/UITitle.h"
#include "Game/05_InputDevice/Input.h"

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
}

void Title::LateUpdate()
{
	m_pUI->LateUpdate();
}

void Title::Draw()
{
	m_pUI->Draw();
}

