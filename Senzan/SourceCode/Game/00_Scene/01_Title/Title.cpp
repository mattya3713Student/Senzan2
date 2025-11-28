#include "Title.h"
#include "02_UIObject/UIObject.h"
#include "02_UIObject/UILoader/UILoader.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"

namespace {
	// UIî•ñƒpƒX.
	std::string ScenePath = "Data\\Image\\Sprite\\UIData\\Title.json";
}


Title::Title()
	: SceneBase		(  )
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
	UILoader::LoadFromJson(ScenePath, m_pUIs);
}

void Title::Update()
{
	for (auto& ui : m_pUIs) { ui->Update(); }
}

void Title::LateUpdate()
{
}

void Title::Draw()
{
	for (auto& ui : m_pUIs) 
	{ 
		DirectX11::GetInstance().SetDepth(false);
		ui->Draw();
		DirectX11::GetInstance().SetDepth(false);
	}
}

