#include "UIGameOver.h"
#include "02_UIObject/UILoader/UILoader.h"
#include "Utility/Color/Color.h"

#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Game//04_Time//Time.h"
#include "Game/05_InputDevice/Input.h"

UIGameOver::UIGameOver()
	: m_pUIs		()
	, m_Select		( GameOverItems::Start )
	, m_InitAlpha	( 0.6 )
	, m_SelectAlpha	( m_InitAlpha )
	, m_AnimReturn	( false )
	, m_AnimeSpeed	( 0.25f )
{
	UILoader::LoadFromJson("Data\\Image\\Sprite\\UIData\\GameOver.json", m_pUIs);
}

//----------------------------------------------------------------.

UIGameOver::~UIGameOver()
{
}

//----------------------------------------------------------------.

void UIGameOver::Update()
{
	SelectUpdate();
}

//----------------------------------------------------------------.

void UIGameOver::LateUpdate()
{
	for (auto& ui : m_pUIs)
	{
		ui->Update();
		SelectLateUpdate(ui);
	}
}

//----------------------------------------------------------------.

void UIGameOver::Draw()
{
	for (auto& ui : m_pUIs)
	{
		DirectX11::GetInstance().SetDepth(false);
		ui->Draw();
		DirectX11::GetInstance().SetDepth(false);
	}
}

//----------------------------------------------------------------.

void UIGameOver::SelectUpdate()
{	
	if (Input::IsKeyDown('W')
	|| Input::IsKeyDown(VK_UP)
	|| Input::IsButtonDown(XInput::Key::Up)) 
	{
		InitAnim(GameOverItems::Start);
	}
	else if (Input::IsKeyDown('S')
	||	Input::IsKeyDown(VK_DOWN)
	||	Input::IsButtonDown(XInput::Key::Down) ) 
	{
		InitAnim(GameOverItems::End);
	}
	
	AnimUpdate();
}

//----------------------------------------------------------------.

void UIGameOver::SelectLateUpdate(std::shared_ptr<UIObject> ui)
{
	if (ui->GetUIName() == "SelectFrame_1" || ui->GetUIName() == "S_Continue_0")
	{
		if (m_Select == 0) { 
			ui->SetColor(ColorUtil::RGBA(ColorPreset::Selected, m_SelectAlpha)); 
		}
		else {
			ui->SetColor(ColorPreset::UnActive); 
		}
	}
	else if (ui->GetUIName() == "SelectFrame_0" || ui->GetUIName() == "S_End_0")
	{
		if (m_Select == 1) {
			ui->SetColor(ColorUtil::RGBA(ColorPreset::Selected, m_SelectAlpha));
		}
		else {
			ui->SetColor(ColorPreset::UnActive);
		}
	}
}

//----------------------------------------------------------------.

void UIGameOver::InitAnim(GameOverItems item)
{
	m_Select = item;
	m_SelectAlpha = m_InitAlpha;
	m_AnimReturn = false;
}

//----------------------------------------------------------------.

void UIGameOver::AnimUpdate()
{
	if (m_SelectAlpha < m_InitAlpha) { m_AnimReturn = false; }
	else if (1.f < m_SelectAlpha) { m_AnimReturn = true; }

	if (m_AnimReturn) {
		m_SelectAlpha = m_SelectAlpha - Time::GetDeltaTime() * m_AnimeSpeed;
	}
	else {
		m_SelectAlpha = m_SelectAlpha + Time::GetDeltaTime() * m_AnimeSpeed;
	}
}

//----------------------------------------------------------------.

UIGameOver::GameOverItems UIGameOver::GetSelected()
{
	return m_Select;
}