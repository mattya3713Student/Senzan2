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
    if (!m_pOwner) return;

    // Determine entity type string for manager lookup
    std::string entityType = dynamic_cast<Player*>(m_pOwner) ? "Player" : "Boss";

    // Manager may hold a selected state via ImGui; use that if set, otherwise fallback to m_StateName
    std::string selected = AnimationTuningManager::GetInstance().GetSelectedState(entityType);
    const std::string& applyState = selected.empty() ? m_StateName : selected;

    // Apply per-frame so UI changes are immediately reflected (debug-only state)
    AnimationTuningManager::GetInstance().ApplyAnimationState(m_pOwner, applyState);
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
