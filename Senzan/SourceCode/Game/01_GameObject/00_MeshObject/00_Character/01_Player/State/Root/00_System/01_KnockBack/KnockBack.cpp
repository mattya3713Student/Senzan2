#include "KnockBack.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

#include "Game/04_Time/Time.h"

namespace PlayerState {
KnockBack::KnockBack(Player* owner)
	: System(owner)
{
}
KnockBack::~KnockBack()
{
}

// IDの取得.
constexpr PlayerState::eID KnockBack::GetStateID() const
{
	return PlayerState::eID::KnockBack;
}

void KnockBack::Enter()
{
    // 吹っ飛びベクトルを計算.
    DirectX::XMVECTOR v_knock_back_vec = DirectX::XMLoadFloat3(&m_pOwner->m_KnockBackVec);
    DirectX::XMVectorSetY(v_knock_back_vec, 8.f);
    DirectX::XMVectorScale(v_knock_back_vec, m_pOwner->m_KnockBackPower);
    DirectX::XMStoreFloat3(&m_pOwner->m_KnockBackVec, v_knock_back_vec);
}

void KnockBack::Update()
{ 
    // 1. ノックバックベクトルをロード
    DirectX::XMVECTOR v_knock_back_vec = DirectX::XMLoadFloat3(&m_pOwner->m_KnockBackVec);

    // 2. 経過時間 (deltaTime) を取得
    float deltaTime = Time::GetDeltaTime();

    // --- 💡 垂直方向 (Y軸) に重力加速度を適用 ---

    // 重力ベクトルを計算 (Y軸方向のみに加速度を適用)
    const float GRAVITY_ACCELERATION = -0.98f; // 重力加速度 (Y軸下向き)
    DirectX::XMVECTOR v_gravity = DirectX::XMVectorSet(0.0f, GRAVITY_ACCELERATION * deltaTime, 0.0f, 0.0f);

    // ノックバックベクトルに重力の影響を加える
    v_knock_back_vec = DirectX::XMVectorAdd(v_knock_back_vec, v_gravity);

    // --- 💡 空気抵抗（全軸共通の減衰）を適用 ---

    // 減衰率 (0.99f) は空気抵抗として機能する
    const float DAMPING_FACTOR = 0.99f;
    v_knock_back_vec = DirectX::XMVectorScale(v_knock_back_vec, DAMPING_FACTOR);

    // --- 処理結果の格納 ---

    // 今フレームで適用する移動量として格納
    DirectX::XMStoreFloat3(&m_AppliedMovement, v_knock_back_vec);

    // 次フレームのためにノックバックベクトルを更新
    DirectX::XMStoreFloat3(&m_pOwner->m_KnockBackVec, v_knock_back_vec);

    // --- ステート遷移チェック ---

    // Y座標が0.f以下になったらIdleステートに戻す（着地判定）
    if (m_pOwner->GetPositionY() < 0.f)
    {
        // Y位置を地面に固定 (めり込み防止)
        m_pOwner->SetPositionY(0.f); 

        m_pOwner->ChangeState(PlayerState::eID::Idle);
    }
}

void KnockBack::LateUpdate()
{
    // 座標移動.
    m_pOwner->AddPosition(m_AppliedMovement);

    System::LateUpdate();
}

void KnockBack::Draw()
{
}

void KnockBack::Exit()
{
}

} // PlayerState.
