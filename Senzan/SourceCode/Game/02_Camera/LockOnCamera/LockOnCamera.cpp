#include "LockOnCamera.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"
#include "Game/04_Time/Time.h"

static constexpr float FOLLOW_SPEED = 5.0f;
static constexpr float HEIGHT_OFFSET = 5.0f;
static constexpr float LOOK_OFFSET = 4.5f;

LockOnCamera::LockOnCamera(const Player& player, const Boss& target)
    : m_rPlayer(player)
    , m_rTarget(target)
    , m_FollowSpeed(0.f)
    , m_HeightOffset(0.f)
    , m_LookOffset(0.f)
{
    m_Distance = 15.0f; // カメラからプレイヤーまでの距離.
}

void LockOnCamera::Update()
{
	const auto& player = m_rPlayer.get();
	const auto& target = m_rTarget.get();

	// 座標を取得.
	DirectX::XMVECTOR v_player_pos = DirectX::XMLoadFloat3(&player.GetPosition());
	DirectX::XMVECTOR v_boss_pos = DirectX::XMLoadFloat3(&target.GetPosition());

	// プレイヤーとボスの距離.
	DirectX::XMVECTOR v_diff_distance = DirectX::XMVectorSubtract(v_player_pos, v_boss_pos);
	float diff_distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(v_diff_distance));

	// 距離が近すぎるとき、方向ベクトルの計算を安定させる.
	DirectX::XMVECTOR v_to_player_direction;
	if (diff_distance < 0.1f) {
		DirectX::XMFLOAT3 player_forward = player.GetTransform()->GetForward();
        v_to_player_direction = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&player_forward), -1.0f);
	}
	else {
        v_to_player_direction = DirectX::XMVector3Normalize(DirectX::XMVectorSetY(v_diff_distance, 0.0f));
	}

    // 注視点を ボスとプレイヤーの間 に設定.
    // MEMO : look_lerpをconstexprに
	float look_lerp = 0.7f;
	DirectX::XMVECTOR v_mid_point = DirectX::XMVectorLerp(v_player_pos, v_boss_pos, look_lerp);
    v_mid_point = DirectX::XMVectorAdd(v_mid_point, DirectX::XMVectorSet(0.0f, LOOK_OFFSET, 0.0f, 0.0f));

	// 近接時のカメラ高補正.
	float height_scaling = (1.0f / (diff_distance + 1.0f)) * 2.0f;
	float final_height = HEIGHT_OFFSET + height_scaling;

	// 理想のカメラ位置.
	DirectX::XMVECTOR v_ideal_pos = DirectX::XMVectorAdd(v_player_pos, DirectX::XMVectorScale(v_to_player_direction, m_Distance));
    v_ideal_pos = DirectX::XMVectorAdd(v_ideal_pos, DirectX::XMVectorSet(0.0f, final_height, 0.0f, 0.0f));

	// 現在の位置から理想の位置へ線形補間.
	DirectX::XMVECTOR v_current_pos = DirectX::XMLoadFloat3(&m_spTransform.Position);
	float delta_time = Time::GetInstance().GetDeltaTime(); 
	float lerpFactor = std::clamp(FOLLOW_SPEED * delta_time, 0.0f, 1.0f);
	DirectX::XMVECTOR vNextPos = DirectX::XMVectorLerp(v_current_pos, v_ideal_pos, lerpFactor);
	DirectX::XMStoreFloat3(&m_spTransform.Position, vNextPos);

	// 注視点の適用.
    // TODO : デッドゾーン計算.
	DirectX::XMStoreFloat3(&m_LookPos, v_mid_point);

	// ベクトル,行列更新.
	DirectX::XMVECTOR vForward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(v_mid_point, vNextPos));
	DirectX::XMStoreFloat3(&m_ForwardVec, vForward);

	DirectX::XMVECTOR vUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vForward));
	DirectX::XMStoreFloat3(&m_RightVec, vRight);

	DirectX::XMMATRIX lookAtMat = DirectX::XMMatrixLookToLH(vNextPos, vForward, vUp);
	DirectX::XMVECTOR v_Quaternion = DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixTranspose(lookAtMat));
	DirectX::XMStoreFloat4(&m_spTransform.Quaternion, v_Quaternion);

	ViewAndProjectionUpdate();
}
