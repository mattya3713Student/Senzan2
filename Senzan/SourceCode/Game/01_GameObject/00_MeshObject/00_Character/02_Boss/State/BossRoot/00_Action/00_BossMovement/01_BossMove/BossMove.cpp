#include "BossMove.h"
#include "..//..//..//..//..//Boss.h"
#include <algorithm>
#include <cmath>

namespace BossState
{
    BossMove::BossMove(Boss* pOwner)
        : BossMovement(pOwner)
        , m_Phase(MovePhase::Start)
        , m_Angle(0.0f)
        , m_StrafeSpeed(0.8f)
        , m_RotationAngle(0.0f)
        , m_RotationSpeed(0.5f)
        , m_rotationDirection(1.0f)
        , m_BaseAngle(0.0f)
        , m_Timer(0.0f)
    {
    }

    BossMove::~BossMove()
    {
    }

    void BossMove::Enter()
    {
        // 初期状態のリセット
        m_Timer = 0.0f;
        m_RotationAngle = 0.0f;
        m_rotationDirection = 1.0f;
        m_Phase = MovePhase::Start;

        if (m_pOwner) {
            m_pOwner->SetAnimSpeed(MOVE_RUN_ANIM_SPEED);
            m_pOwner->ChangeAnim(Boss::enBossAnim::IdolToRun);
        }
    }

    void BossMove::Update()
    {
        using namespace DirectX;
        if (!m_pOwner) return;

        RotateToPlayer();

        float deltaTime = m_pOwner->GetDelta();

        // 1. 座標情報の取得
        XMFLOAT3 bossPosF = m_pOwner->GetPosition();
        XMFLOAT3 playerPosF = m_pOwner->GetTargetPos();

        XMVECTOR vBossPos = XMLoadFloat3(&bossPosF);
        XMVECTOR vTargetPos = XMLoadFloat3(&playerPosF);

        // プレイヤーへの方向と距離計算（Y軸無視）
        XMVECTOR vToPlayer = XMVectorSubtract(vTargetPos, vBossPos);
        vToPlayer = XMVectorSetY(vToPlayer, 0.0f);
        float distance = XMVectorGetX(XMVector3Length(vToPlayer));

        // 2. フェーズ別移動ロジック
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
            // 直進接近
            XMVECTOR vDir = XMVector3Normalize(vToPlayer);
            vBossPos = XMVectorAdd(vBossPos, XMVectorScale(vDir, APPROACH_SPEED * deltaTime));

            if (distance <= STRAFE_RANGE) {
                m_Phase = MovePhase::Stop;
                m_pOwner->ChangeAnim(Boss::enBossAnim::RunToIdol);
            }
        }
        break;

        case MovePhase::Stop:
            // 止まる動作を待ってから回り込み開始
            if (m_pOwner->IsAnimEnd(Boss::enBossAnim::RunToIdol)) {
                m_Phase = MovePhase::Strafe;
                // 開始時のプレイヤーから見たボスの角度を保存
                XMVECTOR vDirFromPlayer = XMVectorSubtract(vBossPos, vTargetPos);
                m_BaseAngle = atan2f(XMVectorGetX(vDirFromPlayer), XMVectorGetZ(vDirFromPlayer));
                m_RotationAngle = 0.0f;
            }
            break;

        case MovePhase::Strafe:
        {
            // 理想の角度更新 (往復運動)
            m_RotationAngle += static_cast<float>(m_RotationSpeed) * deltaTime * m_rotationDirection;
            const float MAX_SWAY = XM_PIDIV4; // 45度
            if (fabsf(m_RotationAngle) > MAX_SWAY) {
                m_rotationDirection *= -1.0f;
                m_RotationAngle = std::clamp(m_RotationAngle, -MAX_SWAY, MAX_SWAY);
            }

            // 理想の座標計算
            float finalAngle = m_BaseAngle + m_RotationAngle;
            XMVECTOR vOffset = XMVectorSet(
                sinf(finalAngle) * STRAFE_RANGE,
                0.0f,
                cosf(finalAngle) * STRAFE_RANGE,
                0.0f
            );
            XMVECTOR vIdealPos = XMVectorAdd(vTargetPos, vOffset);

            // 補間による滑らかな追尾 (TRACKING_DELAY)
            float lerpFactor = TRACKING_DELAY * deltaTime;
            if (lerpFactor > 1.0f) lerpFactor = 1.0f;
            vBossPos = XMVectorLerp(vBossPos, vIdealPos, lerpFactor);

            // アニメーション更新
            m_pOwner->SetAnimSpeed(STRAFE_ANIM_SPEED);
            if (m_rotationDirection > 0)
                m_pOwner->ChangeAnim(Boss::enBossAnim::LeftMove);
            else
                m_pOwner->ChangeAnim(Boss::enBossAnim::RightMove);
        }
        break;
        }

        // 座標の適用
        XMFLOAT3 finalPos;
        XMStoreFloat3(&finalPos, vBossPos);
        finalPos.y = bossPosF.y; // 高さは維持
        m_pOwner->SetPosition(finalPos);
    }

    void BossMove::LateUpdate()
    {
        using namespace DirectX;
        if (!m_pOwner) return;

        // 3. プレイヤー注視の完全修正
        // Updateで動いた後の最新位置から向きを計算
        XMFLOAT3 currentPos = m_pOwner->GetPosition();
        XMFLOAT3 targetPos = m_pOwner->GetTargetPos();

        float dx = targetPos.x - currentPos.x;
        float dz = targetPos.z - currentPos.z;

        // モデルが正面を向くように XM_PI(180度) を加算
        // これでも逆を向く場合は、加算を消すか角度を調整してください
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