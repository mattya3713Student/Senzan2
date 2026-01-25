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
    PostEffectManager::GetInstance().StartCircleGrayEffect(0.3f, 0.4f, 0.5f);
    // BGMをフェードで少し下げて自動で戻す
    // 下げ時間:0.2s, 保持:1.8s, 戻す:0.4s -> 合計約2.4s
    SoundManager::LowerCurrentBGMVolumeTemporarily(6500, 0.2f, 0.4f, 0.4f);


    // Bossにパリィ被弾を通知（パリィステートへ遷移）。
    // withDelay フラグは Boss 側で解釈して遅延挙動を組み込む。
    if (m_pBoss)
    {
        m_pBoss->OnParried(withDelay);
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
