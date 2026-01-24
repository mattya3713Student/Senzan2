#include "JustDodge.h"

#include "00_MeshObject/00_Character/01_Player/Player.h"

#include "Game/05_InputDevice/VirtualPad.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/PostEffectManager/PostEffectManager.h"
#include "Resource/Effect/EffectResource.h"

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

// IDの取得.
constexpr PlayerState::eID JustDodge::GetStateID() const
{
	return PlayerState::eID::JustDodge;
}

void JustDodge::Enter()
{
    Dodge::Enter();

	m_Distance = 250.f;
	m_MaxTime = 1.8f;

    Time::GetInstance().SetWorldTimeScale(0.1f, 1.5f, true);

    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimSpeed(JUSTDODGE_ANIM_SPEED);
    m_pOwner->ChangeAnim(Player::eAnim::Dodge);

    PostEffectManager::GetInstance().SetGray(true);

    // ゲージ増加
    m_pOwner->m_CurrentUltValue += 300.0f;

    // UI用エフェクト再生（画面中央に黄色い閃光）
    auto effect = EffectResource::GetResource("Hit2"); // TODO: JustDodgeFlash用エフェクトに差し替え
    if (effect != nullptr)
    {
        float screenX = static_cast<float>(WND_W) * 0.5f;
        float screenY = static_cast<float>(WND_H) * 0.5f;
        m_UIEffectHandle = EffekseerManager::GetInstance().GetManager()->Play(effect, screenX, screenY, 0.0f);
    }
}

void JustDodge::Update()
{
    //Dodge::Update();

	// デルタタイムの計算.
	double animSpeed = MyMath::IsNearlyEqual(m_pOwner->m_AnimSpeed, 0.0) ? 0.0 : m_pOwner->m_AnimSpeed;
	float deltaTime = static_cast<float>(animSpeed) * m_pOwner->GetDelta();

	// 時間と距離の更新.
	m_currentTime += deltaTime;
    // 入力を取得.
    DirectX::XMFLOAT2 input_vec = VirtualPad::GetInstance().GetAxisInput(VirtualPad::eGameAxisAction::Move);

    // それぞれベクトルを取得.
    DirectX::XMFLOAT3 camera_forward_vec = CameraManager::GetInstance().GetCurrentCamera()->GetForwardVec();
    DirectX::XMFLOAT3 camera_right_vec = CameraManager::GetInstance().GetCurrentCamera()->GetRightVec();
    DirectX::XMVECTOR v_camera_forward = DirectX::XMLoadFloat3(&camera_forward_vec);
    DirectX::XMVECTOR v_camera_right = DirectX::XMLoadFloat3(&camera_right_vec);

    // Y座標を削除.
    v_camera_forward = DirectX::XMVectorSetY(v_camera_forward, 0.0f);
    v_camera_right = DirectX::XMVectorSetY(v_camera_right, 0.0f);

    // 正規化.
    v_camera_forward = DirectX::XMVector3Normalize(v_camera_forward);
    v_camera_right = DirectX::XMVector3Normalize(v_camera_right);

    // 入力とベクトルを合成.
    DirectX::XMVECTOR v_movement_z = DirectX::XMVectorScale(v_camera_forward, input_vec.y);
    DirectX::XMVECTOR v_movement_x = DirectX::XMVectorScale(v_camera_right, input_vec.x);

    // 最終的な移動ベクトルを合成.
    DirectX::XMVECTOR v_final_move = DirectX::XMVectorAdd(v_movement_z, v_movement_x);

    // 速度 * delta * 時間尺度.
    float speed_and_delta = m_pOwner->m_RunMoveSpeed * m_pOwner->GetDelta();
    v_final_move = DirectX::XMVectorScale(v_final_move, speed_and_delta);
    DirectX::XMFLOAT3 final_move;
    DirectX::XMStoreFloat3(&final_move, v_final_move);

    // 移動加算.
    m_pOwner->m_MoveVec.x = final_move.x;
    m_pOwner->m_MoveVec.y = final_move.z;


    // UI用エフェクト描画（スクリーン座標系）
    if (m_UIEffectHandle != -1)
    {
        // エフェクトがまだ再生中かチェック
        if (EffekseerManager::GetInstance().GetManager()->Exists(m_UIEffectHandle))
        {
            EffekseerManager::GetInstance().UpdateHandle(m_UIEffectHandle);
            EffekseerManager::GetInstance().RenderHandleUI(m_UIEffectHandle);
        }
        else
        {
            // 再生終了したのでハンドルをリセット
            m_UIEffectHandle = -1;
        }
    }

	// 回避完了.
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
    PostEffectManager::GetInstance().SetGray(false);

    // UIエフェクトが有効なハンドルなら停止して解放する
    if (m_UIEffectHandle != -1)
    {
        // 念のため Effekseer のマネージャー側にも存在確認のうえ停止を試みる（ラッパーが無い場合の保険）
        if (EffekseerManager::GetInstance().GetManager()->Exists(m_UIEffectHandle))
        {
            EffekseerManager::GetInstance().GetManager()->StopEffect(m_UIEffectHandle);
        }

        m_UIEffectHandle = -1;
    }
}

} // PlayerState.

