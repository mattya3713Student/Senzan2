#include "UIEnding.h"
#include "02_UIObject/UILoader/UILoader.h"
#include "Utility/Color/Color.h"

#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Game//04_Time//Time.h"
#include "Game/05_InputDevice/Input.h"


UIEnding::UIEnding()
	: m_pUIs		( ) 
	, m_InitAlpha	( 0.6f )
	, m_SelectAlpha	( m_InitAlpha )
	, m_AnimReturn	( false )
	, m_AnimeSpeed	( 0.3f )
{
	UILoader::LoadFromJson("Data\\Image\\Sprite\\UIData\\Ending.json", m_pUIs);
}

//----------------------------------------------------------------.

UIEnding::~UIEnding()
{
}

//----------------------------------------------------------------.

void UIEnding::Update()
{
	SelectUpdate();
}

//----------------------------------------------------------------.

void UIEnding::LateUpdate()
{
	for (auto& ui : m_pUIs)
	{
		ui->Update();
		SelectLateUpdate(ui);
	}
}

//----------------------------------------------------------------.

void UIEnding::Draw()
{
	for (auto& ui : m_pUIs)
	{
		DirectX11::GetInstance().SetDepth(false);
		ui->Draw();
		DirectX11::GetInstance().SetDepth(true);
	}
}

//----------------------------------------------------------------.

void UIEnding::SelectUpdate()
{
	AnimUpdate();
}

//----------------------------------------------------------------.

void UIEnding::SelectLateUpdate(std::shared_ptr<UIObject> ui)
{
	if (ui->GetUIName() == "SelectFrame_0" || ui->GetUIName() == "S_End_0")
	{
		ui->SetColor(ColorUtil::RGBA(ColorPreset::Selected, m_SelectAlpha));
	}
}

//----------------------------------------------------------------.

void UIEnding::AnimUpdate()
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