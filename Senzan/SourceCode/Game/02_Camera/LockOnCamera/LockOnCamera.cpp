#include "LockOnCamera.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/01_Player/Player.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/Boss.h"

static constexpr float FOLLOW_SPEED = 0.004f;
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

	// 1. 各座標を取得.
	DirectX::XMVECTOR vPlayerPos = DirectX::XMLoadFloat3(&player.GetPosition());
	DirectX::XMVECTOR vBossPos = DirectX::XMLoadFloat3(&target.GetPosition());

	// 2. プレイヤーとボスの距離を算出（高さ調整や安定化に使用）.
	DirectX::XMVECTOR vDiffPB = DirectX::XMVectorSubtract(vPlayerPos, vBossPos);
	float distToBoss = DirectX::XMVectorGetX(DirectX::XMVector3Length(vDiffPB));

	// --- 改善点1：至近距離でのガクつき防止 ---
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

	// --- 改善点2：注視点を「ボスとプレイヤーの間」に設定 ---
	// これによりボスが画面中央に固定されすぎず、自分も映りやすくなる.
	// 割合 (0.7f) を変えると、ボス寄りかプレイヤー寄りか調整可能.
	float lookLerp = 0.7f;
	DirectX::XMVECTOR vMidPoint = DirectX::XMVectorLerp(vPlayerPos, vBossPos, lookLerp);
	vMidPoint = DirectX::XMVectorAdd(vMidPoint, DirectX::XMVectorSet(0.0f, LOOK_OFFSET, 0.0f, 0.0f));

	// --- 改善点3：近接時のカメラ高補正 ---
	// 近づくほど少しカメラを高くし、見下ろす角度を強くすることでキャラ被りを防ぐ.
	float heightScaling = (1.0f / (distToBoss + 1.0f)) * 2.0f; // 近いほど加算値が増える.
	float finalHeight = HEIGHT_OFFSET + heightScaling;

	// 3. 理想のカメラ位置を計算.
	DirectX::XMVECTOR vIdealPos = DirectX::XMVectorAdd(vPlayerPos, DirectX::XMVectorScale(vToPlayerDir, m_Distance));
	vIdealPos = DirectX::XMVectorAdd(vIdealPos, DirectX::XMVectorSet(0.0f, finalHeight, 0.0f, 0.0f));

	// 4. 現在の位置から理想の位置へ線形補間 (Lerp).
	DirectX::XMVECTOR vCurrentPos = DirectX::XMLoadFloat3(&m_spTransform.Position);
	DirectX::XMVECTOR vNextPos = DirectX::XMVectorLerp(vCurrentPos, vIdealPos, FOLLOW_SPEED);
	DirectX::XMStoreFloat3(&m_spTransform.Position, vNextPos);

	// 5. 注視点の適用（デッドゾーン計算を入れる場合はここで行う）.
	// 前回のコードの「遊び（円）」を vMidPoint に対して適用するとさらに良くなります.
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

	ViewAndProjectionUpdate();
}