#include "CImGuiManager.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include <utility>

// font path etc.
constexpr char FONT_FILE_PATH[] = "Data//ImGui//Font//NotoSansJP-SemiBold.ttf";
constexpr float FONT_SIZE = 18.f;
constexpr float SAMALINE_OFFSET = 100.f;

CImGuiManager::CImGuiManager()
{

}

CImGuiManager::~CImGuiManager()
{

}

CImGuiManager* CImGuiManager::GetInstance()
{
    static CImGuiManager Instance;
    return &Instance;
}

HRESULT CImGuiManager::Init(HWND hWnd)
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.DeltaTime = 1.f / 60.f;
    io.DisplaySize.x = WND_W;
    io.DisplaySize.y = WND_H;

    ImFont* font = io.Fonts->AddFontFromFileTTF(
        FONT_FILE_PATH,
        FONT_SIZE,
        NULL,
        io.Fonts->GetGlyphRangesJapanese());

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.f;
    }

    if (ImGui_ImplWin32_Init(hWnd) == false) { return E_FAIL; };
    if (ImGui_ImplDX11_Init(DirectX11::GetInstance().GetDevice(), DirectX11::GetInstance().GetContext()) == false) { return E_FAIL; }

    return S_OK;
}

void CImGuiManager::Relese()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CImGuiManager::NewFrameSetting()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImGuiManager::Render()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

// template Input/Slider/Combo/CheckBox/Graph implementations (unchanged) ...
// For brevity they remain identical to previous implementation in header file's expectations.

template<typename T>
bool CImGuiManager::Input(const char* label,
    T& value,
    bool isLabel,
    float step,
    float stepFast,
    const char* format,
    ImGuiInputTextFlags flags)
{
    const std::string newlabel = "##" + std::string(label);

    if (isLabel == true)
    {
        ImGui::Text(label);
        ImGui::SameLine(SAMALINE_OFFSET);
    }

    if constexpr (std::is_same_v<T, int>)
    {
        return ImGui::InputInt(newlabel.c_str(),
            &value,
            static_cast<int>(step),
            static_cast<int>(stepFast),
            flags);
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return ImGui::InputFloat(newlabel.c_str(),
            &value,
            step,
            stepFast,
            format,
            flags);
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return ImGui::InputDouble(newlabel.c_str(),
            &value,
            static_cast<double>(step),
            static_cast<double>(stepFast),
            format,
            flags);
    }
    else if constexpr (std::is_same_v<T, D3DXVECTOR2>)
    {
        return ImGui::InputFloat2(newlabel.c_str(),
            &value.x,
            format,
            flags);
    }
    else if constexpr (std::is_same_v<T, D3DXVECTOR3>)
    {
        return ImGui::InputFloat3(newlabel.c_str(),
            &value.x,
            format,
            flags);
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        char buffer[256];
        strncpy_s(buffer,
            value.c_str(),
            sizeof(buffer));
        if (ImGui::InputText(newlabel.c_str(),
            buffer,
            sizeof(buffer)))
        {
            value = buffer;
            return true;
        }
    }
    return false;
}

template bool CImGuiManager::Input<int>(const char*,
    int&,
    bool,
    float,
    float,
    const char*,
    ImGuiInputTextFlags);
template bool CImGuiManager::Input<float>(const char*,
    float&,
    bool,
    float,
    float,
    const char*,
    ImGuiInputTextFlags);
template bool CImGuiManager::Input<double>(const char*,
    double&,
    bool,
    float,
    float,
    const char*,
    ImGuiInputTextFlags);
template bool CImGuiManager::Input<D3DXVECTOR2>(const char*,
    D3DXVECTOR2&,
    bool,
    float,
    float,
    const char*,
    ImGuiInputTextFlags);
template bool CImGuiManager::Input<D3DXVECTOR3>(const char*,
    D3DXVECTOR3&,
    bool,
    float,
    float,
    const char*,
    ImGuiInputTextFlags);
template bool CImGuiManager::Input<std::string>(const char*,
    std::string&,
    bool,
    float,
    float,
    const char*,
    ImGuiInputTextFlags);


template<typename T>
void CImGuiManager::Slider(const char* label, T& value, T valueMin, T valueMax, bool isLabel)
{
    const std::string newlabel = "##" + std::string(label);

    if (isLabel == true)
    {
        ImGui::Text(label);
        ImGui::SameLine(SAMALINE_OFFSET);
    }

    if constexpr (std::is_same_v<T, int>)
    {
        ImGui::SliderInt(
            newlabel.c_str(),
            &value,
            valueMin,
            valueMax);
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        ImGui::SliderFloat(
            newlabel.c_str(),
            &value,
            valueMin,
            valueMax);
    }
}

template void CImGuiManager::Slider<int>(
    const char*,
    int&,
    int,
    int,
    bool);
template void CImGuiManager::Slider<float>(const char*,
    float&,
    float,
    float,
    bool);

std::string CImGuiManager::Combo(const char* Label,
    std::string& NowItem,
    const std::vector<std::string>& List,
    bool isLabel,
    float space)
{
    int No = 0;
    const int List_Max = static_cast<int>(List.size());
    for (int i = 0; i < List_Max; ++i)
    {
        if (List[i] == NowItem)
        {
            No = i;
            break;
        }
    }

    if (isLabel == true)
    {
        ImGui::Text(Label);
        ImGui::SameLine(space);
    }

    if (ImGui::BeginCombo(std::string("##" + std::string(Label)).c_str(),
        NowItem.c_str()))
    {
        const int Size = static_cast<int>(List.size());
        for (int i = 0; i < Size; ++i)
        {
            bool is_Selected = (NowItem == List[i]);
            if (ImGui::Selectable(List[i].c_str(),
                is_Selected))
            {
                No = i;
            }
            if (is_Selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return NowItem = List[No];
}

bool CImGuiManager::CheckBox(
    const char* label,
    bool& flag,
    bool isLabel)
{
    if (isLabel == true)
    {
        ImGui::Text(label);
        ImGui::SameLine(SAMALINE_OFFSET);
    }
    return ImGui::Checkbox(
        std::string("##" + std::string(label)).c_str(),
        &flag);
}

void CImGuiManager::Graph(
    const char* Label,
    std::vector<float>& Data,
    const ImVec2& Size)
{
    ImGui::PlotLines(std::string("##" + std::string(Label)).c_str(),
        Data.data(), static_cast<int>(Data.size()),
        0, nullptr,
        FLT_MAX,
        FLT_MAX,
        Size);
}

// Collider schedule accessors
bool CImGuiManager::GetColliderSchedule(const std::string& stateName, size_t index, ColliderSchedule& out)
{
    auto it = GetInstance()->m_ColliderSchedules.find(stateName);
    if (it == GetInstance()->m_ColliderSchedules.end()) return false;
    const auto& vec = it->second;
    if (index >= vec.size()) return false;
    out = vec[index];
    return true;
}

void CImGuiManager::SetColliderSchedule(const std::string& stateName, size_t index, const ColliderSchedule& sched)
{
    auto& mapRef = GetInstance()->m_ColliderSchedules;
    auto it = mapRef.find(stateName);
    if (it == mapRef.end())
    {
        // create vector sized to index+1
        std::vector<ColliderSchedule> v(index + 1);
        v[index] = sched;
        mapRef[stateName] = std::move(v);
        return;
    }
    auto& vec = it->second;
    if (index >= vec.size()) vec.resize(index + 1);
    vec[index] = sched;
}
