#include "BossMove.h"
#include "..//..//..//..//..//Boss.h"
#include <algorithm>
#include <cmath>

namespace BossState
{
    BossMove::BossMove(Boss* pOwner)
        : BossMovement      (pOwner)
        , m_Phase           (MovePhase::Start)
        , m_BaseAngle       (0.0f)
        , m_Timer           (0.0f)
        , m_StrafeFrequency (1.5f)
        , m_StrafeAmplitude (DirectX::XM_PIDIV2)

        , m_RotationAngle   (0.0f)
    {
    }

    BossMove::~BossMove()
    {
    }

    void BossMove::Enter()
    {
        m_Timer = 0.0f;
        m_Phase = MovePhase::Start;

        if (m_pOwner)
        {
            m_pOwner->SetAnimSpeed(MOVE_RUN_ANIM_SPEED);
            m_pOwner->ChangeAnim(Boss::enBossAnim::IdolToRun);
        }
    }

    void BossMove::Update()
    {
        if (!m_pOwner) 
        {
            return; 
        }

        // 共通：プレイヤーを向く処理（基底クラスの関数と仮定）
        RotateToPlayer();

        float deltaTime = m_pOwner->GetDelta();
        DirectX::XMFLOAT3 bossPosF = m_pOwner->GetPosition();
        DirectX::XMFLOAT3 playerPosF = m_pOwner->GetTargetPos();

        DirectX::XMVECTOR vBossPos = DirectX::XMLoadFloat3(&bossPosF);
        DirectX::XMVECTOR vTargetPos = DirectX::XMLoadFloat3(&playerPosF);

        DirectX::XMVECTOR vToPlayer = DirectX::XMVectorSubtract(vTargetPos, vBossPos);
        vToPlayer = DirectX::XMVectorSetY(vToPlayer, 0.0f);
        float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(vToPlayer));

        switch (m_Phase)
        {
        case MovePhase::Start:
            if (m_pOwner->IsAnimEnd(Boss::enBossAnim::IdolToRun)) {
                m_pOwner->ChangeAnim(Boss::enBossAnim::Run);
                m_Phase = MovePhase::Run;
            }
            break;

        case MovePhase::Run:
        {
            DirectX::XMVECTOR vDir = DirectX::XMVector3Normalize(vToPlayer);
            vBossPos = DirectX::XMVectorAdd(vBossPos, DirectX::XMVectorScale(vDir, APPROACH_SPEED * deltaTime));

            if (distance <= STRAFE_RANGE) {
                m_Phase = MovePhase::Stop;
                m_pOwner->ChangeAnim(Boss::enBossAnim::RunToIdol);
            }
        }
        break;

        case MovePhase::Stop:
            if (m_pOwner->IsAnimEnd(Boss::enBossAnim::RunToIdol)) {
                m_Phase = MovePhase::Strafe;
                // 開始時のプレイヤーから見たボスの位置を角度として保持
                DirectX::XMVECTOR vDirFromPlayer = DirectX::XMVectorSubtract(vBossPos, vTargetPos);
                m_BaseAngle = atan2f(DirectX::XMVectorGetX(vDirFromPlayer), DirectX::XMVectorGetZ(vDirFromPlayer));
                m_Timer = 0.0f; // サイン波を中央（0）から開始
            }
            break;

        case MovePhase::Strafe:
        {
            // 1. 時間を進める
            m_Timer += deltaTime * m_StrafeFrequency;

            // 2. 理想の角度を計算 (sin関数で -1.0 ～ 1.0 の間を往復)
            // 振幅(Amplitude)をかけることで、移動する角度の幅を決定
            float currentSwayAngle = sinf(m_Timer) * m_StrafeAmplitude;

            // 3. 理想の座標を計算
            float finalAngle = m_BaseAngle + currentSwayAngle;
            DirectX::XMVECTOR vOffset = DirectX::XMVectorSet(
                sinf(finalAngle) * STRAFE_RANGE,
                0.0f,
                cosf(finalAngle) * STRAFE_RANGE,
                0.0f
            );
            DirectX::XMVECTOR vIdealPos = DirectX::XMVectorAdd(vTargetPos, vOffset);

            // 4. 補間による追従
            // TRACKING_DELAYを大きく設定しているため、高速移動でも端まで届く
            float lerpFactor = TRACKING_DELAY * deltaTime;
            if (lerpFactor > 1.0f) lerpFactor = 1.0f;
            vBossPos = DirectX::XMVectorLerp(vBossPos, vIdealPos, lerpFactor);

            // 5. アニメーション判定 (サイン波の微分であるcos波で向きを判断)
            m_pOwner->SetAnimSpeed(STRAFE_ANIM_SPEED);
            if (cosf(m_Timer) > 0) {
                m_pOwner->ChangeAnim(Boss::enBossAnim::LeftMove);
            }
            else {
                m_pOwner->ChangeAnim(Boss::enBossAnim::RightMove);
            }
        }
        break;
        }

        // 座標の適用
        DirectX::XMFLOAT3 finalPos;
        DirectX::XMStoreFloat3(&finalPos, vBossPos);
        finalPos.y = bossPosF.y;
        m_pOwner->SetPosition(finalPos);
    }

    void BossMove::LateUpdate()
    {
        using namespace DirectX;
        if (!m_pOwner) return;

        XMFLOAT3 currentPos = m_pOwner->GetPosition();
        XMFLOAT3 targetPos = m_pOwner->GetTargetPos();

        float dx = targetPos.x - currentPos.x;
        float dz = targetPos.z - currentPos.z;

        // プレイヤーを正面に捉える計算
        float angle = atan2f(-dx, -dz) + XM_PI;
        m_pOwner->SetRotationY(angle);
    }

    void BossMove::Draw() {}
    void BossMove::Exit() {}

    constexpr BossState::enID BossMove::GetStateID() const
    {
        return BossState::enID::Move;
    }
}