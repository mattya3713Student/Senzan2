#include "UITitle.h"
#include "02_UIObject/UILoader/UILoader.h"
#include "Utility/Color/Color.h"

#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Game//04_Time//Time.h"
#include "Game/05_InputDevice/Input.h"
#include "SceneManager/SceneManager.h"
#include "Singleton/ResourceManager/ResourceManager.h"

UITitle::UITitle()
	: m_pUIs		()
	, m_Select		( 0 )
	, m_InitAlpha	( 0.6 )
	, m_SelectAlpha	( m_InitAlpha )
	, m_AnimReturn	( false )
	, m_AnimeSpeed	( 0.25f )
{
	UILoader::LoadFromJson("Data\\Image\\Sprite\\UIData\\Title.json", m_pUIs);
}

//----------------------------------------------------------------.

UITitle::~UITitle()
{
}

//----------------------------------------------------------------.

void UITitle::Update()
{
	SelectUpdate();
}

//----------------------------------------------------------------.

void UITitle::LateUpdate()
{
	for (auto& ui : m_pUIs)
	{
		ui->Update();
		SelectLateUpdate(ui);
	}
}

//----------------------------------------------------------------.

void UITitle::Draw()
{
	for (auto& ui : m_pUIs)
	{
		DirectX11::GetInstance().SetDepth(false);
		ui->Draw();
		DirectX11::GetInstance().SetDepth(false);
	}
}

//----------------------------------------------------------------.

void UITitle::SelectUpdate()
{	
	if (Input::IsKeyDown('W')
	|| Input::IsKeyDown(VK_UP)
	|| Input::IsButtonDown(XInput::Key::Up)) 
	{
		InitAnim(TitleItems::Start);
	}
	else if (Input::IsKeyDown('S')
	||	Input::IsKeyDown(VK_DOWN)
	||	Input::IsButtonDown(XInput::Key::Down) ) 
	{
		InitAnim(TitleItems::End);
	}

	if (Input::IsKeyDown(VK_SPACE)
	|| Input::IsKeyDown('C')
	|| Input::IsButtonDown(XInput::Key::B))
	{
		if (m_Select == TitleItems::Start) {
			SceneManager::LoadScene(eList::GameMain);
		}
		else if (m_Select == TitleItems::End) {
			DestroyWindow(ResourceManager::GethWnd());
		}
	}
	
	AnimUpdate();
}

//----------------------------------------------------------------.

void UITitle::SelectLateUpdate(std::shared_ptr<UIObject> ui)
{
	if (ui->GetUIName() == "SelectFrame_1" || ui->GetUIName() == "S_Start_0")
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

void UITitle::InitAnim(TitleItems item)
{
	m_Select = item;
	m_SelectAlpha = m_InitAlpha;
	m_AnimReturn = false;
}

//----------------------------------------------------------------.

void UITitle::AnimUpdate()
{
	if (m_SelectAlpha < m_InitAlpha) { m_AnimReturn = false; }
	else if (0.98f < m_SelectAlpha) { m_AnimReturn = true; }

	if (m_AnimReturn) {
		m_SelectAlpha = m_SelectAlpha - Time::GetDeltaTime() * m_AnimeSpeed;
	}
	else {
		m_SelectAlpha = m_SelectAlpha + Time::GetDeltaTime() * m_AnimeSpeed;
	}
}