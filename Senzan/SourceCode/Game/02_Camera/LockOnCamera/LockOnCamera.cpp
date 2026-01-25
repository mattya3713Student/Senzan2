#include "LockOnCamera.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game/04_Time/Time.h"
#include "System/Singleton/ImGui/CImGuiManager.h"
#include "System/Utility/FileManager/FileManager.h"

LockOnCamera::LockOnCamera(const Player& player, const Boss& target)
    : m_rPlayer(player)
    , m_rTarget(target)
    , m_FollowSpeed(3.15f)
    , m_HeightOffset(5.0f)
    , m_LookOffset(4.5f)
    , m_LookLerp(0.7f)
    , m_CameraMode(eCameraMode::Normal)
    , m_ParryTime(0.0f)
{
    m_Distance = 15.0f; // カメラからプレイヤーまでの距離.
    try { LoadSettings(); } catch (...) {}
}

void LockOnCamera::Update()
{
    float dt = Time::GetInstance().GetDeltaTime();

#if _DEBUG
	if (ImGui::Begin(IMGUI_JP("LockOnCamera Debug")))
	{
		ImGui::DragFloat(IMGUI_JP("距離"), &m_Distance, 0.1f, 1.0f, 50.0f);
		ImGui::DragFloat(IMGUI_JP("追従速度"), &m_FollowSpeed, 0.05f, 0.1f, 10.0f);
		ImGui::DragFloat(IMGUI_JP("高さオフセット"), &m_HeightOffset, 0.1f, 0.0f, 20.0f);
		ImGui::DragFloat(IMGUI_JP("注視点高さ"), &m_LookOffset, 0.1f, 0.0f, 20.0f);
		ImGui::DragFloat(IMGUI_JP("注視点補間"), &m_LookLerp, 0.01f, 0.0f, 1.0f);

        ImGui::Separator();
        ImGui::Text(IMGUI_JP("パリィカメラ設定"));
        ImGui::DragFloat(IMGUI_JP("パリィ演出時間"), &m_ParryDuration, 0.05f, 0.1f, 3.0f);
        ImGui::DragFloat(IMGUI_JP("パリィ時高さ"), &m_ParryHeightOffset, 0.1f, -5.0f, 10.0f);
        ImGui::DragFloat(IMGUI_JP("パリィ時注視点高さ"), &m_ParryLookOffset, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat(IMGUI_JP("パリィ時距離"), &m_ParryDistance, 0.1f, 1.0f, 20.0f);
        
        if (ImGui::Button(IMGUI_JP("パリィカメラテスト"))) {
            StartParryCamera();
        }

		if (ImGui::Button(IMGUI_JP("Load"))) {
			try { LoadSettings(); } catch (...) {}
		}
		ImGui::SameLine();
		if (ImGui::Button(IMGUI_JP("Save"))) {
			try { SaveSettings(); } catch (...) {}
		}
		ImGui::End();
	}
#endif

    // カメラモードに応じて更新処理を分岐.
    switch (m_CameraMode)
    {
    case eCameraMode::Normal:
        UpdateNormalCamera(dt);
        break;
    case eCameraMode::Parry:
        UpdateParryCamera(dt);
        break;
    case eCameraMode::Special:
        UpdateSpecialCamera(dt);
        break;
    }

	ViewAndProjectionUpdate();
}

void LockOnCamera::UpdateNormalCamera(float dt)
{
	const auto& player = m_rPlayer.get();
	const auto& target = m_rTarget.get();

	// 1. 各座標を取得.
	DirectX::XMVECTOR vPlayerPos = DirectX::XMLoadFloat3(&player.GetPosition());
	DirectX::XMVECTOR vBossPos = DirectX::XMLoadFloat3(&target.GetPosition());

	// 2. プレイヤーとボスの距離を算出（高さ調整や安定化に使用）.
	DirectX::XMVECTOR vDiffPB = DirectX::XMVectorSubtract(vPlayerPos, vBossPos);
	float distToBoss = DirectX::XMVectorGetX(DirectX::XMVector3Length(vDiffPB));

	// --- 改善点1: 至近距離でのガクつき防止 ---
	// 距離が近すぎるとき、方向ベクトルの計算を安定させる.
	DirectX::XMVECTOR vToPlayerDir;
	if (distToBoss < 0.1f) {
		DirectX::XMFLOAT3 playerForward = player.GetTransform()->GetForward();
		// XMFLOAT3をXMVECTORにロードしてから計算.
		vToPlayerDir = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&playerForward), -1.0f);
	}
	else {
		vToPlayerDir = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(vDiffPB, 0.0f));
	}

	// --- 改善点2: 注視点を「ボスとプレイヤーの間」に設定 ---
	// これによりボスが画面中央に固定されすぎず、自分も映りやすくなる.
	DirectX::XMVECTOR vMidPoint = DirectX::XMVectorLerp(vPlayerPos, vBossPos, m_LookLerp);
	vMidPoint = DirectX::XMVectorAdd(vMidPoint, DirectX::XMVectorSet(0.0f, m_LookOffset, 0.0f, 0.0f));

	// --- 改善点3: 近接時のカメラ高補正 ---
	// 近づくほど少しカメラを高くし、見下ろす角度を強くすることでキャラ被りを防ぐ.
	float heightScaling = (1.0f / (distToBoss + 1.0f)) * 2.0f; // 近いほど加算値が増える.
	float finalHeight = m_HeightOffset + heightScaling;

	// 3. 理想のカメラ位置を計算.
	DirectX::XMVECTOR vIdealPos = DirectX::XMVectorAdd(vPlayerPos, DirectX::XMVectorScale(vToPlayerDir, m_Distance));
	vIdealPos = DirectX::XMVectorAdd(vIdealPos, DirectX::XMVectorSet(0.0f, finalHeight, 0.0f, 0.0f));

	// 4. 現在の位置から理想の位置へ線形補間 (Lerp).
	DirectX::XMVECTOR vCurrentPos = DirectX::XMLoadFloat3(&m_spTransform.Position);
	float lerpFactor = std::clamp(m_FollowSpeed * dt, 0.0f, 1.0f);
	DirectX::XMVECTOR vNextPos = DirectX::XMVectorLerp(vCurrentPos, vIdealPos, lerpFactor);
	DirectX::XMStoreFloat3(&m_spTransform.Position, vNextPos);

	// 5. 注視点の適用（デッドゾーン計算を入れる場合はここで行う）.
	DirectX::XMStoreFloat3(&m_LookPos, vMidPoint);

	// --- 以下、前方ベクトル・右方ベクトル・行列更新（共通） ---
	DirectX::XMVECTOR vForward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vMidPoint, vNextPos));
	DirectX::XMStoreFloat3(&m_ForwardVec, vForward);

	DirectX::XMVECTOR vUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vForward));
	DirectX::XMStoreFloat3(&m_RightVec, vRight);

	DirectX::XMMATRIX lookAtMat = DirectX::XMMatrixLookToLH(vNextPos, vForward, vUp);
	DirectX::XMVECTOR v_Quaternion = DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixTranspose(lookAtMat));
	DirectX::XMStoreFloat4(&m_spTransform.Quaternion, v_Quaternion);
}

void LockOnCamera::UpdateParryCamera(float dt)
{
    m_ParryTime += dt;

    // 演出終了判定.
    if (m_ParryTime >= m_ParryDuration)
    {
        EndParryCamera();
        UpdateNormalCamera(dt);
        return;
    }

    const auto& player = m_rPlayer.get();
    const auto& target = m_rTarget.get();

    // イージング進行率（0.0 ~ 1.0）.
    float t = m_ParryTime / m_ParryDuration;
    
    // 前半は潜り込み、後半は戻り.
    float easedT;
    if (t < 0.5f)
    {
        // 前半: 通常→パリィ位置へ（EaseOutQuad: 1 - (1-t)^2）.
        float halfT = t * 2.0f;
        easedT = 1.0f - (1.0f - halfT) * (1.0f - halfT);
    }
    else if (t < 0.6f)
    {
        float halfT = 0.5f * 2.0f;
        easedT = 1.0f - (1.0f - halfT) * (1.0f - halfT);
        // 一瞬止まる.
    }
    else 
    {
        // 後半: パリィ位置→通常へ（EaseInQuad: t^2）.
        float halfT = (t - 0.5f) * 2.0f;
        easedT = 1.0f - (halfT * halfT);
    }

    // 1. 各座標を取得.
    DirectX::XMVECTOR vPlayerPos = DirectX::XMLoadFloat3(&player.GetPosition());
    DirectX::XMVECTOR vBossPos = DirectX::XMLoadFloat3(&target.GetPosition());

    // 2. プレイヤーとボスの距離を算出.
    DirectX::XMVECTOR vDiffPB = DirectX::XMVectorSubtract(vPlayerPos, vBossPos);
    float distToBoss = DirectX::XMVectorGetX(DirectX::XMVector3Length(vDiffPB));

    DirectX::XMVECTOR vToPlayerDir;
    if (distToBoss < 0.1f) {
        DirectX::XMFLOAT3 playerForward = player.GetTransform()->GetForward();
        vToPlayerDir = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&playerForward), -1.0f);
    }
    else {
        vToPlayerDir = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(vDiffPB, 0.0f));
    }

    // 通常パラメータとパリィパラメータを補間.
    float currentHeight = m_HeightOffset + (m_ParryHeightOffset - m_HeightOffset) * easedT;
    float currentLookOffset = m_LookOffset + (m_ParryLookOffset - m_LookOffset) * easedT;
    float currentDistance = m_Distance + (m_ParryDistance - m_Distance) * easedT;

    // 注視点を計算（パリィ時はボス寄りにして見上げる）.
    float parryLookLerp = m_LookLerp + (0.9f - m_LookLerp) * easedT;
    DirectX::XMVECTOR vMidPoint = DirectX::XMVectorLerp(vPlayerPos, vBossPos, parryLookLerp);
    vMidPoint = DirectX::XMVectorAdd(vMidPoint, DirectX::XMVectorSet(0.0f, currentLookOffset, 0.0f, 0.0f));

    // 3. 理想のカメラ位置を計算（低い位置から）.
    DirectX::XMVECTOR vIdealPos = DirectX::XMVectorAdd(vPlayerPos, DirectX::XMVectorScale(vToPlayerDir, currentDistance));
    vIdealPos = DirectX::XMVectorAdd(vIdealPos, DirectX::XMVectorSet(0.0f, currentHeight, 0.0f, 0.0f));

    // 4. カメラ位置を更新（パリィ中は高速追従）.
    DirectX::XMVECTOR vCurrentPos = DirectX::XMLoadFloat3(&m_spTransform.Position);
    float lerpFactor = std::clamp(m_FollowSpeed * 3.0f * dt, 0.0f, 1.0f);
    DirectX::XMVECTOR vNextPos = DirectX::XMVectorLerp(vCurrentPos, vIdealPos, lerpFactor);
    DirectX::XMStoreFloat3(&m_spTransform.Position, vNextPos);

    // 5. 注視点の適用.
    DirectX::XMStoreFloat3(&m_LookPos, vMidPoint);

    // 前方ベクトル・右方ベクトル・行列更新.
    DirectX::XMVECTOR vForward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vMidPoint, vNextPos));
    DirectX::XMStoreFloat3(&m_ForwardVec, vForward);

    DirectX::XMVECTOR vUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vForward));
    DirectX::XMStoreFloat3(&m_RightVec, vRight);

    DirectX::XMMATRIX lookAtMat = DirectX::XMMatrixLookToLH(vNextPos, vForward, vUp);
    DirectX::XMVECTOR v_Quaternion = DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixTranspose(lookAtMat));
    DirectX::XMStoreFloat4(&m_spTransform.Quaternion, v_Quaternion);
}

void LockOnCamera::StartParryCamera()
{
    m_CameraMode = eCameraMode::Parry;
    m_ParryTime = 0.0f;
}

void LockOnCamera::EndParryCamera()
{
    m_CameraMode = eCameraMode::Normal;
    m_ParryTime = 0.0f;
}

void LockOnCamera::StartSpecialCamera()
{
    m_CameraMode = eCameraMode::Special;
}

void LockOnCamera::EndSpecialCamera()
{
    m_CameraMode = eCameraMode::Normal;
}

void LockOnCamera::UpdateSpecialCamera(float dt)
{
    const auto& player = m_rPlayer.get();
    const auto& target = m_rTarget.get();

    // 1. 各座標を取得.
    DirectX::XMVECTOR vPlayerPos = DirectX::XMLoadFloat3(&player.GetPosition());
    DirectX::XMVECTOR vBossPos = DirectX::XMLoadFloat3(&target.GetPosition());

    // 2. プレイヤーとボスの距離を算出.
    DirectX::XMVECTOR vDiffPB = DirectX::XMVectorSubtract(vPlayerPos, vBossPos);
    float distToBoss = DirectX::XMVectorGetX(DirectX::XMVector3Length(vDiffPB));

    DirectX::XMVECTOR vToPlayerDir;
    if (distToBoss < 0.1f) {
        DirectX::XMFLOAT3 playerForward = player.GetTransform()->GetForward();
        vToPlayerDir = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&playerForward), -1.0f);
    }
    else {
        vToPlayerDir = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(vDiffPB, 0.0f));
    }

    // 注視点をボスに完全ロック.
    DirectX::XMVECTOR vLookAt = DirectX::XMVectorLerp(vPlayerPos, vBossPos, m_SpecialLookLerp);
    vLookAt = DirectX::XMVectorAdd(vLookAt, DirectX::XMVectorSet(0.0f, m_LookOffset, 0.0f, 0.0f));

    // 3. カメラ位置を計算（通常と同じ距離・高さ）.
    float heightScaling = (1.0f / (distToBoss + 1.0f)) * 2.0f;
    float finalHeight = m_HeightOffset + heightScaling;

    DirectX::XMVECTOR vIdealPos = DirectX::XMVectorAdd(vPlayerPos, DirectX::XMVectorScale(vToPlayerDir, m_Distance));
    vIdealPos = DirectX::XMVectorAdd(vIdealPos, DirectX::XMVectorSet(0.0f, finalHeight, 0.0f, 0.0f));

    // 4. カメラ位置を更新.
    DirectX::XMVECTOR vCurrentPos = DirectX::XMLoadFloat3(&m_spTransform.Position);
    float lerpFactor = std::clamp(m_FollowSpeed * dt, 0.0f, 1.0f);
    DirectX::XMVECTOR vNextPos = DirectX::XMVectorLerp(vCurrentPos, vIdealPos, lerpFactor);
    DirectX::XMStoreFloat3(&m_spTransform.Position, vNextPos);

    // 5. 注視点の適用（ボスをロック）.
    DirectX::XMStoreFloat3(&m_LookPos, vLookAt);

    // 前方ベクトル・右方ベクトル・行列更新.
    DirectX::XMVECTOR vForward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vLookAt, vNextPos));
    DirectX::XMStoreFloat3(&m_ForwardVec, vForward);

    DirectX::XMVECTOR vUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vForward));
    DirectX::XMStoreFloat3(&m_RightVec, vRight);

    DirectX::XMMATRIX lookAtMat = DirectX::XMMatrixLookToLH(vNextPos, vForward, vUp);
    DirectX::XMVECTOR v_Quaternion = DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixTranspose(lookAtMat));
    DirectX::XMStoreFloat4(&m_spTransform.Quaternion, v_Quaternion);
}

void LockOnCamera::SaveSettings() const
{
    json j;
    j["Distance"] = m_Distance;
    j["FollowSpeed"] = m_FollowSpeed;
    j["HeightOffset"] = m_HeightOffset;
    j["LookOffset"] = m_LookOffset;
    j["LookLerp"] = m_LookLerp;

    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        auto dir = std::filesystem::current_path() / "Data" / "Json" / "Camera";
        std::filesystem::create_directories(dir);
        filePath = dir / filePath;
    }
    FileManager::JsonSave(filePath, j);
}

void LockOnCamera::LoadSettings()
{
    auto filePath = GetSettingsFileName();
    if (!filePath.is_absolute()) {
        filePath = std::filesystem::current_path() / "Data" / "Json" / "Camera" / filePath;
    }
    if (!std::filesystem::exists(filePath)) return;
    json j = FileManager::JsonLoad(filePath);
    if (j.contains("Distance")) m_Distance = j["Distance"].get<float>();
    if (j.contains("FollowSpeed")) m_FollowSpeed = j["FollowSpeed"].get<float>();
    if (j.contains("HeightOffset")) m_HeightOffset = j["HeightOffset"].get<float>();
    if (j.contains("LookOffset")) m_LookOffset = j["LookOffset"].get<float>();
    if (j.contains("LookLerp")) m_LookLerp = j["LookLerp"].get<float>();
}
