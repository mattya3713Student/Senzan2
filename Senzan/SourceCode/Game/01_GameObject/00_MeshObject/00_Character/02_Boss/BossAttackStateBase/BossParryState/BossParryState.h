#pragma once

#include "..//BossAttackStateBase.h"

// ボスのパリィ（怯み）専用ステート
class BossParryState : public BossAttackStateBase
{
public:
    enum class ParryPhase : byte
    {
        None,
        Flinch,
        FlinchTimer,
        FlinchToIdol,
    };

    BossParryState(Boss* owner);
    ~BossParryState() override;

    void Enter() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void Exit() override;

private:
    void BossAttack() override {}

    ParryPhase m_Phase;
};
