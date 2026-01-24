#pragma once

#include "..//BossAttackStateBase.h"

// ボスのパリィ（怯み）専用ステート
class BossParryState : public BossAttackStateBase
{
public:
    enum class ParryPhase : byte
    {
        None,
        PlayHit,
        PlayFlinchParis,
        PlayFlinch,
        PlayFlinchToIdol,
        ToIdol,
    };

    BossParryState(Boss* owner);
    BossParryState(Boss* owner, bool withDelay, float delaySeconds = 2.0f);
    ~BossParryState() override;

    void Enter() override;
    void Update() override;
    void LateUpdate() override;
    void Draw() override;
    void Exit() override;

private:

    ParryPhase m_Phase;
    bool m_WithDelay = false;
    float m_DelaySeconds = 0.0f;
    float m_DelayElapsed = 0.0f;
};
