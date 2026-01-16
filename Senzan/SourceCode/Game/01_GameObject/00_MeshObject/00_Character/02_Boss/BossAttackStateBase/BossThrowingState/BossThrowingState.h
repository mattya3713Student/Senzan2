#pragma once
#include "..//BossAttackStateBase.h"

#include "00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"

#include "00_MeshObject/00_Character/03_SnowBall/SnowBall.h"


class BossThrowingState
    : public BossAttackStateBase
{
public:
    enum class enThrowing : byte
    {
        None,
        Anim,
        Attack,
        CoolDown,
        Trans
    };

public:
    BossThrowingState(Boss* owner);
    ~BossThrowingState() override;

    void Enter() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void Exit() override;
private:
    void BossAttack();
private:
    std::unique_ptr<SnowBall> m_pBall;
    enThrowing m_List;
    bool m_IsLaunched;
};

