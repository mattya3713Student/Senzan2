#include "Parry.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
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


    m_IsParrySuccessful = false;
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
    if (!m_IsParrySuccessful)
    {
        m_ElapsedTime += Time::GetInstance().GetDeltaTime();
        if (!m_IsPaused && m_ElapsedTime >= m_PauseThreshold)
        {
            m_pOwner->SetAnimSpeed(0.0f);
            m_IsPaused = true;
        }
        // パリィが成功しなかった場合、最大待機時間を超えたらステートを抜ける
        if (m_ElapsedTime >= m_MaxWaitTime)
        {
            if(!m_IsAnimEndStart)
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

    // プレイヤーの成功フラグを監視して、成功したら再生を再開する
    if (m_pOwner->IsParry() && !m_IsParrySuccessful)
    {
        m_IsParrySuccessful = true;
        if (m_IsPaused)
        {
            m_pOwner->SetAnimSpeed(1.0f);
            m_IsPaused = false;
        }
    }

    // アニメーション終了時の処理
    if (m_pOwner->IsAnimEnd(Player::eAnim::Parry)) {
        if (!m_IsParrySuccessful) {
            m_pOwner->ChangeState(PlayerState::eID::Idle); // 失敗時も Idle に遷移
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
    m_IsParrySuccessful = false;

    // Exit で必ず再生速度を復帰
    m_pOwner->SetAnimSpeed(1.0f);
    m_IsPaused = false;
    m_IsAnimEndStart = false;
    m_ElapsedTime = 0.0f;

    m_pOwner->SetDamageColliderActive(true);
    m_pOwner->SetParryColliderActive(false);
}
} // PlayerState.
