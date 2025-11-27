#include "KnockBack.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"

#include "Game/04_Time/Time.h"

// 減衰率.
static constexpr float DAMPING_FACTOR = 0.999f;
static constexpr float GRAVITY_ACCELERATION = -5.f;

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
    DirectX::XMVECTOR v_knockback_vec_xz = DirectX::XMLoadFloat3(&m_pOwner->m_KnockBackVec);

    // Y成分を0にし、水平方向の初速を計算 (衝撃の強さ m_KnockBackPower を適用)
    v_knockback_vec_xz = DirectX::XMVectorSetY(v_knockback_vec_xz, 0.0f);
    v_knockback_vec_xz = DirectX::XMVectorScale(v_knockback_vec_xz, m_pOwner->m_KnockBackPower);
    
    // ジャンプ力. 
    const float INITIAL_JUMP_SPEED = 8.0f;

    // 水平速度と垂直速度を組み合わせる
    DirectX::XMVECTOR v_final_velocity = DirectX::XMVectorSelect(v_knockback_vec_xz,
        DirectX::XMVectorSet(0.0f, INITIAL_JUMP_SPEED, 0.0f, 0.0f),
        DirectX::XMVectorSelectControl(0, 1, 0, 0));

    // 初速を速度変数に格納
    DirectX::XMStoreFloat3(&m_pOwner->m_KnockBackVec, v_final_velocity);

    //// 吹っ飛びベクトルを計算.
    //DirectX::XMVECTOR v_knock_back_vec = DirectX::XMLoadFloat3(&m_pOwner->m_KnockBackVec);
    //DirectX::XMVectorScale(v_knock_back_vec, m_pOwner->m_KnockBackPower);
    //v_knock_back_vec = DirectX::XMVectorSetY(v_knock_back_vec, 3.f);
    //DirectX::XMStoreFloat3(&m_pOwner->m_KnockBackVec, v_knock_back_vec);
}

void KnockBack::Update()
{ 
    DirectX::XMVECTOR v_knock_back_speed = DirectX::XMLoadFloat3(&m_pOwner->m_KnockBackVec); 
    float deltaTime = Time::GetDeltaTime();

    // 重力.
    DirectX::XMVECTOR v_gravity = DirectX::XMVectorSet(0.0f, GRAVITY_ACCELERATION * deltaTime, 0.0f, 0.0f);
    v_knock_back_speed = DirectX::XMVectorAdd(v_knock_back_speed, v_gravity);

    v_knock_back_speed = DirectX::XMVectorScale(v_knock_back_speed, DAMPING_FACTOR);
    DirectX::XMVECTOR v_movement_distance = DirectX::XMVectorScale(v_knock_back_speed, deltaTime);

    // 今フレームの移動距離.
    DirectX::XMStoreFloat3(&m_AppliedMovement, v_movement_distance);

    // 計算後の速度を設定.
    DirectX::XMStoreFloat3(&m_pOwner->m_KnockBackVec, v_knock_back_speed);

    // 着地判定.
    if (m_pOwner->GetPositionY() < 0.f)
    {
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
    int i = 0; 
    ++i;
}

} // PlayerState.
