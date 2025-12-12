#include "BossMoveState.h"

#include "Game/04_Time/Time.h"


#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossSpecialState/BossSpecialState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossSlashState/BossSlashState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeState/BossChargeState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossLaserState/BossLaserState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossShoutState/BossShoutState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossStompState/BossStompState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"


#include "System/Singleton/ImGui/CImGuiManager.h"


#include <algorithm>

// コンストラクタ（変更なし）
BossMoveState::BossMoveState(Boss* owner)
	: StateBase<Boss>(owner)
	, m_RotationAngle(0.0f)
	, m_RotationSpeed(0.015f) // 回転速度
	, m_rotationDirection(1.0f)

	, m_pAttack(std::make_unique<BossSpecialState>(owner))
	, m_pSlash(std::make_unique<BossSlashState>(owner))
	, m_pCharge(std::make_unique<BossChargeState>(owner))
	, m_pLaser(std::make_unique<BossLaserState>(owner))
	, m_pShout(std::make_unique<BossShoutState>(owner))
	, m_pStomp(std::make_unique<BossStompState>(owner))
	, m_pThrowing(std::make_unique<BossThrowingState>(owner))
{
}

// デストラクタ（変更なし）
BossMoveState::~BossMoveState()
{
}

// Enter（変更なし）
void BossMoveState::Enter()
{
	m_Timer = 0.0f;
	// 周回中心の初期位置としてボスの現在位置を保存
	m_InitBossPos = m_pOwner->GetPosition();
}


void BossMoveState::Update()
{
	using namespace DirectX;

	float delta = Time::GetInstance().GetDeltaTime();
	m_Timer += delta;

	//----------------------------------------
	//周回軌道中心をプレイヤーに向けて移動させる
	//----------------------------------------

	// プレイヤーの位置を目標点としてロード
	XMFLOAT3 playerPosF = m_pOwner->GetTargetPos();
	XMVECTOR vTarget = XMLoadFloat3(&playerPosF);

	// 現在の周回中心の位置をロード (m_InitBossPosを使用)
	XMVECTOR vCurrentCenter = XMLoadFloat3(&m_InitBossPos);

	// 目標 (vTarget) への方向ベクトルを計算
	XMVECTOR vDirToTarget = XMVectorSubtract(vTarget, vCurrentCenter);

	// Y軸方向の移動は無視し、X-Z平面の移動のみとする
	vDirToTarget = XMVectorSetY(vDirToTarget, 0.0f);

	// 目標との距離をチェック
	float distance_center = XMVectorGetX(XMVector3Length(vDirToTarget));

	// 周回中心の移動速度を定義 (調整用)
	constexpr float CENTER_MOVE_SPEED = 0.15f;
	float moveStep = CENTER_MOVE_SPEED * delta;

	// 距離が移動ステップ量より大きい、または近づいている場合のみ移動
	if (distance_center > 0.001f)
	{
		// 移動ステップ量を正規化して適用
		XMVECTOR vMove = XMVector3Normalize(vDirToTarget);
		// 目標を通り過ぎないように、移動量を制限する
		vMove = XMVectorScale(vMove, std::min(moveStep, distance_center));

		// 周回中心の位置を更新 (ゆっくりプレイヤーに近づく)
		vCurrentCenter = XMVectorAdd(vCurrentCenter, vMove);
		XMStoreFloat3(&m_InitBossPos, vCurrentCenter); // メンバ変数に保存
	}

	//----------------------------------------
	// 1. 円運動（更新された中心 vCurrentCenter を周回する）
	//----------------------------------------

	XMVECTOR vCenter = vCurrentCenter; // 更新された周回中心を使用

	// 回転角度を更新
	m_RotationAngle += m_RotationSpeed * delta * m_rotationDirection;

	// 角度が制限を超えたら方向を反転（左右往復）
	const float MAX_ANGLE = XM_PIDIV4; // π/4 ラジアン (45度)
	if (fabsf(m_RotationAngle) > MAX_ANGLE)
		m_rotationDirection *= -1.0f;

	// ボスが中心から離れる距離（円の半径）を定義
	constexpr float fixedDistance = 10.0f;

	// オフセット基準ベクトル (Z軸方向にfixedDistance離れた点)
	XMVECTOR initialOffset = XMVectorSet(0.0f, 0.0f, fixedDistance, 0.0f);

	// Y軸回転行列を作成し、オフセットベクトルに適用
	XMMATRIX rot = XMMatrixRotationY(m_RotationAngle);
	XMVECTOR vOffset = XMVector3Transform(initialOffset, rot);

	// 中心位置にオフセットを加算し、新しいボスの位置を決定
	XMVECTOR vNew = XMVectorAdd(vCenter, vOffset);

	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, vNew);
	m_pOwner->SetPosition(newPos); // ボスの位置を直接設定

	//----------------------------------------
	// 2. プレイヤー方向を向く
	//----------------------------------------

	// プレイヤーへの方向ベクトルを計算 (プレイヤー位置 - ボス位置)
	XMVECTOR vLookDir = XMVectorSubtract(vTarget, vNew); // vTarget (プレイヤー位置) を使用

	float dx = XMVectorGetX(vLookDir);
	float dz = XMVectorGetZ(vLookDir);

	// atan2fでY軸回転角度を計算
	float angle = atan2f(dx, dz);
	m_pOwner->SetRotationY(angle); // ボスをプレイヤーの方向に向かせる

	// -----------------------
	// 3. 一定時間経過後に攻撃判定
	// -----------------------
	constexpr float AttackDelay = 270.0f; // 2秒後に攻撃可能 (この値はフレーム数か秒か要確認)
	if (m_Timer >= AttackDelay)
	{
		// プレイヤーとボスの距離を再計算
		float distance_to_player = XMVectorGetX(XMVector3Length(vLookDir));

		std::vector<std::function<std::unique_ptr<StateBase<Boss>>()>> candidates;

		// 距離に応じた攻撃候補の選択ロジック
		if (distance_to_player < 8.0f)
		{
			candidates = {
				[this]() { return std::make_unique<BossSlashState>(m_pOwner); },
				[this]() { return std::make_unique<BossChargeState>(m_pOwner); },
				[this]() { return std::make_unique<BossStompState>(m_pOwner); }
			};
		}
		else if (distance_to_player < 30.0f)
		{
			candidates = {
				[this]() { return std::make_unique<BossThrowingState>(m_pOwner); },
				[this]() { return std::make_unique<BossShoutState>(m_pOwner); }
			};
		}
		else
		{
			candidates = {
				[this]() { return std::make_unique<BossSpecialState>(m_pOwner); },
				[this]() { return std::make_unique<BossLaserState>(m_pOwner); }
			};
		}

		if (!candidates.empty())
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(0, static_cast<int>(candidates.size()) - 1);

			auto nextAttack = candidates[dis(gen)]();
			m_pOwner->GetStateMachine()->ChangeState(std::move(nextAttack));
			return;
		}
	}

	// アニメーション設定（Move用）
	m_AnimNo = 0;
	m_AnimTimer = 0.0;
}


void BossMoveState::LateUpdate()
{
}

void BossMoveState::Draw()
{
}

void BossMoveState::Exit()
{
}

void BossMoveState::SetInitialAngle(float angle)
{
	m_RotationAngle = angle;
}