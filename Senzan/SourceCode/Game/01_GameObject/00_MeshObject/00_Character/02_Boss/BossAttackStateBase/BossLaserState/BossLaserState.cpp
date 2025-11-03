#include "BossLaserState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"

constexpr float MY_PI = 3.1415926535f;


BossLaserState::BossLaserState(Boss* owner)
	: BossAttackStateBase(owner)
    , m_ChargeDuration(1.5f)      // 1.5秒予兆
    , m_AttackDuration(1.0f)      // 1.0秒かけて薙ぎ払う
    , m_CoolDownDuration(1.5f)      // 1.5秒硬直
    , m_LaserLenght(30.0f)     // 射程30m
    , m_SweepSpeed(MY_PI / 1.0f) // 1秒で180度(PI)回転
    , m_StartAnglePitch(0.0f)      // 水平(0度)から開始
    , m_EndAnglePitch(-MY_PI / 3.0f) // -60度まで回転
    , m_CurrentPhase(Phase::Charge)
    , m_PhaseTime(0.0f)
    , m_CurrentAngle(0.0f)
    , m_LaserOridin()
{
}

BossLaserState::~BossLaserState()
{
}

void BossLaserState::Enter()
{
    m_Attacktime = 0.0f;
    m_PhaseTime = 0.0f;
    m_CurrentPhase = Phase::Charge;

    //方向をむく.

    DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
    //ここにレーザーの生成位置を設定する.
    //今はまだ発射させないらかかない最終的にはm_LaserOridinを使用してポジションを取得する.
    m_CurrentAngle = m_StartAnglePitch;
}

void BossLaserState::Update()
{
    const float Time_Rate = 1.0f;

    const float deltaTime = Time::GetInstance().GetDeltaTime();

    //全体の時間とフェーズ内時間を進める.
    m_Attacktime += Time_Rate * Time::GetInstance().GetDeltaTime();
    m_PhaseTime += Time::GetInstance().GetDeltaTime();


    switch (m_CurrentPhase)
    {
    case BossLaserState::Phase::Charge:
        if (m_PhaseTime >= m_ChargeDuration)
        {
            //予兆から攻撃.
            m_CurrentPhase = Phase::Attack;
            m_PhaseTime = 0.0f;
        }
        break;
    case BossLaserState::Phase::Attack:
        BossAttack();
        if (m_PhaseTime >= m_AttackDuration)
        {
            //攻撃からクールダウン.
            m_CurrentPhase = Phase::CoolDown;
            m_PhaseTime = 0.0f;
        }
        break;
    case BossLaserState::Phase::CoolDown:
        if (m_PhaseTime >= m_CoolDownDuration)
        {
            //動作クラスへの移動.
            m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>(m_pOwner));
            return;
        }
        break;
    default:
        break;
    }
}

void BossLaserState::LateUpdate()
{
}

void BossLaserState::Draw()
{
}

void BossLaserState::Exit()
{
}

void BossLaserState::BossAttack()
{
    //
}
