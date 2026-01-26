#include "ParryManager.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/PostEffectManager/PostEffectManager.h"
#include "Game/02_Camera/LockOnCamera/LockOnCamera.h"
#include "System/Singleton/ResourceManager/SoundManager/SoundManager.h"

ParryManager::ParryManager()
	: m_pPlayer(nullptr)
	, m_pBoss(nullptr)
{
}

ParryManager::~ParryManager()
{
	Clear();
}

void ParryManager::Initialize(Player* player, Boss* boss)
{
	m_pPlayer = player;
	m_pBoss = boss;
}

void ParryManager::Clear()
{
	m_pPlayer = nullptr;
	m_pBoss = nullptr;
}

void ParryManager::OnParrySuccess(bool withDelay)
{
    // カメラ演出（シェイク + パリィカメラ）.
    CameraManager::GetInstance().ShakeCamera(0.15f, 0.3f);
    CameraManager::GetInstance().StartParryCamera();

    // 円状グレースケールエフェクト開始
    PostEffectManager::GetInstance().StartCircleGrayEffect(0.0f, 0.016f * 5, 0.016f * 2);
    // 一時的にワールド時間を止める（短時間）
    Time::GetInstance().SetWorldTimeScale(0.01f, 0.016f * 5, true);
    // BGMをフェードで少し下げて自動で戻す
    // 下げ時間:0.2s, 保持:1.8s, 戻す:0.4s -> 合計約2.4s
    SoundManager::LowerCurrentBGMVolumeTemporarily(6500, 0.2f, 0.4f, 0.4f);


    // Bossにパリィ被弾を通知（パリィステートへ遷移）。
    // withDelay フラグは Boss 側で解釈して遅延挙動を組み込む。
    if (m_pBoss)
    {
        m_pBoss->OnParried(withDelay);
        m_pBoss->Hit(50.f);
    }
}

void ParryManager::StartJustDodge(float TimeScale)
{
    if (m_pBoss)
    {
        JustCancelAttackCollider();
        m_pBoss->OffAttackCollider();
        m_pBoss->SetTimeScale(TimeScale);
        m_pBoss->SetLockOnPlayer(false);
    }
}

void ParryManager::EndJustDodge()
{
    if (m_pBoss)
    {
        m_pBoss->OffAttackCollider();
        m_pBoss->Hit(50.f);
        m_pBoss->SetTimeScale(-1.f);
        m_pBoss->SetLockOnPlayer(true);
    }
}

void ParryManager::DisableBossAttackColliders()
{
    if (m_pBoss)
    {
        m_pBoss->OffAttackCollider();
    }
}

bool ParryManager::IsParryCameraFinished() const
{
	auto camera = CameraManager::GetInstance().GetCurrentCamera();
	if (!camera) return true;

	auto lockOnCamera = std::dynamic_pointer_cast<LockOnCamera>(camera);
	if (lockOnCamera)
	{
		return !lockOnCamera->IsParryCameraActive();
	}
	return true;
}

void ParryManager::JustCancelAttackCollider()
{
    if (m_pBoss)
    {
        m_pBoss->SetNextAttackCansel();
    }
}
