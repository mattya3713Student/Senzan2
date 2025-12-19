<<<<<<< HEAD
E¿#include "UIEditor.h"
=======
#include "UIEditor.h"
>>>>>>> main
#include "FileManager/FileManager.h"
#include "ResourceManager/SpriteManager/SpriteManager.h"
#include "Game/05_InputDevice/Input.h"
#include "Game/03_Collision/Sprite/SpriteCollider.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
<<<<<<< HEAD
#include "Game//04_Time//Time.h"
#include "Singleton/SceneManager/SceneManager.h"
#include "ResourceManager/ResourceManager.h"
=======
>>>>>>> main

#if _DEBUG
#include "ImGui/CImGuiManager.h"
#endif // _DEBUG


<<<<<<< HEAD
// jsonå‹ã‚’ä½œæE
using Json = nlohmann::json;

namespace {
	char m_NewSceneName[64] = ""; // æ–°è¦ä½œæEç”¨ãƒãƒƒãƒ•ã‚¡ã‚’ãƒ¡ãƒ³ãƒå¤‰æ•°ã«è¿½åŠ 
}

//-----------------------------------------------------------------------.
=======
// jsonŒ^‚ğì¬
using Json = nlohmann::json;


namespace {
	char m_NewSceneName[64] = ""; // V‹Kì¬—pƒoƒbƒtƒ@‚ğƒƒ“ƒo•Ï”‚É’Ç‰Á
}

>>>>>>> main

UIEditor::UIEditor()
{
}
UIEditor::~UIEditor()
{
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::Create()
{
	SelectSceneLoad("Title");
}

//-----------------------------------------------------------------------.

=======

//=============================================================================
//		ì¬ˆ—
//=============================================================================
void UIEditor::Create()
{
	SelectSceneLoad("Title");
	SelectInit();
}


//=============================================================================
//		ƒf[ƒ^“Ç
//=============================================================================
>>>>>>> main
HRESULT UIEditor::LoadData()
{
	return E_NOTIMPL;
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

=======

//=============================================================================
//		‰Šú‰»ˆ—
//=============================================================================
>>>>>>> main
void UIEditor::Initialize()
{
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::Update()
{
	// ã‚­ãƒ¼å…¥åŠE
	KeyInput();

	// ã‚·ãƒ¼ãƒ³ã‚’é¸æŠã™ã‚E
	ImGuiSelectScene();

	//--------------------------------------------------------------
	//		UIã®è¿½åŠ ã¨å‰Šé™¤
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIã®è¿½åŠ ãƒ»å‰Šé™¤"));
	// ã‚·ãƒ¼ãƒ³ã«UIã‚’è¿½åŠ 
	AddDeleteSprite();
	// ç”»åƒåã‚’å¤‰æ›´ã—åå‰è¢«ã‚Šã«å¯¾å‡¦
=======

//-----------------------------------------------------------------------------
//		UI‘I‘ğ‚É‰¼•Ï”“™‚ğ‰Šú‰»‚·‚é
//-----------------------------------------------------------------------------
void UIEditor::SelectInit()
{
	m_PatternNo = POINTS(0, 0);
	m_PatternMax = POINTS(1, 1);
	m_PatternAuto = false;
}


//=============================================================================
//		XVˆ—
//=============================================================================
void UIEditor::Update()
{
	// ƒL[“ü—Í
	KeyInput();

	// ƒV[ƒ“‚ğ‘I‘ğ‚·‚é
	ImGuiSelectScene();

	//--------------------------------------------------------------
	//		UI‚Ì’Ç‰Á‚Æíœ
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI‚Ì’Ç‰ÁEíœ"));
	// ƒV[ƒ“‚ÉUI‚ğ’Ç‰Á
	AddDeleteSprite();
	// ‰æ‘œ–¼‚ğ•ÏX‚µ–¼‘O”í‚è‚É‘Îˆ
>>>>>>> main
	RenameUIObjects();
	ImGui::End();

	//--------------------------------------------------------------
<<<<<<< HEAD
	//		UIèª¿æ•´
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIã‚¨ãƒE‚£ã‚¿ãƒ¼ã‚¦ã‚£ãƒ³ãƒ‰ã‚¦"));
	// UIãƒªã‚¹ãƒˆãEæ¤œç´¢é–¢æ•°
	ImGuiSearchUI();
	
	//-----------------------------------------------------------
	//		é¸æŠä¸­ã®ã‚ªãƒ–ã‚¸ã‚§ã‚¯ãƒˆãEç·¨é›E
	//-----------------------------------------------------------
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// é¸æŠã•ã‚Œã¦ãE‚‹UIã‚’è¡¨ç¤º
		ImGui::Text(IMGUI_JP("é¸æŠã•ã‚Œã¦ãE‚‹UI: %s"), m_pUIs[m_SelectedUIIndex]->GetUIName().c_str());


		// åº§æ¨™ãEèª¿æ•´
		ImGuiPosEdit(m_pUIs[m_SelectedUIIndex]);
		// Zåº§æ¨™ã‚’åŸºæº–ã«ã‚½ãƒ¼ãƒE
		SortBySpritePosZ(m_pUIs[m_SelectedUIIndex]);

		// ç”»åƒæƒ…å ±ã®èª¿æ•´
		ImGuiInfoEdit(m_pUIs[m_SelectedUIIndex]);
		// ããEä»–ãEæƒE ±ã®èª¿æ•´
		ImGuiEtcInfoEdit(m_pUIs[m_SelectedUIIndex]);

		HighLightUI(m_pUIs[m_SelectedUIIndex]);
=======
	//		UI’²®
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIƒGƒfƒBƒ^[ƒEƒBƒ“ƒhƒE"));
	// UIƒŠƒXƒg‚ÌŒŸõŠÖ”
	ImGuiSearchUI();
	
	//-----------------------------------------------------------
	//		‘I‘ğ’†‚ÌƒIƒuƒWƒFƒNƒg‚Ì•ÒW
	//-----------------------------------------------------------
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// ‘I‘ğ‚³‚ê‚Ä‚¢‚éUI‚ğ•\¦
		ImGui::Text(IMGUI_JP("‘I‘ğ‚³‚ê‚Ä‚¢‚éUI: %s"), m_pUIs[m_SelectedUIIndex]->GetResourceName().c_str());


		// À•W‚Ì’²®
		ImGuiPosEdit(m_pUIs[m_SelectedUIIndex]);
		// ZÀ•W‚ğŠî€‚Éƒ\[ƒg
		SortBySpritePosZ(m_pUIs[m_SelectedUIIndex]);

		// ‰æ‘œî•ñ‚Ì’²®
		ImGuiInfoEdit(m_pUIs[m_SelectedUIIndex]);
		// ‚»‚Ì‘¼‚Ìî•ñ‚Ì’²®
		ImGuiEtcInfoEdit(m_pUIs[m_SelectedUIIndex]);
>>>>>>> main
	}

	ImGui::End();

	//--------------------------------------------------------------
<<<<<<< HEAD
	//		ä¿å­˜ã™ã‚E
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UIä¿å­˜ã‚¦ã‚£ãƒ³ãƒ‰ã‚¦"));
	if (ImGui::Button(IMGUI_JP("UIã‚’ä¿å­E))) {
=======
	//		•Û‘¶‚·‚é
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI•Û‘¶ƒEƒBƒ“ƒhƒE"));
	if (ImGui::Button(IMGUI_JP("UI‚ğ•Û‘¶"))) {
>>>>>>> main
		SaveScene();
		m_MoveAny = false;
	}
	ImGui::End();
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.
=======
>>>>>>> main

void UIEditor::LateUpdate() 
{
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

=======

//-----------------------------------------------------------------------------
//		ƒL[“ü—Íˆ—
//-----------------------------------------------------------------------------
>>>>>>> main
void UIEditor::KeyInput()
{
	m_DragValue = 1.f;
	if (GetAsyncKeyState(VK_SHIFT) && 0x8000) { m_DragValue *= 0.01f; }
	if (GetAsyncKeyState(VK_SPACE) && 0x8000) { m_DragValue *= 0.001f; }
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::Draw()
{
	if (m_pUIs.empty()) { return; }
	for (size_t i = 0; i < m_pUIs.size(); ++i) 
	{
		DirectX11::GetInstance().SetDepth(false);
		m_pUIs[i]->Draw();
		DirectX11::GetInstance().SetDepth(true);
	}
}

//-----------------------------------------------------------------------.

=======

//=============================================================================
//		•`‰æˆ—
//=============================================================================
void UIEditor::Draw()
{
	if (m_pUIs.empty()) { return; }
	DirectX11::GetInstance().SetDepth(false);
	for (size_t i = 0; i < m_pUIs.size(); ++i) { m_pUIs[i]->Draw(); }
	DirectX11::GetInstance().SetDepth(true);
}


//-----------------------------------------------------------------------------
//		‘I‘ğ‚µ‚½ƒV[ƒ“‚ÌUI‚ğ“Ç‚İ‚İ
//-----------------------------------------------------------------------------
>>>>>>> main
void UIEditor::SelectSceneLoad(const std::string& sceneName)
{
	m_pSprite2Ds.clear();
	for (auto sprite : m_pSprite2Ds) sprite.reset();
	for (auto ui : m_pUIs) ui.reset();
	m_pUIs.clear();
	m_SpritePosList.clear();

	m_CurrentSceneName = sceneName;
	m_ScenePath = "Data\\Image\\Sprite\\UIData\\" + sceneName + ".json";

<<<<<<< HEAD
	// JSONèª­ã¿è¾¼ã¿
	Json jsonData = FileManager::JsonLoad(m_ScenePath);

	// ç©ºãªã‚‰åEæœŸUIã‚Eå€‹è¿½åŠ 
=======
	// JSON“Ç‚İ‚İ
	Json jsonData = FileManager::JsonLoad(m_ScenePath);

	// ‹ó‚È‚ç‰ŠúUI‚ğ1ŒÂ’Ç‰Á
>>>>>>> main
	if (jsonData.is_null() || jsonData.empty()) {
		std::shared_ptr<Sprite2D> sprite = std::make_shared<Sprite2D>();
		std::shared_ptr<UIObject> ui = std::make_shared<UIObject>();

<<<<<<< HEAD
		sprite->Initialize("Data\\Image\\Sprite\\Other\\White.png"); // é»’ç”»åƒã‚’åˆæœŸã§å‡ºãE
=======
		sprite->Initialize("Data\\Image\\Sprite\\Other\\White.png"); // •‰æ‘œ‚ğ‰Šú‚Åo‚·
>>>>>>> main
		ui->AttachSprite(sprite);
		ui->SetPosition(0.f, 0.f, 0.f);

		m_pSprite2Ds.push_back(sprite);
		m_pUIs.push_back(ui);
		m_SpritePosList.push_back(ui->GetPosition());

<<<<<<< HEAD
		// ã¤ãE§ã«SaveScene()ã§æ›¸ãè¾¼ã‚“ã§ãŠã
		SaveScene();
	}

	// ä¿å­˜ã•ã‚ŒãŸUIãƒEEã‚¿ã‚’èª­ã¿è¾¼ã¿ã€å±•é–‹
	for (auto& [imageName, spriteArray] : jsonData.items()) {
		// æ‹¡å¼µå­ãŒ .json ãªã‚‰ã‚¹ã‚­ãƒEE
=======
		// ‚Â‚¢‚Å‚ÉSaveScene()‚Å‘‚«‚ñ‚Å‚¨‚­
		SaveScene();
	}

	// •Û‘¶‚³‚ê‚½UIƒf[ƒ^‚ğ“Ç‚İ‚İA“WŠJ
	for (auto& [imageName, spriteArray] : jsonData.items()) {
		// Šg’£q‚ª .json ‚È‚çƒXƒLƒbƒv
>>>>>>> main
		std::string::size_type dotPos = imageName.find_last_of('.');
		if (dotPos != std::string::npos) {
			std::string ext = imageName.substr(dotPos);
			if (ext == ".json" || ext == ".JSON") continue;
		}
		
<<<<<<< HEAD
		// ã‚¹ãƒ—ãƒ©ã‚¤ãƒˆå–å¾E
		std::shared_ptr<Sprite2D> Sprite = SpriteManager::GetSprite2D(GetBaseName(imageName));
		if (!Sprite) {
			MessageBoxA(NULL, ("ã‚¹ãƒ—ãƒ©ã‚¤ãƒˆãŒè¦‹ã¤ã‹ã‚Šã¾ã›ã‚“: " + imageName).c_str(), "Error", MB_OK);
			continue;
		}

		// å„UIã‚¤ãƒ³ã‚¹ã‚¿ãƒ³ã‚¹ã‚’å±•é–‹
		for (auto& value : spriteArray) {
			std::shared_ptr<UIObject> ui = std::make_shared<UIObject>();

			ui->SetUIName(imageName);
=======
		// ƒXƒvƒ‰ƒCƒgæ“¾
		std::shared_ptr<Sprite2D> Sprite = SpriteManager::GetSprite2D(GetBaseName(imageName));
		if (!Sprite) {
			MessageBoxA(NULL, ("ƒXƒvƒ‰ƒCƒg‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ: " + imageName).c_str(), "Error", MB_OK);
			continue;
		}

		// ŠeUIƒCƒ“ƒXƒ^ƒ“ƒX‚ğ“WŠJ
		for (auto& value : spriteArray) {
			std::shared_ptr<UIObject> ui = std::make_shared<UIObject>();

>>>>>>> main
			ui->SetPosition(DirectX::XMFLOAT3(value["Pos"]["x"], value["Pos"]["y"], value["Pos"]["z"]));
			ui->SetColor(DirectX::XMFLOAT4(value["Color"]["x"], value["Color"]["y"], value["Color"]["z"], value["Color"]["a"]));
			ui->SetAlpha(value["Alpha"]);
			ui->SetScale(DirectX::XMFLOAT3(value["Scale"]["x"], value["Scale"]["y"], value["Scale"]["z"]));
			ui->SetPivot(DirectX::XMFLOAT2(value["Pivot"]["x"], value["Pivot"]["y"]));
			ui->SetRotation(DirectX::XMFLOAT3(value["Rotate"]["x"], value["Rotate"]["y"], value["Rotate"]["z"]));
			ui->SetDrawSize(DirectX::XMFLOAT2(value["Disp"]["w"], value["Disp"]["h"]));
			
<<<<<<< HEAD
			// ãƒªã‚¹ãƒˆã«è¿½åŠ 
=======
			// ‚È‚¢‚Ì‚©HH
			//ui->SetBase(DirectX::XMFLOAT2(value["Base"]["w"], value["Base"]["h"]));
			//ui->SetStride(DirectX::XMFLOAT2(value["Stride"]["w"], value["Stride"]["h"]));

			// ƒŠƒXƒg‚É’Ç‰Á
>>>>>>> main
			m_pSprite2Ds.push_back(Sprite);
			ui->AttachSprite(Sprite);
			m_pUIs.push_back(ui);
			m_SpritePosList.push_back(ui->GetPosition());
		}
	}
<<<<<<< HEAD
	// Zåº§æ¨™ã‚’åŸºæº–ã«ã‚½ãƒ¼ãƒˆã™ã‚E
=======
	// ZÀ•W‚ğŠî€‚Éƒ\[ƒg‚·‚é
>>>>>>> main
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](std::shared_ptr<UIObject> a, std::shared_ptr<UIObject> b) {
		return a->GetPosition().z < b->GetPosition().z;
		});
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

=======

//-----------------------------------------------------------------------------
//		Œ»İƒV[ƒ“‚ÌUIî•ñ‚ğ•Û‘¶
//-----------------------------------------------------------------------------
>>>>>>> main
HRESULT UIEditor::SaveScene()
{
	Json jsonData;
	for (size_t i = 0; i < m_pUIs.size(); ++i)
	{
<<<<<<< HEAD
		std::string imageName = m_pUIs[i]->GetUIName();

		// ç”»åƒåã”ã¨ã®ãƒªã‚¹ãƒˆã«UIæƒE ±ã‚’è¿½åŠ 
=======
		std::string imageName = m_pUIs[i]->GetResourceName();

		// ‰æ‘œ–¼‚²‚Æ‚ÌƒŠƒXƒg‚ÉUIî•ñ‚ğ’Ç‰Á
>>>>>>> main
		Json SpriteState;
		SpriteState["Pos"]["x"] = m_pUIs[i]->GetPosition().x;
		SpriteState["Pos"]["y"] = m_pUIs[i]->GetPosition().y;
		SpriteState["Pos"]["z"] = m_pUIs[i]->GetPosition().z;
		SpriteState["Disp"]["w"] = m_pUIs[i]->GetDrawSize().x;
		SpriteState["Disp"]["h"] = m_pUIs[i]->GetDrawSize().y;
<<<<<<< HEAD
=======
		//SpriteState["Base"]["w"] = m_pUIs[i]->GetSpriteData().Base.w;
		//SpriteState["Base"]["h"] = m_pUIs[i]->GetSpriteData().Base.h;
		//SpriteState["Stride"]["w"] = m_pUIs[i]->GetSpriteData().Stride.w;
		//SpriteState["Stride"]["h"] = m_pUIs[i]->GetSpriteData().Stride.h;
>>>>>>> main

		SpriteState["Color"]["x"] = m_pUIs[i]->GetColor().x;
		SpriteState["Color"]["y"] = m_pUIs[i]->GetColor().y;
		SpriteState["Color"]["z"] = m_pUIs[i]->GetColor().z;
		SpriteState["Color"]["a"] = m_pUIs[i]->GetColor().w;
		SpriteState["Alpha"] = m_pUIs[i]->GetAlpha();

		SpriteState["Scale"]["x"] = m_pUIs[i]->GetScale().x;
		SpriteState["Scale"]["y"] = m_pUIs[i]->GetScale().y;
		SpriteState["Scale"]["z"] = m_pUIs[i]->GetScale().z;
		SpriteState["Pivot"]["x"] = m_pUIs[i]->GetPivot().x;
		SpriteState["Pivot"]["y"] = m_pUIs[i]->GetPivot().y;
		SpriteState["Rotate"]["x"] = m_pUIs[i]->GetRotation().x;
		SpriteState["Rotate"]["y"] = m_pUIs[i]->GetRotation().y;
		SpriteState["Rotate"]["z"] = m_pUIs[i]->GetRotation().z;

<<<<<<< HEAD
		// jsonData[ç”»åƒå] ã«é…åEã¨ã—ã¦è¿½åŠ 
=======
		// jsonData[‰æ‘œ–¼] ‚É”z—ñ‚Æ‚µ‚Ä’Ç‰Á
>>>>>>> main
		jsonData[imageName].push_back(SpriteState);
	}
	std::string outPath = "Data\\Image\\Sprite\\UIData\\" + m_CurrentSceneName + ".json";
	if (!SUCCEEDED(FileManager::JsonSave(outPath, jsonData))) return E_FAIL;

	return S_OK;
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::SortBySpritePosZ(std::shared_ptr<UIObject> object)
{
	// ä½•ã‚‚åº§æ¨™ã«é–¢ã—ã¦å¤‰æ›´ãŒãªãE ´åˆæ—©æœŸãƒªã‚¿ãƒ¼ãƒ³
	if (!m_MovedSpritePos) { return; }

	// ã‚½ãƒ¼ãƒˆå‰ã®é¸æŠã•ã‚Œã¦ãEŸ UI ã®ãƒã‚¤ãƒ³ã‚¿ã‚’ä¿å­E
	std::shared_ptr<UIObject> pPreviousSelectedUI = object;

	// Zåº§æ¨™ã‚’åŸºæº–ã«ã‚½ãƒ¼ãƒˆã™ã‚E
	std::sort(m_pUIs.begin(), m_pUIs.end(),
		[](const std::shared_ptr<UIObject>& a, const std::shared_ptr<UIObject>& b) {
			if (!a || !b) return false; // UIObjectãŒnullptrãªã‚‰å¾Œã‚ã«
=======

//-----------------------------------------------------------------------------
//		ZÀ•W‚ğŒ³‚Éƒ\[ƒg‚·‚éŠÖ”
//-----------------------------------------------------------------------------
void UIEditor::SortBySpritePosZ(std::shared_ptr<UIObject> object)
{
	// ‰½‚àÀ•W‚ÉŠÖ‚µ‚Ä•ÏX‚ª‚È‚¢ê‡‘ŠúƒŠƒ^[ƒ“
	if (!m_MovedSpritePos) { return; }

	// ƒ\[ƒg‘O‚Ì‘I‘ğ‚³‚ê‚Ä‚¢‚½ UI ‚Ìƒ|ƒCƒ“ƒ^‚ğ•Û‘¶
	std::shared_ptr<UIObject> pPreviousSelectedUI = object;

	// ZÀ•W‚ğŠî€‚Éƒ\[ƒg‚·‚é
	std::sort(m_pUIs.begin(), m_pUIs.end(),
		[](const std::shared_ptr<UIObject>& a, const std::shared_ptr<UIObject>& b) {
			if (!a || !b) return false; // UIObject‚ªnullptr‚È‚çŒã‚ë‚É
>>>>>>> main

			const auto& posA = a->GetPosition();
			const auto& posB = b->GetPosition();

			return posA.z < posB.z;
		});

<<<<<<< HEAD
	// ã‚½ãƒ¼ãƒˆå¾Œã«ã€ä»¥å‰é¸æŠã•ã‚Œã¦ãEŸ UI ãŒã¾ã ãƒªã‚¹ãƒˆã«å­˜åœ¨ã™ã‚‹ã‹ç¢ºèªã—ã€åEé¸æŠE
=======
	// ƒ\[ƒgŒã‚ÉAˆÈ‘O‘I‘ğ‚³‚ê‚Ä‚¢‚½ UI ‚ª‚Ü‚¾ƒŠƒXƒg‚É‘¶İ‚·‚é‚©Šm”F‚µAÄ‘I‘ğ
>>>>>>> main
	if (pPreviousSelectedUI != nullptr)
	{
		auto it = std::find(m_pUIs.begin(), m_pUIs.end(), pPreviousSelectedUI);
		if (it != m_pUIs.end()) {
<<<<<<< HEAD
			// å†é¸æŠE
			m_SelectedUIIndex = static_cast<int>(std::distance(m_pUIs.begin(), it));
		}
		else {
			// ä»¥å‰é¸æŠã•ã‚Œã¦ãEŸUIãŒãƒªã‚¹ãƒˆã«ãªãE ´åE
			object = nullptr;
			m_SelectedUIIndex = 0;
=======
			// Ä‘I‘ğ
			m_SelectedUIIndex = static_cast<int>(std::distance(m_pUIs.begin(), it));
		}
		else {
			// ˆÈ‘O‘I‘ğ‚³‚ê‚Ä‚¢‚½UI‚ªƒŠƒXƒg‚É‚È‚¢ê‡
			object = nullptr;
			m_SelectedUIIndex = 0; // ‚Ü‚½‚Í“KØ‚ÈƒfƒtƒHƒ‹ƒg’l
>>>>>>> main
		}
	}

	m_MovedSpritePos = false;
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::ImGuiSelectScene()
{
	ImGui::Begin(IMGUI_JP("ã‚·ãƒ¼ãƒ³ç®¡çE));

	// æ–°è¦ã‚·ãƒ¼ãƒ³ã®ä½œæE
	ImGui::InputText(IMGUI_JP("æ–°è¦ã‚·ãƒ¼ãƒ³åE), m_NewSceneName, IM_ARRAYSIZE(m_NewSceneName));
	if (ImGui::Button(IMGUI_JP("æ–°è¦ã‚·ãƒ¼ãƒ³ä½œæE"))) {
		std::string newPath = "Data\\Image\\Sprite\\UIData\\" + std::string(m_NewSceneName) + ".json";
		if (!std::filesystem::exists(newPath)) {
			std::ofstream ofs(newPath);
			ofs << "{}"; // ç©ºã®JSONã‚’æ›¸ãè¾¼ã‚€
=======

//-----------------------------------------------------------------------------
//		ImGui‚ğ—p‚¢‚½ƒV[ƒ“‘I‘ğŠÖ”
//-----------------------------------------------------------------------------
void UIEditor::ImGuiSelectScene()
{
	ImGui::Begin(IMGUI_JP("ƒV[ƒ“ŠÇ—"));

	// V‹KƒV[ƒ“‚Ìì¬
	ImGui::InputText(IMGUI_JP("V‹KƒV[ƒ“–¼"), m_NewSceneName, IM_ARRAYSIZE(m_NewSceneName));
	if (ImGui::Button(IMGUI_JP("V‹KƒV[ƒ“ì¬"))) {
		std::string newPath = "Data\\Image\\Sprite\\UIData\\" + std::string(m_NewSceneName) + ".json";
		if (!std::filesystem::exists(newPath)) {
			std::ofstream ofs(newPath);
			ofs << "{}"; // ‹ó‚ÌJSON‚ğ‘‚«‚Ş
>>>>>>> main
			ofs.close();
		}
		m_CurrentSceneName = m_NewSceneName;
		SelectSceneLoad(m_CurrentSceneName);
		m_MoveAny = false;
	}

	ImGui::Separator();
<<<<<<< HEAD
	ImGui::Text(IMGUI_JP("æ—¢å­˜ãEã‚·ãƒ¼ãƒ³"));

	static std::string sceneToDelete; // å‰Šé™¤å€™è£œãEã‚·ãƒ¼ãƒ³åE
	static bool showDeleteConfirm = false; // å‰Šé™¤ç¢ºèªãƒ€ã‚¤ã‚¢ãƒ­ã‚°è¡¨ç¤ºãƒ•ãƒ©ã‚°
=======
	ImGui::Text(IMGUI_JP("Šù‘¶‚ÌƒV[ƒ“"));

	static std::string sceneToDelete; // íœŒó•â‚ÌƒV[ƒ“–¼
	static bool showDeleteConfirm = false; // íœŠm”Fƒ_ƒCƒAƒƒO•\¦ƒtƒ‰ƒO
>>>>>>> main

	for (const auto& entry : std::filesystem::directory_iterator("Data\\Image\\Sprite\\UIData\\")) {
		if (entry.path().extension() == ".json") {
			std::string sceneName = entry.path().stem().string();

			ImGui::PushID(sceneName.c_str());

			if (ImGui::Button(sceneName.c_str())) {
				m_CurrentSceneName = sceneName;
				SelectSceneLoad(m_CurrentSceneName);
				m_MoveAny = false;
			}
			ImGui::SameLine();

<<<<<<< HEAD
			if (ImGui::Button(IMGUI_JP("å‰Šé™¤"))) {
=======
			if (ImGui::Button(IMGUI_JP("íœ"))) {
>>>>>>> main
				sceneToDelete = sceneName;
				showDeleteConfirm = true;
			}

			ImGui::PopID();
		}
	}

<<<<<<< HEAD
	// å‰Šé™¤ç¢ºèªãƒ¢ãƒ¼ãƒ€ãƒ«
	if (showDeleteConfirm) {
		ImGui::OpenPopup(IMGUI_JP("å‰Šé™¤ç¢ºèªE));
	}
	if (ImGui::BeginPopupModal(IMGUI_JP("å‰Šé™¤ç¢ºèªE), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n%s", sceneToDelete.c_str(), IMGUI_JP("ã‚’å‰Šé™¤ã—ã¾ã™ã‹EE));
		ImGui::Separator();

		if (ImGui::Button(IMGUI_JP("ã¯ãE), ImVec2(120, 0))) {
=======
	// íœŠm”Fƒ‚[ƒ_ƒ‹
	if (showDeleteConfirm) {
		ImGui::OpenPopup(IMGUI_JP("íœŠm”F"));
	}
	if (ImGui::BeginPopupModal(IMGUI_JP("íœŠm”F"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n%s", sceneToDelete.c_str(), IMGUI_JP("‚ğíœ‚µ‚Ü‚·‚©H"));
		ImGui::Separator();

		if (ImGui::Button(IMGUI_JP("‚Í‚¢"), ImVec2(120, 0))) {
>>>>>>> main
			std::string deletePath = "Data\\Image\\Sprite\\UIData\\" + sceneToDelete + ".json";
			if (std::filesystem::exists(deletePath)) {
				try {
					std::filesystem::remove(deletePath);
				}
				catch (...) {}
			}

			if (m_CurrentSceneName == sceneToDelete) {
<<<<<<< HEAD
				// å‰Šé™¤å¯¾è±¡ã®ã‚·ãƒ¼ãƒ³ã‚’ç¾åœ¨ã®ã‚·ãƒ¼ãƒ³ã‹ã‚‰å¤–ã™
				m_CurrentSceneName.clear();

				// UIãªã©ã®ãƒEEã‚¿ã‚’ã‚¯ãƒªã‚¢
=======
				// íœ‘ÎÛ‚ÌƒV[ƒ“‚ğŒ»İ‚ÌƒV[ƒ“‚©‚çŠO‚·
				m_CurrentSceneName.clear();

				// UI‚È‚Ç‚Ìƒf[ƒ^‚ğƒNƒŠƒA
>>>>>>> main
				m_pUIs.clear();
				m_pSprite2Ds.clear();
				m_SpritePosList.clear();
				m_MoveAny = false;
			}

			sceneToDelete.clear();
			showDeleteConfirm = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
<<<<<<< HEAD
		if (ImGui::Button(IMGUI_JP("ãE„ãE), ImVec2(120, 0))) {
=======
		if (ImGui::Button(IMGUI_JP("‚¢‚¢‚¦"), ImVec2(120, 0))) {
>>>>>>> main
			sceneToDelete.clear();
			showDeleteConfirm = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::ImGuiSearchUI()
{
	if (ImGui::TreeNodeEx(IMGUI_JP("UIãƒªã‚¹ãƒE), ImGuiTreeNodeFlags_DefaultOpen)) {
		// æ¤œç´¢ãƒãE
		ImGui::InputText(IMGUI_JP("æ¤œç´¢"), m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
		// ã‚¹ã‚¯ãƒ­ãƒ¼ãƒ«å¯èƒ½ãªãƒªã‚¹ãƒE
		ImGui::BeginChild(IMGUI_JP("ãƒªã‚¹ãƒE), ImVec2(315, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < m_pUIs.size(); ++i) {
			// æ¤œç´¢ãƒ•ã‚£ãƒ«ã‚¿ãƒªãƒ³ã‚°
			if (strlen(m_SearchBuffer) > 0
				&& m_pUIs[i]->GetUIName().find(m_SearchBuffer) == std::string::npos) {
=======

//-----------------------------------------------------------------------------
//		UIƒŠƒXƒgŒŸõŠÖ”
//-----------------------------------------------------------------------------
void UIEditor::ImGuiSearchUI()
{
	if (ImGui::TreeNodeEx(IMGUI_JP("UIƒŠƒXƒg"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// ŒŸõƒo[
		ImGui::InputText(IMGUI_JP("ŒŸõ"), m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
		// ƒXƒNƒ[ƒ‹‰Â”\‚ÈƒŠƒXƒg
		ImGui::BeginChild(IMGUI_JP("ƒŠƒXƒg"), ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < m_pUIs.size(); ++i) {
			// ŒŸõƒtƒBƒ‹ƒ^ƒŠƒ“ƒO
			if (strlen(m_SearchBuffer) > 0
				&& m_pUIs[i]->GetResourceName().find(m_SearchBuffer) == std::string::npos) {
>>>>>>> main
				continue;
			}

			bool isSelected = (m_SelectedUIIndex == i);
<<<<<<< HEAD
			if (ImGui::Selectable(m_pUIs[i]->GetUIName().c_str(), isSelected)) {
				m_SelectedUIIndex = i; // é¸æŠæ›´æ–°
				TriggeHgihLight();
=======
			if (ImGui::Selectable(m_pUIs[i]->GetResourceName().c_str(), isSelected)) {
				m_SelectedUIIndex = i; // ‘I‘ğXV
				SelectInit();
>>>>>>> main
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::AddDeleteSprite()
{
	std::vector<std::string> spriteNames = SpriteManager::GetSprite2D2List();
	if (ImGui::TreeNodeEx(IMGUI_JP("è¿½åŠ å¯èƒ½UIãƒªã‚¹ãƒE), ImGuiTreeNodeFlags_DefaultOpen)) {
		// æ¤œç´¢ãƒãE
		ImGui::InputText(IMGUI_JP("æ¤œç´¢"), m_SpriteSearchBuffer,
			IM_ARRAYSIZE(m_SpriteSearchBuffer));
		// ã‚¹ã‚¯ãƒ­ãƒ¼ãƒ«å¯èƒ½ãªãƒªã‚¹ãƒE
		ImGui::BeginChild(IMGUI_JP("ãƒªã‚¹ãƒE),
			ImVec2(315, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
=======

//-----------------------------------------------------------------------------
//		SpriteManager‚©‚çUI‚ğ’Ç‰Á
//-----------------------------------------------------------------------------
void UIEditor::AddDeleteSprite()
{
	std::vector<std::string> spriteNames = SpriteManager::GetSprite2D2List();
	if (ImGui::TreeNodeEx(IMGUI_JP("’Ç‰Á‰Â”\UIƒŠƒXƒg"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// ŒŸõƒo[
		ImGui::InputText(IMGUI_JP("ŒŸõ"), m_SpriteSearchBuffer,
			IM_ARRAYSIZE(m_SpriteSearchBuffer));
		// ƒXƒNƒ[ƒ‹‰Â”\‚ÈƒŠƒXƒg
		ImGui::BeginChild(IMGUI_JP("ƒŠƒXƒg"),
			ImVec2(315, 100), true, ImGuiWindowFlags_HorizontalScrollbar);
>>>>>>> main

		for (int i = 0; i < spriteNames.size(); ++i) {
			const std::string& name = spriteNames[i];

<<<<<<< HEAD
			// æ¤œç´¢ãƒ•ã‚£ãƒ«ã‚¿
=======
			// ŒŸõƒtƒBƒ‹ƒ^
>>>>>>> main
			if (strlen(m_SpriteSearchBuffer) > 0 &&
				name.find(m_SpriteSearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedSpriteName == name);
			if (ImGui::Selectable(name.c_str(), isSelected)) {
<<<<<<< HEAD
				m_SelectedSpriteName = name; // é¸æŠæ›´æ–°
=======
				m_SelectedSpriteName = name; // ‘I‘ğXV
>>>>>>> main
			}
		}
		ImGui::EndChild();

<<<<<<< HEAD
		// é¸æŠã•ã‚ŒãŸã‚¹ãƒ—ãƒ©ã‚¤ãƒˆã‚’UIã¨ã—ã¦è¿½åŠ 
		if (ImGui::Button(IMGUI_JP("UIè¿½åŠ "))) {
=======
		// ‘I‘ğ‚³‚ê‚½ƒXƒvƒ‰ƒCƒg‚ğUI‚Æ‚µ‚Ä’Ç‰Á
		if (ImGui::Button(IMGUI_JP("UI’Ç‰Á"))) {
>>>>>>> main
			if (!m_SelectedSpriteName.empty()) {
				std::shared_ptr<Sprite2D> pSprite = SpriteManager::GetSprite2D(m_SelectedSpriteName);
				if (!pSprite) return;

				std::shared_ptr<UIObject> ui = std::make_shared<UIObject>();
				ui->AttachSprite(pSprite);

				m_pSprite2Ds.push_back(pSprite);
				m_pUIs.push_back(ui);
				m_SpritePosList.push_back(ui->GetPosition());
<<<<<<< HEAD

				TriggeHgihLight();
			}
		}

		// é¸æŠã•ã‚ŒãŸUIã‚’å‰Šé™¤
		if (ImGui::Button(IMGUI_JP("UIå‰Šé™¤")) && !m_pUIs.empty()) {
=======
			}
		}

		// ‘I‘ğ‚³‚ê‚½UI‚ğíœ
		if (ImGui::Button(IMGUI_JP("UIíœ")) && !m_pUIs.empty()) {
>>>>>>> main
			m_pUIs[m_SelectedUIIndex].reset();
			
			m_pUIs.erase(m_pUIs.begin() + m_SelectedUIIndex);
			m_pSprite2Ds.erase(m_pSprite2Ds.begin() + m_SelectedUIIndex);
			m_SpritePosList.erase(m_SpritePosList.begin() + m_SelectedUIIndex);

<<<<<<< HEAD
			// ã‚¤ãƒ³ãƒEƒƒã‚¯ã‚¹ã‚’ãƒªã‚»ãƒEƒˆ
=======
			// ƒCƒ“ƒfƒbƒNƒX‚ğƒŠƒZƒbƒg
>>>>>>> main
			m_SelectedUIIndex = 0;
		}

		ImGui::TreePop();
	}
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

=======

//-----------------------------------------------------------------------------
//		ƒ\[ƒg‚Æ–¼‘O•ÏX‚ğ“¯‚És‚¤
//-----------------------------------------------------------------------------
>>>>>>> main
void UIEditor::RenameUIObjects()
{
	if (m_pUIs.empty()) { return; }
	std::vector<std::pair<std::string, std::shared_ptr<UIObject>>> nameUIList;

<<<<<<< HEAD
	// UIåã¨UIã‚ªãƒ–ã‚¸ã‚§ã‚¯ãƒˆãEãƒšã‚¢ã‚’åé›E
	for (std::shared_ptr<UIObject> ui : m_pUIs) {
		std::string baseName = GetBaseName(ui->GetUIName());
		nameUIList.emplace_back(baseName, ui);
	}

	// ãƒŠãƒ³ãƒãƒªãƒ³ã‚°ã—ã¦åå‰ã‚’åEè¨­å®E
=======
	// UI–¼‚ÆUIƒIƒuƒWƒFƒNƒg‚ÌƒyƒA‚ğûW
	for (std::shared_ptr<UIObject> ui : m_pUIs) {
		std::string baseName = GetBaseName(ui->GetResourceName());
		nameUIList.emplace_back(baseName, ui);
	}

	// ƒiƒ“ƒoƒŠƒ“ƒO‚µ‚Ä–¼‘O‚ğÄİ’è
>>>>>>> main
	std::unordered_map<std::string, int> nameCount;

	for (auto& [baseName, ui] : nameUIList) {
		int index = nameCount[baseName]++;
		std::string newName = baseName + "_" + std::to_string(index);
<<<<<<< HEAD
		ui->SetUIName(newName);
	}
}

//-----------------------------------------------------------------------.

void UIEditor::ImGuiPosEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("åº§æ¨E)))
	{
		// ãƒ‰ãƒ©ãƒE‚°&ãƒ‰ãƒ­ãƒEEç”¨ã«ãƒã‚¦ã‚¹æ“ä½œãEDirectInputã‚’ç”¨æ„E
=======
		ui->SetResourceName(newName);
	}
}


//-----------------------------------------------------------------------------
//		À•W’²®ŠÖ”(‘I‘ğ‚³‚ê‚½UIObect)
//-----------------------------------------------------------------------------
void UIEditor::ImGuiPosEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("À•W")))
	{
		// ƒhƒ‰ƒbƒO&ƒhƒƒbƒv—p‚Éƒ}ƒEƒX‘€ì‚ÌDirectInput‚ğ—pˆÓ
>>>>>>> main
		DirectX::XMFLOAT3 pos = object->GetPosition();
		bool posdrag = ImGui::DragFloat3("##Position", &pos.x, m_DragValue);
	
		POINT MousePos;
		GetCursorPos(&MousePos);
		RECT rect;
		GetWindowRect(GetForegroundWindow(), &rect);
		DirectX::XMFLOAT2 objectpos = DirectX::XMFLOAT2(object->GetPosition().x + rect.left, object->GetPosition().y + rect.top);

<<<<<<< HEAD
		// ç”»åƒç¯E›²å†E§å·¦ã‚¯ãƒªãƒE‚¯å…¥åŠ›ä¸­ã®å ´åˆã€ãƒ‰ãƒ©ãƒE‚°æ“ä½œã‚’é–‹å§E
		if (SpriteCollider::PointInSquare(MousePos, objectpos,object->GetDrawSize()) && !m_DoDrag) {
=======
		// ‰æ‘œ”ÍˆÍ“à‚Å¶ƒNƒŠƒbƒN“ü—Í’†‚Ìê‡Aƒhƒ‰ƒbƒO‘€ì‚ğŠJn
		if (SpriteCollider::PointInSquare(MousePos, objectpos,object->GetDrawSize()) && !m_DoDrag) {
			// ‰ö‚µ‚¢ƒ|ƒCƒ“ƒg(‚Ù‚ñ‚Ü‚É”»’è‚Æ‚ê‚é‚Ì‚±‚êHH)
>>>>>>> main
			if (Input::IsMouseGrab()) {
				m_DoDrag = true;
				m_OffsetPos = DirectX::XMFLOAT2(pos.x - MousePos.x, pos.y - MousePos.y);
			}
		}
		if (m_DoDrag) {
			posdrag = true;
<<<<<<< HEAD
			// è£œæ­£å€¤+ãƒã‚¦ã‚¹åº§æ¨™ã—ãŸåº§æ¨™ã‚’å…¥ã‚Œã‚‹
			pos = DirectX::XMFLOAT3(MousePos.x + m_OffsetPos.x, MousePos.y + m_OffsetPos.y, pos.z);
			// ãƒã‚¦ã‚¹ã®å·¦ã‚¯ãƒªãƒE‚¯ã‚’è©±ã—ãŸå ´åˆã€ãƒ‰ãƒ©ãƒE‚°æ“ä½œã‚’åœæ­¢
			if (!Input::IsMouseGrab()) { m_DoDrag = false; }
		}

		// å¤‰æ›´ãŒã‚ã£ãŸå ´åˆä¿å­˜ã™ã‚E
=======
			// •â³’l+ƒ}ƒEƒXÀ•W‚µ‚½À•W‚ğ“ü‚ê‚é
			pos = DirectX::XMFLOAT3(MousePos.x + m_OffsetPos.x, MousePos.y + m_OffsetPos.y, pos.z);
			// ƒ}ƒEƒX‚Ì¶ƒNƒŠƒbƒN‚ğ˜b‚µ‚½ê‡Aƒhƒ‰ƒbƒO‘€ì‚ğ’â~
			if (!Input::IsMouseGrab()) { m_DoDrag = false; }
		}

		// •ÏX‚ª‚ ‚Á‚½ê‡•Û‘¶‚·‚é
>>>>>>> main
		if (posdrag) {
			object->SetPosition(pos);
			m_MoveAny = true;
			m_MovedSpritePos = true;
		}
		ImGui::TreePop();
	}
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::ImGuiInfoEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("ç”»åƒæƒ…å ±")))
	{
		// è¡¨ç¤ºã‚µã‚¤ã‚ºã‚’ä»£å…¥
=======

//-----------------------------------------------------------------------------
//		î•ñ’²®ŠÖ”(‘I‘ğ‚³‚ê‚½UIObect)
//-----------------------------------------------------------------------------
void UIEditor::ImGuiInfoEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("‰æ‘œî•ñ")))
	{
		// Œ³A•\¦A•ªŠ„‚»‚ê‚¼‚ê‚ÌƒTƒCƒY‚ğ‘ã“ü
		//DirectX::XMFLOAT2 base = DirectX::XMFLOAT2(
		//	object->GetSpriteData().x,
		//	object->GetSpriteData().y);
		//DirectX::XMFLOAT2 stride = DirectX::XMFLOAT2(
		//	object->GetSpriteData().Stride.w,
		//	object->GetSpriteData().Stride.h);
>>>>>>> main
		DirectX::XMFLOAT2 disp = DirectX::XMFLOAT2(
			object->GetDrawSize().x,
			object->GetDrawSize().y);

<<<<<<< HEAD
		ImGui::Text(IMGUI_JP("è¡¨ç¤ºã‚µã‚¤ã‚º(x,y)"));
		bool dispdrag = ImGui::DragFloat2("##DispDrag", &disp.x, m_DragValue);

		// å¤‰æ›´ãŒã‚ã£ãŸå ´åˆä¿å­˜ã™ã‚E
		if (dispdrag)
		{
			object->SetDrawSize(disp);
=======
		//ImGui::Text(IMGUI_JP("Œ³‚ÌƒTƒCƒY(x,y)"));
		//bool basedrag = ImGui::DragFloat2("##BaseDrag", base, m_DragValue);

		ImGui::Text(IMGUI_JP("•\¦ƒTƒCƒY(x,y)"));
		bool dispdrag = ImGui::DragFloat2("##DispDrag", &disp.x, m_DragValue);

		//ImGui::Text(IMGUI_JP("•ªŠ„ƒTƒCƒY(x,y)"));
		//bool stridedrag = ImGui::DragFloat2("##StrideDrag", stride, m_DragValue);

		// •ÏX‚ª‚ ‚Á‚½ê‡•Û‘¶‚·‚é
		if (dispdrag
			//|| basedrag
			//|| stridedrag
			)
		{
			object->SetDrawSize(disp);
		//	object->SetBase(base);
		//	object->SetStride(stride);
>>>>>>> main
			m_MoveAny = true;
		}
		ImGui::TreePop();
	}
}

<<<<<<< HEAD
//-----------------------------------------------------------------------.

void UIEditor::ImGuiEtcInfoEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("ããEä»E)))
=======

//-----------------------------------------------------------------------------
//		‚»‚Ì‘¼‚Ìî•ñ’²®ŠÖ”(‘I‘ğ‚³‚ê‚½UIObect)
//-----------------------------------------------------------------------------
void UIEditor::ImGuiEtcInfoEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("‚»‚Ì‘¼")))
>>>>>>> main
	{
		DirectX::XMFLOAT4 color = object->GetColor();
		DirectX::XMFLOAT3 scale = object->GetScale();
		DirectX::XMFLOAT3 rot = object->GetRotation();
		DirectX::XMFLOAT2 pivot = object->GetPivot();

<<<<<<< HEAD
		ImGui::Text(IMGUI_JP("ã‚«ãƒ©ãƒ¼"));
		bool colorslider = ImGui::ColorEdit4("##Color", &color.x);
		ImGui::Text(IMGUI_JP("ã‚¹ã‚±ãƒ¼ãƒ«"));
		bool scaledrag = ImGui::DragFloat3("##ScaleDrag", &scale.x, m_DragValue);
		ImGui::Text(IMGUI_JP("å›è»¢è»¸"));
		bool Pivotdrag = ImGui::DragFloat2("##PivotDrag", &pivot.x, m_DragValue);
		ImGui::Text(IMGUI_JP("å›è»¢"));
		bool rotdrag = ImGui::DragFloat3("##RotDrag", &rot.x, m_DragValue);

		// å¤‰æ›´ãŒã‚ã£ãŸå ´åˆä¿å­˜ã™ã‚E
=======
		ImGui::Text(IMGUI_JP("ƒJƒ‰["));
		bool colorslider = ImGui::ColorEdit4("##Color", &color.x);
		ImGui::Text(IMGUI_JP("ƒXƒP[ƒ‹"));
		bool scaledrag = ImGui::DragFloat3("##ScaleDrag", &scale.x, m_DragValue);
		ImGui::Text(IMGUI_JP("‰ñ“]²"));
		bool Pivotdrag = ImGui::DragFloat3("##PivotDrag", &pivot.x, m_DragValue);
		ImGui::Text(IMGUI_JP("‰ñ“]"));
		bool rotdrag = ImGui::DragFloat3("##RotDrag", &rot.x, m_DragValue);

		// •ÏX‚ª‚ ‚Á‚½ê‡•Û‘¶‚·‚é
>>>>>>> main
		if (scaledrag
			|| Pivotdrag
			|| rotdrag
			|| colorslider)
		{
			object->SetColor(DirectX::XMFLOAT4(color.x, color.y, color.z,color.w));
			object->SetAlpha(color.w);
			object->SetScale(scale);
			object->SetPivot(pivot);
			object->SetRotation(rot);
			m_MoveAny = true;
		}
		ImGui::TreePop();
	}
<<<<<<< HEAD
}

//-----------------------------------------------------------------------.

void UIEditor::TriggeHgihLight()
{
	m_HighlightTime = 30.f * Time::GetDeltaTime();
}

//-----------------------------------------------------------------------.

void UIEditor::HighLightUI(std::shared_ptr<UIObject> object)
{
	if (m_HighlightTime <= 0.0f) return;

	DirectX::XMFLOAT3 pos = object->GetPosition();
	DirectX::XMFLOAT2 size = object->GetDrawSize();
	DirectX::XMFLOAT3 scale = object->GetScale();

	POINT clientLT = { 0, 0 };
	ClientToScreen(ResourceManager::GethWnd(), &clientLT);

	ImVec2 iwp = ImVec2(
		clientLT.x + pos.x + WND_W / 2 - size.x / 2,
		clientLT.y + pos.y + WND_H / 2 - size.y / 2);
	ImVec2 iws = ImVec2(size.x, size.y);

	ImGui::SetNextWindowPos(iwp);
	ImGui::SetNextWindowSize(iws);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground;

	ImGui::Begin("##UIHighlightOverlay", nullptr, flags);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImVec2 fillsize = ImVec2(iwp.x + size.x, iwp.y + size.y);

	draw->AddRectFilled(
		iwp,
		fillsize,
		IM_COL32(255, 0, 0, 100)
	);

	ImGui::End();

	m_HighlightTime = m_HighlightTime - Time::GetDeltaTime();
=======
>>>>>>> main
}