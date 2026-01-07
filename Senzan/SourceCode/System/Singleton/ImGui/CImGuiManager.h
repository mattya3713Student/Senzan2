#pragma once

#include "../../Data/ImGui/Library/imgui_impl_dx11.h"
#include "../../Data/ImGui/Library/imgui_impl_win32.h"

#include <string>
#include <vector>
#include <unordered_map>

/*****************************************************
*    ImGui manager singleton.
**/

class CImGuiManager final
{
public:
    // Get singleton.
    static CImGuiManager* GetInstance();

    // Initialization.
    static HRESULT Init(HWND hWnd);

    // Release.
    static void Relese();

    // New frame setup.
    static void NewFrameSetting();

    // Render.
    static void Render();

    // Generic input helpers (existing)...
    template<typename T>
    static bool Input(
        const char* label,
        T& value,
        bool isLabel = true,
        float step = 0.f,
        float steoFast = 0.f,
        const char* format = "% .3f",
        ImGuiInputTextFlags flags = 0);

    template<typename T>
    static void Slider(
        const char* label,
        T& value,
        T valueMin,
        T valieMax,
        bool isLabel = true);

    static std::string Combo(
        const char* label,
        std::string& NowItem,
        const std::vector<std::string>& List,
        bool isLabel = false,
        float space = 100.f);

    static bool CheckBox(
        const char* Label,
        bool& flag,
        bool isLabel = true);

    static void Graph(
        const char* Label,
        std::vector<float>& Data,
        const ImVec2& Size = ImVec2(300.f, 300.f));

    // Collider schedule struct and accessors
    struct ColliderSchedule
    {
        float Delay = 0.0f;
        float Duration = 0.0f;
    };

    // Get schedule for a named state + index. Returns true if schedule exists (out param written), false otherwise.
    static bool GetColliderSchedule(const std::string& stateName, size_t index, ColliderSchedule& out);

    // Set schedule for a named state + index.
    static void SetColliderSchedule(const std::string& stateName, size_t index, const ColliderSchedule& sched);

private:
    CImGuiManager();
    ~CImGuiManager();

    CImGuiManager(const CImGuiManager& rhs) = delete;
    CImGuiManager& operator = (const CImGuiManager& rhs) = delete;

    // Internal storage for schedules: map stateName -> vector of schedules
    std::unordered_map<std::string, std::vector<ColliderSchedule>> m_ColliderSchedules;
};
