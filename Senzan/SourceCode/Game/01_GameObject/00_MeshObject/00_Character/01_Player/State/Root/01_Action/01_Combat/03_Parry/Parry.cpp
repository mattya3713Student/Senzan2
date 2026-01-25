#include "Parry.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "System/Singleton/PostEffectManager/PostEffectManager.h"
#include "Game/04_Time/Time.h"

namespace PlayerState {
Parry::Parry(Player* owner)
	: Combat(owner)
{
}
Parry::~Parry()
{
}

// IDの取得.
constexpr PlayerState::eID Parry::GetStateID() const
{
	return PlayerState::eID::Parry;
}

void Parry::Enter()
{
    SoundManager::GetInstance().Play("ReadyParry"); 
    SoundManager::GetInstance().SetVolume("ReadyParry",8500); 

    m_pOwner->SetDamageColliderActive(false);
    m_pOwner->SetParryColliderActive(true);

    m_pOwner->SetTimeScale(1.0f);

    m_ElapsedTime = 0.0f;
    m_IsPaused = false;

    // アニメーション設定
    m_pOwner->SetIsLoop(false);
    m_pOwner->SetAnimTime(0.0f);
    m_pOwner->SetAnimSpeed(1.0f);
    m_pOwner->ChangeAnim(Player::eAnim::Parry);
}

void Parry::Update()
{
    Combat::Update();
    
    // パリィ成功を待つ間、所定時間経過したらアニメ速度を0にして停止させる
    if (m_pOwner->IsParry())
    {
        if (!m_IsFastTime)
        {
            m_pOwner->SetAnimSpeed(2.0f);
            m_IsFastTime = true;
            m_ElapsedTime = 0.f;
        }
        m_ElapsedTime += m_pOwner->GetDelta();

        // アニメーション終了時の処理
        if (m_ElapsedTime >= 1.2f) {
            m_pOwner->ChangeState(PlayerState::eID::Idle); // 失敗時も Idle に遷移
        }
    }
    else
    {

        m_ElapsedTime += m_pOwner->GetDelta();
        if (!m_IsPaused && m_ElapsedTime >= m_PauseThreshold)
        {
            m_pOwner->SetAnimSpeed(0.0f);
            m_IsPaused = true;
        }
        // パリィが成功しなかった場合、最大待機時間を超えたらステートを抜ける
        if (m_ElapsedTime >= m_MaxWaitTime)
        {
            if (!m_IsAnimEndStart)
            {
                m_pOwner->SetAnimTime(2.926);
                m_pOwner->SetAnimSpeed(1.0f);
                m_IsAnimEndStart = true;
            }
            else
            {
                if (m_ElapsedTime >= m_PauseThreshold + (Time::GetInstance().GetDeltaTime() * 10.f))
                {
                    m_pOwner->ChangeState(PlayerState::eID::Idle); // 失敗時も Idle に遷移
                }
            }
            return;
        }
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
    // Exit で必ず再生速度を復帰
    m_pOwner->SetAnimSpeed(1.0f);
    m_IsPaused = false;
    m_IsAnimEndStart = false;
    m_IsFastTime = false;
    m_ElapsedTime = 0.0f;

    m_pOwner->m_IsSuccessParry = true;
    m_pOwner->SetDamageColliderActive(true);
    m_pOwner->SetParryColliderActive(false);
    m_pOwner->SetTimeScale(-1.0f);
}
} // PlayerState.
