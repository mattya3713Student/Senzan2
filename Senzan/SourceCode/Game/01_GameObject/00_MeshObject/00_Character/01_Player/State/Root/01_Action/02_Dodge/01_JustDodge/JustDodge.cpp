#include "JustDodge.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"

#include "Game/05_InputDevice/VirtualPad.h"
#include "System/Singleton/CameraManager/CameraManager.h"

#include "../Dodge.h"


static constexpr double JUSTDODGE_ANIM_SPEED = 2.00;

namespace PlayerState {
JustDodge::JustDodge(Player* owner)
	: Dodge(owner)
{
}
JustDodge::~JustDodge()
{
}

// ID�̎擾.
constexpr PlayerState::eID JustDodge::GetStateID() const
{
	return PlayerState::eID::JustDodge;
}

void JustDodge::Enter()
{
    Dodge::Enter();

	m_Distance = 250.f;
	m_MaxTime = 1.8f;

    Time::GetInstance().SetWorldTimeScale(0.1f, 1.5f);

    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimSpeed(JUSTDODGE_ANIM_SPEED);
    m_pOwner->ChangeAnim(Player::eAnim::Dodge);
    
    // ジャスト回避成功時に強化攻撃モードを有効化
    m_pOwner->m_IsEnhancedAttackMode = true;
}

void JustDodge::Update()
{
    Dodge::Update();

	// �f���^�^�C���̌v�Z.
	double animSpeed = MyMath::IsNearlyEqual(m_pOwner->m_AnimSpeed, 0.0) ? 0.0 : m_pOwner->m_AnimSpeed;
	float deltaTime = static_cast<float>(animSpeed) * m_pOwner->GetDelta();

	// ���ԂƋ����̍X�V.
	m_currentTime += deltaTime;
    // ���͂��擾.
    DirectX::XMFLOAT2 input_vec = VirtualPad::GetInstance().GetAxisInput(VirtualPad::eGameAxisAction::Move);

    // ���͂��Ȃ���Αҋ@�֑J��.
    if (MyMath::IsVector2NearlyZero(input_vec, 0.f)) {
        m_pOwner->ChangeState(PlayerState::eID::Idle);
        return;
    }

    // ���ꂼ��x�N�g�����擾.
    DirectX::XMFLOAT3 camera_forward_vec = CameraManager::GetInstance().GetCurrentCamera()->GetForwardVec();
    DirectX::XMFLOAT3 camera_right_vec = CameraManager::GetInstance().GetCurrentCamera()->GetRightVec();
    DirectX::XMVECTOR v_camera_forward = DirectX::XMLoadFloat3(&camera_forward_vec);
    DirectX::XMVECTOR v_camera_right = DirectX::XMLoadFloat3(&camera_right_vec);

    // Y���W���폜.
    v_camera_forward = DirectX::XMVectorSetY(v_camera_forward, 0.0f);
    v_camera_right = DirectX::XMVectorSetY(v_camera_right, 0.0f);

    // ���K��.
    v_camera_forward = DirectX::XMVector3Normalize(v_camera_forward);
    v_camera_right = DirectX::XMVector3Normalize(v_camera_right);

    // ���͂ƃx�N�g��������.
    DirectX::XMVECTOR v_movement_z = DirectX::XMVectorScale(v_camera_forward, input_vec.y);
    DirectX::XMVECTOR v_movement_x = DirectX::XMVectorScale(v_camera_right, input_vec.x);

    // �ŏI�I�Ȉړ��x�N�g��������.
    DirectX::XMVECTOR v_final_move = DirectX::XMVectorAdd(v_movement_z, v_movement_x);

    // ���x * delta * ���Ԏړx.
    float speed_and_delta = m_pOwner->m_RunMoveSpeed * m_pOwner->GetDelta();
    v_final_move = DirectX::XMVectorScale(v_final_move, speed_and_delta);
    DirectX::XMFLOAT3 final_move;
    DirectX::XMStoreFloat3(&final_move, v_final_move);

    // �ړ����Z.
    m_pOwner->m_MoveVec.x = final_move.x;
    m_pOwner->m_MoveVec.y = final_move.z;

	// �������.
	if (m_currentTime >= m_MaxTime)
	{
		m_pOwner->ChangeState(PlayerState::eID::Idle);
	}
}

void JustDodge::LateUpdate()
{
    Dodge::LateUpdate();

    m_pOwner->AddPosition(m_pOwner->m_MoveVec.x * 0.5f, 0.f, m_pOwner->m_MoveVec.y * 0.5f);
}

void JustDodge::Draw()
{
    Dodge::Draw();
}

void JustDodge::Exit()
{
	Dodge::Exit();
}

} // PlayerState.