#include "RepeatAnimationState.h"
#include "System/Singleton/Debug/AnimationTuningManager/AnimationTuningManager.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include <type_traits>
#include <vector>
#include <algorithm>

template<typename FSM_Owner>
RepeatAnimationState<FSM_Owner>::RepeatAnimationState(Character* owner, const std::string& stateName, int animationIndex, float animSpeed)
    : StateBase<FSM_Owner>(static_cast<FSM_Owner*>(owner))
    , m_pOwner(owner)
    , m_StateName(stateName)
    , m_AnimationIndex(animationIndex)
    , m_AnimSpeed(animSpeed)
{
}

template<typename FSM_Owner>
RepeatAnimationState<FSM_Owner>::~RepeatAnimationState() {}

template<typename FSM_Owner>
void RepeatAnimationState<FSM_Owner>::Enter()
{
    if (!m_pOwner) return;
    m_pOwner->SetAnimSpeed(m_AnimSpeed);
    m_pOwner->SetIsLoop(true);
    m_pOwner->ChangeAnim(m_AnimationIndex);

    // If tuning has colliders for this state, ensure they are applied
    AnimationTuningManager::GetInstance().ApplyAnimationState(m_pOwner, m_StateName);
}

template<typename FSM_Owner>
void RepeatAnimationState<FSM_Owner>::Update()
{
    // Keep looping animation; and provide a small ImGui to switch by tuning-state
    if (!m_pOwner) return;

    // Only render for Player or Boss owners
    if constexpr (std::is_same_v<FSM_Owner, Player> || std::is_same_v<FSM_Owner, Boss>)
    {
        auto& mgr = AnimationTuningManager::GetInstance();
        const char* entityType = std::is_same_v<FSM_Owner, Player> ? "Player" : "Boss";

        // Fetch (or prepare) tuning block
        EntityTuning* tuning = mgr.GetEntityTuning(entityType);
        if (!tuning)
        {
            mgr.GetAnimationStateTuning(entityType, "Default");
            tuning = mgr.GetEntityTuning(entityType);
        }

        if (!tuning) return;

        const char* windowTitle = std::is_same_v<FSM_Owner, Player> ? "Player Animation" : "Boss Animation";
        // Use centralized manager to begin/end state debug window so all debug UI is controlled here
        if (mgr.BeginStateDebugWindow(windowTitle))
        {
            // Build state list from tuning data
            std::vector<std::string> stateNames;
            stateNames.reserve(tuning->states.size());
            for (const auto& kv : tuning->states) stateNames.push_back(kv.first);

            // Sort for stable UI
            std::sort(stateNames.begin(), stateNames.end());

            // Keep selection index per owner type
            static int s_idxPlayer = 0;
            static int s_idxBoss = 0;
            int& idx = (std::is_same_v<FSM_Owner, Player> ? s_idxPlayer : s_idxBoss);
            if (!stateNames.empty())
            {
                std::vector<const char*> items;
                items.reserve(stateNames.size());
                for (auto& n : stateNames) items.push_back(n.c_str());

                ImGui::Combo("State", &idx, items.data(), static_cast<int>(items.size()));
                ImGui::SameLine();
                if (ImGui::Button("Apply") && idx >= 0 && idx < static_cast<int>(stateNames.size()))
                {
                    const std::string& selected = stateNames[idx];
                    // Apply tuning (colliders etc.)
                    mgr.ApplyAnimationState(m_pOwner, selected);

                    // Read anim settings if available
                    if (auto* st = mgr.GetAnimationStateTuning(entityType, selected))
                    {
                        m_AnimSpeed = st->animSpeed;
                        m_AnimationIndex = st->animationIndex;
                        m_StateName = selected;

                        m_pOwner->SetAnimSpeed(m_AnimSpeed);
                        m_pOwner->SetIsLoop(st->loop);
                        m_pOwner->ChangeAnim(m_AnimationIndex);
                    }
                }

                // --- New: animation time display and controls ---
                double animTime = m_pOwner->GetAnimTime();
                ImGui::Text("AnimTime: %.3f", animTime);

                static double inputAnimTime = 0.0;
                inputAnimTime = animTime;
                if (ImGui::InputDouble("Set Anim Time (s)", &inputAnimTime, 0.0, 0.0, "%.3f"))
                {
                    m_pOwner->SetAnimTime(inputAnimTime);
                }

                // Stop / Resume / Step controls
                if (ImGui::Button("Stop"))
                {
                    m_pOwner->StopAnimation();
                }
                ImGui::SameLine();
                if (ImGui::Button("Play 1.0x"))
                {
                    m_pOwner->SetAnimSpeed(1.0);
                }

                ImGui::SameLine();
                if (ImGui::Button("<< 1 Frame"))
                {
                    // Step back one frame (assume 60fps frame step)
                    double frameSec = 1.0 / 60.0;
                    double cur = m_pOwner->GetAnimTime();
                    double newT = cur - frameSec;
                    if (newT < 0.0) newT = 0.0;
                    m_pOwner->SetAnimTime(newT);
                }

                ImGui::SameLine();
                if (ImGui::Button(">> 1 Frame"))
                {
                    double frameSec = 1.0 / 60.0;
                    double cur = m_pOwner->GetAnimTime();
                    double newT = cur + frameSec;
                    // clamp to animation period if possible
                    double period = m_pOwner->GetAnimPeriod(m_AnimationIndex);
                    if (period > 0.0 && newT > period) newT = period;
                    m_pOwner->SetAnimTime(newT);
                }
                // --- end new controls ---
            }
            mgr.EndStateDebugWindow();
        }
    }
}

template<typename FSM_Owner>
void RepeatAnimationState<FSM_Owner>::Exit()
{
    if (!m_pOwner) return;
    // Reset loop flag
    m_pOwner->SetIsLoop(false);
}

// Explicit instantiations for FSM owner types used in this project
template class RepeatAnimationState<Player>;
template class RepeatAnimationState<Boss>;
