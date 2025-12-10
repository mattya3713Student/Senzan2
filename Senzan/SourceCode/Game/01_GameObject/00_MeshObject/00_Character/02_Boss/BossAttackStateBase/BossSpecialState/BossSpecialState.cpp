#include "BossSpecialState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h"

BossSpecialState::BossSpecialState(Boss* owner)
    : BossAttackStateBase(owner)
    , m_CurrentPhase(Phase::Charge)
    , m_PhaseTime(0.0f)
    , m_AttackTime(0.0f)
    , m_HasHit(false)
    // 時間設定
    , m_ChargeDuration(1.5f)      // 溜め時間: 1.5秒
    , m_JumpDuration(1.0f)        // ジャンプ移動時間: 1.0秒
    , m_AttackDuration(1.0f)      // 突進斬り時間: 1.0秒
    , m_CoolDownDuration(1.0f)    // 硬直時間: 1.0秒    // Jump パラメータ
    , m_JumpHeght(5.0f)      // 最大到達高度 (地表からの高さ)
    // Attack パラメータ
    //m_AttackSpeedなんだけど距離になっている.
    , m_AttackSpeed(30.0f)        // 突進の速度: 30.0 ユニット/秒 (要調整)
    , m_SlashRange(1.0f) // 突進斬りの判定幅 (未使用、突進ロジックで使用予定)
    // 位置と方向
    , m_StartPos()
    , m_JumpTargetPos()
    , m_AttackDir()
{
}

BossSpecialState::~BossSpecialState()
{
}

//void BossSpecialState::Enter()
//{
//    //Bossの行動の初期化.
//    m_AttackTime = 0.0f;
//    m_PhaseTime = 0.0f;
//    m_CurrentPhase = Phase::Charge;
//
//    //Bossの向きを設定(チャージ開始時にプレイヤーの方向に向く).
//    //Bossの位置を取得する.
//    const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
//    DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);
//
//    //Playerの位置を取得する.
//    const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
//    DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);
//
//    //PlayerとBossの距離を求める？
//    DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
//    //PlayerとBossの距離を取得する.
//    Direction = DirectX::XMVectorSetY(Direction, 0.0f);
//
//    //Y軸回転角度を計算して、BossをPlayerの方へ向かせる.
//    float dx = DirectX::XMVectorGetX(Direction);
//    float dz = DirectX::XMVectorGetZ(Direction);
//    float Angle_Radian = std::atan2(dx, dz);
//    m_pOwner->SetRotationY(Angle_Radian);
//
//    //攻撃開始位置の保存.
//    DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);
//
//    //Jumpフェーズの目的地点(突進の地点)を計算.
//    DirectX::XMVECTOR DirToPlayerXZ = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
//    DirToPlayerXZ = DirectX::XMVectorSetY(DirToPlayerXZ, 0.0f);
//    //正規化.
//    DirToPlayerXZ = DirectX::XMVector3Normalize(DirToPlayerXZ);
//
//    // Bossの着地地点の設定.
//        // 変更点: JumpTargetPosをプレイヤーのXZ位置、Yを地面の高さ(0.0f)に設定する.
//    DirectX::XMVECTOR TargetPosXM = DirectX::XMLoadFloat3(&PlayerPosF); // PlayerPosFを使用
//    TargetPosXM = DirectX::XMVectorSetY(TargetPosXM, 0.0f); // Y座標を地面に固定
//
//    // BossのJumpの最高地点を設定しているコードは、JumpTargetPosの計算では不要なため削除/修正.
//    // TargetPosXM = DirectX::XMVectorSetY(TargetPosXM, 5.0f); // 削除するかコメントアウト
//
//    // JumpTargetPosは、Jumpフェーズの終点（Attackフェーズの始点）= プレイヤーの足元に設定する.
//    DirectX::XMStoreFloat3(&m_JumpTargetPos, TargetPosXM);
//
//    //位置をm_StartPosに固定する.
//    m_pOwner->SetPosition(m_StartPos);
//}


void BossSpecialState::Enter()
{
    // ボスの行動の初期化.
    m_AttackTime = 0.0f;
    m_PhaseTime = 0.0f;
    m_CurrentPhase = Phase::Charge;

    // Bossの向きを設定(チャージ開始時にプレイヤーの方向に向く).
    // Bossの位置を取得する.
    const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
    DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

    // Playerの位置を取得する.
    const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
    DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

    // PlayerとBossの方向ベクトルを求める.
    DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);

    // Y軸方向の成分を0にして、水平方向の向きだけにする.
    Direction = DirectX::XMVectorSetY(Direction, 0.0f);

    // Y軸回転角度を計算して、BossをPlayerの方へ向かせる.
    float dx = DirectX::XMVectorGetX(Direction);
    float dz = DirectX::XMVectorGetZ(Direction);
    float Angle_Radian = std::atan2(dx, dz);
    m_pOwner->SetRotationY(Angle_Radian);

    // 攻撃開始位置の保存.
    // ※ Jumpフェーズの始点 (水平移動と放物線計算の基準点)
    //始まった瞬間にまず最初の位置にボスをワープさせて攻撃をする.
    DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);

    // Jumpフェーズの目標地点 (着地地点 / 突進の始点) を計算.
    // 目標地点はプレイヤーのXZ座標とし、Y座標を地面の高さ(0.0f)に固定する.
    //Playerのポジションをターゲットに変更する.
    DirectX::XMVECTOR TargetPosXM = PlayerPosXM;

    // Y座標を地面に固定する. これが距離に関わらずジャンプで到達するためのポイント.
    TargetPosXM = DirectX::XMVectorSetY(TargetPosXM, 0.0f);

    // m_JumpTargetPosに保存する.
    DirectX::XMStoreFloat3(&m_JumpTargetPos, TargetPosXM);

    // 位置をm_StartPosに固定する. (チャージ開始時は移動しないため)
    //原点？.
    m_pOwner->SetPosition(m_StartPos);
}

void BossSpecialState::Update()
{
    //----------------------------------------
    // Bossの攻撃.
    //----------------------------------------
    const float deltaTime = Time::GetInstance().GetDeltaTime();

    //時間の更新.
    m_AttackTime += deltaTime;
    m_PhaseTime += deltaTime;

    switch (m_CurrentPhase)
    {
    case BossSpecialState::Phase::Charge:
        if (m_PhaseTime >= m_ChargeDuration)
        {
            m_PhaseTime -= m_ChargeDuration;
            //次の動作へ移動.
            m_CurrentPhase = Phase::Jump;
        }
        break;
    case BossSpecialState::Phase::Jump:
    {
        float t = m_PhaseTime / m_JumpDuration;

        t = std::min(t, 1.0f);

        DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&m_StartPos);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&m_JumpTargetPos);

        // 水平移動: StartからTargetに移動.
        DirectX::XMVECTOR XZPos = DirectX::XMVectorLerp(Start, Target, t);

        // 垂直移動: 山なりに描くような感じにする.
        float StartY = m_StartPos.y; // Bossの初期Y座標 (通常は0.0f)
        float TargetY = m_JumpTargetPos.y; // JumpTargetPosのY座標 (通常は0.0f)

        // Yの線形補間
        float LinearY = StartY * (1.0f - t) + TargetY * t;

        // 放物線カーブの計算（t=0.5で最高点m_JumpHeghtとなるように）
        float JumpCurve = m_JumpHeght * (1.0f - std::pow(2.0f * t - 1.0f, 2.0f));

        DirectX::XMVECTOR NewPos = DirectX::XMVectorSetY(XZPos, LinearY + JumpCurve);

        //Bossの位置を更新.
        DirectX::XMFLOAT3 NewPosF;
        DirectX::XMStoreFloat3(&NewPosF, NewPos);
        m_pOwner->SetPosition(NewPosF);

        if (m_PhaseTime >= m_JumpDuration)
        {
            //ジャンプが実行できたらAttackへ移動.
            m_PhaseTime -= m_JumpDuration;
            m_CurrentPhase = Phase::Attack;

            // Attackフェーズ開始時: 最初の突進方向を決定する（現在のBossの位置からPlayerの方）.
            // ※ Attackフェーズに入ると、以下のAttackケース内で方向を毎フレーム更新します。
            DirectX::XMFLOAT3 PlayerPosXM = m_pOwner->m_PlayerPos;
            DirectX::XMVECTOR v_PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosXM);

            DirectX::XMFLOAT3 CurrentBossPos = m_pOwner->GetPosition();
            DirectX::XMVECTOR v_CurrentBossPos = DirectX::XMLoadFloat3(&CurrentBossPos);

            // Playerへ向かう方向ベクトル.
            DirectX::XMVECTOR Dir = DirectX::XMVectorSubtract(v_PlayerPosXM, v_CurrentBossPos);
            Dir = DirectX::XMVectorSetY(Dir, 0.0f); // Y成分を0にし、水平方向のみの突進にする

            //正規化.
            Dir = DirectX::XMVector3Normalize(Dir);
            DirectX::XMStoreFloat3(&m_AttackDir, Dir); //方向を保存.
        }
    }
    break;
    case BossSpecialState::Phase::Attack:
    {
        // 1. 突進方向を現在のPlayerの位置へ毎フレーム更新 (追尾)

        DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
        DirectX::XMVECTOR v_PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

        DirectX::XMFLOAT3 CurrentBossPos = m_pOwner->GetPosition();
        DirectX::XMVECTOR v_CurrentBossPos = DirectX::XMLoadFloat3(&CurrentBossPos);

        // Playerへ向かう方向ベクトルを再計算.
        DirectX::XMVECTOR Dir = DirectX::XMVectorSubtract(v_PlayerPosXM, v_CurrentBossPos);
        Dir = DirectX::XMVectorSetY(Dir, 0.0f); // 水平方向のみの追尾
        Dir = DirectX::XMVector3Normalize(Dir);
        DirectX::XMStoreFloat3(&m_AttackDir, Dir);


        // 2. 移動処理
        DirectX::XMVECTOR AttackDir = DirectX::XMLoadFloat3(&m_AttackDir);

        DirectX::XMVECTOR MoveDelta = DirectX::XMVectorScale(AttackDir, m_AttackSpeed * deltaTime);
        DirectX::XMVECTOR NewPos = DirectX::XMVectorAdd(v_CurrentBossPos, MoveDelta);

        // ボスの位置を更新.
        DirectX::XMFLOAT3 NewPosF;
        DirectX::XMStoreFloat3(&NewPosF, NewPos);
        m_pOwner->SetPosition(NewPosF);

        // 攻撃判定の実行.
        BossAttack();

        // 3. 終了条件: 時間切れ OR Playerへの近接到達 (確実な到達)

        // PlayerとBossの水平距離の二乗を計算する.
        DirectX::XMVECTOR Diff = DirectX::XMVectorSubtract(v_PlayerPosXM, v_CurrentBossPos);
        DirectX::XMVECTOR XZ_Diff = DirectX::XMVectorSetY(Diff, 0.0f);
        float distanceSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(XZ_Diff));

        // Playerに近づいたと判定するしきい値 (例: 2.0ユニットの二乗 = 4.0f)
        const float proximityThresholdSq = 4.0f;

        if (m_PhaseTime >= m_AttackDuration || distanceSq <= proximityThresholdSq)
        {
            m_PhaseTime -= m_AttackDuration;
            m_CurrentPhase = Phase::CoolDown;

            // 💡 備考: CoolDown移行時にPlayerの位置にZ軸を合わせるなど、最後の調整をしても良い
        }
    }
    break;
    case BossSpecialState::Phase::CoolDown:
        //硬直時間終了.
        if (m_PhaseTime >= m_CoolDownDuration)
        {
            //地面に確実に着地させる処理.
            DirectX::XMFLOAT3 PosF = m_pOwner->GetPosition();
            m_pOwner->SetPosition({ PosF.x, 0.0f, PosF.z });

            // 攻撃を最初から繰り返すか、次の移動ステートへ戻る
            m_CurrentPhase = Phase::Charge;
            // m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>(m_pOwner));
            // return;
        }
        break;
    default:
        break;
    }
}

void BossSpecialState::LateUpdate()
{
}

void BossSpecialState::Draw()
{
}

void BossSpecialState::Exit()
{
}

void BossSpecialState::BossAttack()
{
}
