#include "ParryManager.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "Game/02_Camera/LockOnCamera/LockOnCamera.h"

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

void ParryManager::OnParrySuccess(bool withDelay, float delaySeconds)
{
    // カメラ演出（シェイク + パリィカメラ）.
    CameraManager::GetInstance().ShakeCamera(0.15f, 0.3f);
    CameraManager::GetInstance().StartParryCamera();

    // Bossにパリィ被弾を通知（パリィステートへ遷移）。
    // withDelay フラグは Boss 側で解釈して遅延挙動を組み込む。
    if (m_pBoss)
    {
        m_pBoss->OnParried(withDelay, delaySeconds);
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
