#include "UIEditor.h"
#include "FileManager/FileManager.h"
#include "ResourceManager/SpriteManager/SpriteManager.h"
#include "Game/05_InputDevice/Input.h"
#include "Game/03_Collision/Sprite/SpriteCollider.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Game//04_Time//Time.h"
#include "Singleton/SceneManager/SceneManager.h"
#include "ResourceManager/ResourceManager.h"

#if _DEBUG
#include "ImGui/CImGuiManager.h"
#endif // _DEBUG


// json�^��쐬
using Json = nlohmann::json;


namespace {
	char m_NewSceneName[64] = ""; // �V�K�쐬�p�o�b�t�@������o�ϐ��ɒǉ�
}

//-----------------------------------------------------------------------.

UIEditor::UIEditor()
{
}
UIEditor::~UIEditor()
{
}

//-----------------------------------------------------------------------.

//=============================================================================
//		�쐬����
//=============================================================================
void UIEditor::Create()
{
	SelectSceneLoad("Title");
	SelectInit();
}

//-----------------------------------------------------------------------.

//=============================================================================
//		�f�[�^�Ǎ�
//=============================================================================
HRESULT UIEditor::LoadData()
{
	return E_NOTIMPL;
}

//-----------------------------------------------------------------------.

//=============================================================================
//		����������
//=============================================================================
void UIEditor::Initialize()
{
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		UI�I����ɉ��ϐ��������������
//-----------------------------------------------------------------------------
void UIEditor::SelectInit()
{
	m_PatternNo = POINTS(0, 0);
	m_PatternMax = POINTS(1, 1);
	m_PatternAuto = false;
}


//=============================================================================
//		�X�V����
//=============================================================================
void UIEditor::Update()
{
	// �L�[����
	KeyInput();

	// �V�[����I�����
	ImGuiSelectScene();

	//--------------------------------------------------------------
	//		UI�̒ǉ��ƍ폜
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�̒ǉ��E�폜"));
	// �V�[����UI��ǉ�
	AddDeleteSprite();
	// �摜����ύX�����O���ɑΏ�
	RenameUIObjects();
	ImGui::End();

	//--------------------------------------------------------------
	//		UI����
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�G�f�B�^�[�E�B���h�E"));
	// UI���X�g�̌����֐�
	ImGuiSearchUI();
	
	//-----------------------------------------------------------
	//		�I�𒆂̃I�u�W�F�N�g�̕ҏW
	//-----------------------------------------------------------
	if (m_SelectedUIIndex >= 0 && m_SelectedUIIndex < m_pUIs.size()) {
		// �I�����Ă���UI��\��
		ImGui::Text(IMGUI_JP("選択されているUI: %s"), m_pUIs[m_SelectedUIIndex]->GetUIName().c_str());


		// ���W�̒���
		ImGuiPosEdit(m_pUIs[m_SelectedUIIndex]);
		// Z���W���Ƀ\�[�g
		SortBySpritePosZ(m_pUIs[m_SelectedUIIndex]);

		// �摜���̒���
		ImGuiInfoEdit(m_pUIs[m_SelectedUIIndex]);
		// ���̑��̏��̒���
		ImGuiEtcInfoEdit(m_pUIs[m_SelectedUIIndex]);

		HighLightUI(m_pUIs[m_SelectedUIIndex]);
	}

	ImGui::End();

	//--------------------------------------------------------------
	//		�ۑ�����
	//--------------------------------------------------------------
	ImGui::Begin(IMGUI_JP("UI�ۑ��E�B���h�E"));
	if (ImGui::Button(IMGUI_JP("UI��ۑ�"))) {
		SaveScene();
		m_MoveAny = false;
	}
	ImGui::End();
}

//-----------------------------------------------------------------------.

void UIEditor::LateUpdate() 
{
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		�L�[���͏���
//-----------------------------------------------------------------------------
void UIEditor::KeyInput()
{
	m_DragValue = 1.f;
	if (GetAsyncKeyState(VK_SHIFT) && 0x8000) { m_DragValue *= 0.01f; }
	if (GetAsyncKeyState(VK_SPACE) && 0x8000) { m_DragValue *= 0.001f; }
}

//-----------------------------------------------------------------------.

//=============================================================================
//		�`�揈��
//=============================================================================
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

//-----------------------------------------------------------------------------
//		�I������V�[����UI��ǂݍ���
//-----------------------------------------------------------------------------
void UIEditor::SelectSceneLoad(const std::string& sceneName)
{
	m_pSprite2Ds.clear();
	for (auto sprite : m_pSprite2Ds) sprite.reset();
	for (auto ui : m_pUIs) ui.reset();
	m_pUIs.clear();
	m_SpritePosList.clear();

	m_CurrentSceneName = sceneName;
	m_ScenePath = "Data\\Image\\Sprite\\UIData\\" + sceneName + ".json";

	// JSON�ǂݍ���
	Json jsonData = FileManager::JsonLoad(m_ScenePath);

	// ��Ȃ珉��UI��1�ǉ�
	if (jsonData.is_null() || jsonData.empty()) {
		std::shared_ptr<Sprite2D> sprite = std::make_shared<Sprite2D>();
		std::shared_ptr<UIObject> ui = std::make_shared<UIObject>();

		sprite->Initialize("Data\\Image\\Sprite\\Other\\White.png"); // ���摜������ŏo��
		ui->AttachSprite(sprite);
		ui->SetPosition(0.f, 0.f, 0.f);

		m_pSprite2Ds.push_back(sprite);
		m_pUIs.push_back(ui);
		m_SpritePosList.push_back(ui->GetPosition());

		// ���ł�SaveScene()�ŏ�������ł���
		SaveScene();
	}

	// �ۑ����ꂽUI�f�[�^��ǂݍ��݁A�W�J
	for (auto& [imageName, spriteArray] : jsonData.items()) {
		// �g���q�� .json �Ȃ�X�L�b�v
		std::string::size_type dotPos = imageName.find_last_of('.');
		if (dotPos != std::string::npos) {
			std::string ext = imageName.substr(dotPos);
			if (ext == ".json" || ext == ".JSON") continue;
		}
		
		// �X�v���C�g�擾
		std::shared_ptr<Sprite2D> Sprite = SpriteManager::GetSprite2D(GetBaseName(imageName));
		if (!Sprite) {
			MessageBoxA(NULL, ("�X�v���C�g��������܂���: " + imageName).c_str(), "Error", MB_OK);
			continue;
		}

		// �eUI�C���X�^���X��W�J
		for (auto& value : spriteArray) {
			std::shared_ptr<UIObject> ui = std::make_shared<UIObject>();

			ui->SetUIName(imageName);
			ui->SetPosition(DirectX::XMFLOAT3(value["Pos"]["x"], value["Pos"]["y"], value["Pos"]["z"]));
			ui->SetColor(DirectX::XMFLOAT4(value["Color"]["x"], value["Color"]["y"], value["Color"]["z"], value["Color"]["a"]));
			ui->SetAlpha(value["Alpha"]);
			ui->SetScale(DirectX::XMFLOAT3(value["Scale"]["x"], value["Scale"]["y"], value["Scale"]["z"]));
			ui->SetPivot(DirectX::XMFLOAT2(value["Pivot"]["x"], value["Pivot"]["y"]));
			ui->SetRotation(DirectX::XMFLOAT3(value["Rotate"]["x"], value["Rotate"]["y"], value["Rotate"]["z"]));
			ui->SetDrawSize(DirectX::XMFLOAT2(value["Disp"]["w"], value["Disp"]["h"]));
			
			// �Ȃ��̂��H�H
			//ui->SetBase(DirectX::XMFLOAT2(value["Base"]["w"], value["Base"]["h"]));
			//ui->SetStride(DirectX::XMFLOAT2(value["Stride"]["w"], value["Stride"]["h"]));

			// ���X�g�ɒǉ�
			m_pSprite2Ds.push_back(Sprite);
			ui->AttachSprite(Sprite);
			m_pUIs.push_back(ui);
			m_SpritePosList.push_back(ui->GetPosition());
		}
	}
	// Z���W���Ƀ\�[�g����
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](std::shared_ptr<UIObject> a, std::shared_ptr<UIObject> b) {
		return a->GetPosition().z < b->GetPosition().z;
		});
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		���݃V�[����UI����ۑ�
//-----------------------------------------------------------------------------
HRESULT UIEditor::SaveScene()
{
	Json jsonData;
	for (size_t i = 0; i < m_pUIs.size(); ++i)
	{
		std::string imageName = m_pUIs[i]->GetUIName();

		// �摜�����Ƃ̃��X�g��UI����ǉ�
		Json SpriteState;
		SpriteState["Pos"]["x"] = m_pUIs[i]->GetPosition().x;
		SpriteState["Pos"]["y"] = m_pUIs[i]->GetPosition().y;
		SpriteState["Pos"]["z"] = m_pUIs[i]->GetPosition().z;
		SpriteState["Disp"]["w"] = m_pUIs[i]->GetDrawSize().x;
		SpriteState["Disp"]["h"] = m_pUIs[i]->GetDrawSize().y;
		//SpriteState["Base"]["w"] = m_pUIs[i]->GetSpriteData().Base.w;
		//SpriteState["Base"]["h"] = m_pUIs[i]->GetSpriteData().Base.h;
		//SpriteState["Stride"]["w"] = m_pUIs[i]->GetSpriteData().Stride.w;
		//SpriteState["Stride"]["h"] = m_pUIs[i]->GetSpriteData().Stride.h;

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

		// jsonData[�摜��] �ɔz��Ƃ��Ēǉ�
		jsonData[imageName].push_back(SpriteState);
	}
	std::string outPath = "Data\\Image\\Sprite\\UIData\\" + m_CurrentSceneName + ".json";
	if (!SUCCEEDED(FileManager::JsonSave(outPath, jsonData))) return E_FAIL;

	return S_OK;
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		Z���W����Ƀ\�[�g����֐�
//-----------------------------------------------------------------------------
void UIEditor::SortBySpritePosZ(std::shared_ptr<UIObject> object)
{
	// ������W�Ɋւ��ĕύX���Ȃ��ꍇ�������^�[��
	if (!m_MovedSpritePos) { return; }

	// �\�[�g�O�̑I�����Ă��� UI �̃|�C���^��ۑ�
	std::shared_ptr<UIObject> pPreviousSelectedUI = object;

	// Z���W���Ƀ\�[�g����
	std::sort(m_pUIs.begin(), m_pUIs.end(),
		[](const std::shared_ptr<UIObject>& a, const std::shared_ptr<UIObject>& b) {
			if (!a || !b) return false; // UIObject��nullptr�Ȃ����

			const auto& posA = a->GetPosition();
			const auto& posB = b->GetPosition();

			return posA.z < posB.z;
		});

	// �\�[�g��ɁA�ȑO�I�����Ă��� UI ���܂����X�g�ɑ��݂��邩�m�F���A�đI��
	if (pPreviousSelectedUI != nullptr)
	{
		auto it = std::find(m_pUIs.begin(), m_pUIs.end(), pPreviousSelectedUI);
		if (it != m_pUIs.end()) {
			// �đI��
			m_SelectedUIIndex = static_cast<int>(std::distance(m_pUIs.begin(), it));
		}
		else {
			// �ȑO�I�����Ă���UI�����X�g�ɂȂ��ꍇ
			object = nullptr;
			m_SelectedUIIndex = 0;
		}
	}

	m_MovedSpritePos = false;
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		ImGui��p�����V�[���I��֐�
//-----------------------------------------------------------------------------
void UIEditor::ImGuiSelectScene()
{
	ImGui::Begin(IMGUI_JP("�V�[���Ǘ�"));

	// �V�K�V�[���̍쐬
	ImGui::InputText(IMGUI_JP("�V�K�V�[����"), m_NewSceneName, IM_ARRAYSIZE(m_NewSceneName));
	if (ImGui::Button(IMGUI_JP("�V�K�V�[���쐬"))) {
		std::string newPath = "Data\\Image\\Sprite\\UIData\\" + std::string(m_NewSceneName) + ".json";
		if (!std::filesystem::exists(newPath)) {
			std::ofstream ofs(newPath);
			ofs << "{}"; // ���JSON���������
			ofs.close();
		}
		m_CurrentSceneName = m_NewSceneName;
		SelectSceneLoad(m_CurrentSceneName);
		m_MoveAny = false;
	}

	ImGui::Separator();
	ImGui::Text(IMGUI_JP("�����̃V�[��"));

	static std::string sceneToDelete; // �폜���̃V�[����
	static bool showDeleteConfirm = false; // �폜�m�F�_�C�A���O�\���t���O

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

			if (ImGui::Button(IMGUI_JP("�폜"))) {
				sceneToDelete = sceneName;
				showDeleteConfirm = true;
			}

			ImGui::PopID();
		}
	}

	// �폜�m�F���[�_��
	if (showDeleteConfirm) {
		ImGui::OpenPopup(IMGUI_JP("�폜�m�F"));
	}
	if (ImGui::BeginPopupModal(IMGUI_JP("�폜�m�F"), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s\n%s", sceneToDelete.c_str(), IMGUI_JP("��폜���܂����H"));
		ImGui::Separator();

		if (ImGui::Button(IMGUI_JP("�͂�"), ImVec2(120, 0))) {
			std::string deletePath = "Data\\Image\\Sprite\\UIData\\" + sceneToDelete + ".json";
			if (std::filesystem::exists(deletePath)) {
				try {
					std::filesystem::remove(deletePath);
				}
				catch (...) {}
			}

			if (m_CurrentSceneName == sceneToDelete) {
				// �폜�Ώۂ̃V�[������݂̃V�[������O��
				m_CurrentSceneName.clear();

				// UI�Ȃǂ̃f�[�^��N���A
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
		if (ImGui::Button(IMGUI_JP("������"), ImVec2(120, 0))) {
			sceneToDelete.clear();
			showDeleteConfirm = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		UI���X�g�����֐�
//-----------------------------------------------------------------------------
void UIEditor::ImGuiSearchUI()
{
	if (ImGui::TreeNodeEx(IMGUI_JP("UI���X�g"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// �����o�[
		ImGui::InputText(IMGUI_JP("����"), m_SearchBuffer, IM_ARRAYSIZE(m_SearchBuffer));
		// �X�N���[���\�ȃ��X�g
		ImGui::BeginChild(IMGUI_JP("リスト"), ImVec2(315, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < m_pUIs.size(); ++i) {
			// �����t�B���^�����O
			if (strlen(m_SearchBuffer) > 0
				&& m_pUIs[i]->GetUIName().find(m_SearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedUIIndex == i);
			if (ImGui::Selectable(m_pUIs[i]->GetUIName().c_str(), isSelected)) {
				m_SelectedUIIndex = i; // �I��X�V
				TriggeHgihLight();
			}
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		SpriteManager����UI��ǉ�
//-----------------------------------------------------------------------------
void UIEditor::AddDeleteSprite()
{
	std::vector<std::string> spriteNames = SpriteManager::GetSprite2D2List();
	if (ImGui::TreeNodeEx(IMGUI_JP("�ǉ��\UI���X�g"), ImGuiTreeNodeFlags_DefaultOpen)) {
		// �����o�[
		ImGui::InputText(IMGUI_JP("����"), m_SpriteSearchBuffer,
			IM_ARRAYSIZE(m_SpriteSearchBuffer));
		// �X�N���[���\�ȃ��X�g
		ImGui::BeginChild(IMGUI_JP("���X�g"),
			ImVec2(315, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < spriteNames.size(); ++i) {
			const std::string& name = spriteNames[i];

			// �����t�B���^
			if (strlen(m_SpriteSearchBuffer) > 0 &&
				name.find(m_SpriteSearchBuffer) == std::string::npos) {
				continue;
			}

			bool isSelected = (m_SelectedSpriteName == name);
			if (ImGui::Selectable(name.c_str(), isSelected)) {
				m_SelectedSpriteName = name; // �I��X�V
			}
		}
		ImGui::EndChild();

		// �I����ꂽ�X�v���C�g��UI�Ƃ��Ēǉ�
		if (ImGui::Button(IMGUI_JP("UI�ǉ�"))) {
			if (!m_SelectedSpriteName.empty()) {
				std::shared_ptr<Sprite2D> pSprite = SpriteManager::GetSprite2D(m_SelectedSpriteName);
				if (!pSprite) return;

				std::shared_ptr<UIObject> ui = std::make_shared<UIObject>();
				ui->AttachSprite(pSprite);

				m_pSprite2Ds.push_back(pSprite);
				m_pUIs.push_back(ui);
				m_SpritePosList.push_back(ui->GetPosition());

				TriggeHgihLight();
			}
		}

		// �I����ꂽUI��폜
		if (ImGui::Button(IMGUI_JP("UI�폜")) && !m_pUIs.empty()) {
			m_pUIs[m_SelectedUIIndex].reset();
			
			m_pUIs.erase(m_pUIs.begin() + m_SelectedUIIndex);
			m_pSprite2Ds.erase(m_pSprite2Ds.begin() + m_SelectedUIIndex);
			m_SpritePosList.erase(m_SpritePosList.begin() + m_SelectedUIIndex);

			// �C���f�b�N�X����Z�b�g
			m_SelectedUIIndex = 0;
		}

		ImGui::TreePop();
	}
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		�\�[�g�Ɩ��O�ύX�𓯎��ɍs��
//-----------------------------------------------------------------------------
void UIEditor::RenameUIObjects()
{
	if (m_pUIs.empty()) { return; }
	std::vector<std::pair<std::string, std::shared_ptr<UIObject>>> nameUIList;

	// UI����UI�I�u�W�F�N�g�̃y�A����W
	for (std::shared_ptr<UIObject> ui : m_pUIs) {
		std::string baseName = GetBaseName(ui->GetUIName());
		nameUIList.emplace_back(baseName, ui);
	}

	// �i���o�����O���Ė��O��Đݒ�
	std::unordered_map<std::string, int> nameCount;

	for (auto& [baseName, ui] : nameUIList) {
		int index = nameCount[baseName]++;
		std::string newName = baseName + "_" + std::to_string(index);
		ui->SetUIName(newName);
	}
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		���W�����֐�(�I����ꂽUIObect)
//-----------------------------------------------------------------------------
void UIEditor::ImGuiPosEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("���W")))
	{
		// �h���b�O&�h���b�v�p�Ƀ}�E�X�����DirectInput��p��
		DirectX::XMFLOAT3 pos = object->GetPosition();
		bool posdrag = ImGui::DragFloat3("##Position", &pos.x, m_DragValue);
	
		POINT MousePos;
		GetCursorPos(&MousePos);
		RECT rect;
		GetWindowRect(GetForegroundWindow(), &rect);
		DirectX::XMFLOAT2 objectpos = DirectX::XMFLOAT2(object->GetPosition().x + rect.left, object->GetPosition().y + rect.top);

		// �摜�͈͓�ō��N���b�N���͒��̏ꍇ�A�h���b�O�����J�n
		if (SpriteCollider::PointInSquare(MousePos, objectpos,object->GetDrawSize()) && !m_DoDrag) {
			// �������|�C���g(�ق�܂ɔ���Ƃ��̂���H�H)
			if (Input::IsMouseGrab()) {
				m_DoDrag = true;
				m_OffsetPos = DirectX::XMFLOAT2(pos.x - MousePos.x, pos.y - MousePos.y);
			}
		}
		if (m_DoDrag) {
			posdrag = true;
			// �␳�l+�}�E�X���W�������W������
			pos = DirectX::XMFLOAT3(MousePos.x + m_OffsetPos.x, MousePos.y + m_OffsetPos.y, pos.z);
			// �}�E�X�̍��N���b�N��b�����ꍇ�A�h���b�O������~
			if (!Input::IsMouseGrab()) { m_DoDrag = false; }
		}

		// �ύX���������ꍇ�ۑ�����
		if (posdrag) {
			object->SetPosition(pos);
			m_MoveAny = true;
			m_MovedSpritePos = true;
		}
		ImGui::TreePop();
	}
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		��񒲐��֐�(�I����ꂽUIObect)
//-----------------------------------------------------------------------------
void UIEditor::ImGuiInfoEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("�摜���")))
	{
		// 表示サイズを代入
		DirectX::XMFLOAT2 disp = DirectX::XMFLOAT2(
			object->GetDrawSize().x,
			object->GetDrawSize().y);

		//ImGui::Text(IMGUI_JP("���̃T�C�Y(x,y)"));
		//bool basedrag = ImGui::DragFloat2("##BaseDrag", base, m_DragValue);

		ImGui::Text(IMGUI_JP("�\���T�C�Y(x,y)"));
		bool dispdrag = ImGui::DragFloat2("##DispDrag", &disp.x, m_DragValue);

		//ImGui::Text(IMGUI_JP("�����T�C�Y(x,y)"));
		//bool stridedrag = ImGui::DragFloat2("##StrideDrag", stride, m_DragValue);

		// �ύX���������ꍇ�ۑ�����
		if (dispdrag)
		{
			object->SetDrawSize(disp);
		//	object->SetBase(base);
		//	object->SetStride(stride);
			m_MoveAny = true;
		}
		ImGui::TreePop();
	}
}

//-----------------------------------------------------------------------.

//-----------------------------------------------------------------------------
//		���̑��̏�񒲐��֐�(�I����ꂽUIObect)
//-----------------------------------------------------------------------------
void UIEditor::ImGuiEtcInfoEdit(std::shared_ptr<UIObject> object)
{
	if (ImGui::TreeNode(IMGUI_JP("���̑�")))
	{
		DirectX::XMFLOAT4 color = object->GetColor();
		DirectX::XMFLOAT3 scale = object->GetScale();
		DirectX::XMFLOAT3 rot = object->GetRotation();
		DirectX::XMFLOAT2 pivot = object->GetPivot();

		ImGui::Text(IMGUI_JP("�J���["));
		bool colorslider = ImGui::ColorEdit4("##Color", &color.x);
		ImGui::Text(IMGUI_JP("�X�P�[��"));
		bool scaledrag = ImGui::DragFloat3("##ScaleDrag", &scale.x, m_DragValue);
		ImGui::Text(IMGUI_JP("��]��"));
		bool Pivotdrag = ImGui::DragFloat2("##PivotDrag", &pivot.x, m_DragValue);
		ImGui::Text(IMGUI_JP("��]"));
		bool rotdrag = ImGui::DragFloat3("##RotDrag", &rot.x, m_DragValue);

		// �ύX���������ꍇ�ۑ�����
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
}