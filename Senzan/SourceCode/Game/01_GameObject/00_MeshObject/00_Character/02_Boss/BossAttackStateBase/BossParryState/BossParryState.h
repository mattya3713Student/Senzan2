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

    // PlayerのParry成功時硬直させたいアニメーションとタイミング.
    std::pair<Boss::enBossAnim, float> GetParryAnimPair() override;

private:

    ParryPhase m_Phase;
    bool m_IsFastTiming = false;
    bool m_WithDelay = false;
    float m_DelaySeconds = 0.0f;
    float m_DelayElapsed = 0.0f;
    // 再生開始アニメと停止参照アニメ
    Boss::enBossAnim m_StartAnim = Boss::enBossAnim::none;
    Boss::enBossAnim m_StopRefAnim = Boss::enBossAnim::none;

};
