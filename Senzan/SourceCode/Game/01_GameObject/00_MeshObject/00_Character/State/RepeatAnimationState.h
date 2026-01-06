#pragma once

#include "Game/01_GameObject/00_MeshObject/00_Character/Character.h"
#include "System/Utility/StateMachine/StateBase.h"
#include <string>


template<typename FSM_Owner>
class RepeatAnimationState : public StateBase<FSM_Owner>
{
public:
    RepeatAnimationState(Character* owner, const std::string& stateName, int animationIndex, float animSpeed = 1.0f);
    virtual ~RepeatAnimationState();

    virtual void Enter() override;
    virtual void Update() override;
    virtual void Exit() override;

private:
    Character* m_pOwner;
    std::string m_StateName;
    int m_AnimationIndex;
    float m_AnimSpeed;
};
