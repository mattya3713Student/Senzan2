#include "Parry.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "Game/04_Time/Time.h"

namespace PlayerState {

// パリィ成功時のカメラ演出定数
static constexpr float PARRY_SUCCESS_TIME_SCALE = 0.1f;   // スローモーション度合い
static constexpr float PARRY_SUCCESS_DURATION = 0.3f;      // スローモーション継続時間
static constexpr float PARRY_CAMERA_SHAKE_POWER = 2.0f;   // カメラシェイク強度

Parry::Parry(Player* owner)
	: Combat(owner)
{
}
Parry::~Parry()
{
}

// ID�̎擾.
constexpr PlayerState::eID Parry::GetStateID() const
{
	return PlayerState::eID::Parry;
}

void Parry::Enter()
{
    m_pOwner->SetDamageColliderActive(false);
    m_pOwner->SetParryColliderActive(true);


    m_IsParrySuccessful = false;

    // �A�j���[�V�����ݒ�
    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimTime(0.0f);
    m_pOwner->SetAnimSpeed(1.0f);
    m_pOwner->ChangeAnim(Player::eAnim::Parry);
}

void Parry::Update()
{
    Combat::Update();
    
    // パリィ成功判定をチェック
    if (m_pOwner->IsParry() && !m_IsParrySuccessful)
    {
        m_IsParrySuccessful = true;
        
        // カメラ演出：スローモーション + シェイク
        Time::GetInstance().SetWorldTimeScale(PARRY_SUCCESS_TIME_SCALE, PARRY_SUCCESS_DURATION);
        CameraManager::GetInstance().ShakeCamera(PARRY_SUCCESS_DURATION, PARRY_CAMERA_SHAKE_POWER);
        
        // TODO: カメラズーム機能が実装されている場合は、ここでカメラを寄せる処理を追加
        // CameraManager::GetInstance().ZoomIn(targetDistance, duration);
    }
    
    // アニメーション終了時の処理
    if (m_pOwner->IsAnimEnd(Player::eAnim::Parry)) {
        // 成功・失敗に関わらず Idle に遷移
        m_pOwner->ChangeState(PlayerState::eID::Idle);
    }
}

void Parry::LateUpdate()
{
    Combat::LateUpdate();
}

void Parry::Draw()
{
    Combat::Draw();
}

void Parry::Exit()
{
    Combat::Exit();
    m_IsParrySuccessful = false;

    m_pOwner->SetDamageColliderActive(true);
    m_pOwner->SetParryColliderActive(false);
}
} // PlayerState.
