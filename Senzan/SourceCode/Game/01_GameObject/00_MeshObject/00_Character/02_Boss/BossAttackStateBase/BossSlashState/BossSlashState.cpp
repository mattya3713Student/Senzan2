#include "BossSlashState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h" // 攻撃後の遷移先
#include "Resource\\Mesh\\02_Skin\\SkinMesh.h"

constexpr float MY_PI = 3.1415926535f;



BossSlashState::BossSlashState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_SlashDuration(0.5f)    // 攻撃全体は0.5秒
	, m_SlashRange(3.0f)    // 射程3.0m
	, m_SlashAngle(MY_PI / 3.0f) // 60度
	, m_BackWardDistance(1.0f)    // 1.0m 後退
	, m_BackWardDuration(0.15f)   // 0.15秒かけて後退
	, m_StartPos()
	, m_ForWardVector()
	, m_HasHit(false)

	, m_IdolDuration(1.5f)      // 1.5秒予兆
	, m_AttackDuration(1.0f)      // 1.0秒かけて薙ぎ払う
	, m_CoolDownDuration(1.5f)      // 1.5秒硬直

	, m_CurrentPhase(Phase::Idol)
	, m_PhaseTime(0.0f)

	, m_pTransform(std::make_shared<Transform>())
{
	//m_pColl->SetHeight(50.0f);
	//m_pColl->SetRadius(5.0f);
}

BossSlashState::~BossSlashState()
{
}

void BossSlashState::Enter()
{
	m_Attacktime	= 0.0f;
	m_HasHit		= false;

	//ボスの向きを設定.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
	//X,Z平面の方向.
	Direction = DirectX::XMVectorSetY(Direction, 0.0f);

	//Y軸回転角度を計算し、ボスをプレイヤーに向かせる.
	float dx = DirectX::XMVectorGetX(Direction);
	float dz = DirectX::XMVectorGetZ(Direction);
	float angle_radian = std::atan2f(dx, dz);
	m_pOwner->SetRotationY(angle_radian);

	//初期位置を保存.
	DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);


}

void BossSlashState::Update()
{
	const float Time_Rate = 1.0f;
	m_Attacktime += Time_Rate * Time::GetInstance().GetDeltaTime();

	//全体の時間とフェーズ内時間を進める.
	m_Attacktime += Time_Rate * Time::GetInstance().GetDeltaTime();
	m_PhaseTime += Time::GetInstance().GetDeltaTime();
	//switch (m_CurrentPhase)
	//{
	//case BossSlashState::Phase::Idol:
	//	if (m_PhaseTime >= m_IdolDuration)
	//	{
	//		//予兆から攻撃.
	//		m_CurrentPhase = Phase::Attack;
	//		m_PhaseTime = 0.0f;
	//	}
	//	break;
	//case BossSlashState::Phase::Attack:
	//	BossAttack();
	//	if (m_PhaseTime >= m_AttackDuration)
	//	{
	//		//攻撃からクールダウン.
	//		m_CurrentPhase = Phase::Attack;
	//		m_PhaseTime = 0.0f;
	//	}
	//	break;
	//case BossSlashState::Phase::CoolDown:
	//	if (m_PhaseTime >= m_CoolDownDuration)
	//	{
	//		//動作クラスへの移動.
	//		m_pOwner->GetStateMachine()->ChangeState(std::make_shared<BossMoveState>(m_pOwner));
	//		return;
	//	}
	//	break;
	//default:
	//	break;
	//}


	//待機アニメーションを
//	m_AnimNo = 0;

	//アニメーション切替
		m_AnimNo = 5;		//登場アニメーション
		m_AnimTimer = 0.0;	//アニメーション経過時間初期化
		m_pOwner->SetAnimSpeed(m_AnimSpeed);

		//アニメーション再生の無限ループ用.
		//m_pOwner->SetIsLoop(true);
		m_pOwner->ChangeAnim(m_AnimNo);

	
		// --- ボーン追従カプセル描画処理 ---
		//{
		//	// ボスのトランスフォームを基準に生成
		//	Transform def = m_pOwner->GetTransform();
		//	std::shared_ptr<Transform> transform = std::make_shared<Transform>(def);

		//	// スキンメッシュ取得
		//	if (m_pOwner->GetAttachMesh().expired()) return;
		//	std::shared_ptr<SkinMesh> staticMesh =
		//		std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
		//	if (!staticMesh)
		//	{
		//		printf("SkinMesh not found.\n");
		//		return;
		//	}

		//	const std::string targetBoneName = "blade_r_head";
		//	DirectX::XMFLOAT3 bonePos{};
		//	if (staticMesh->GetPosFromBone(targetBoneName.c_str(), &bonePos))
		//	{
		//		transform->SetPosition(bonePos);
		//		printf("Bone [%s] Pos = (%.2f, %.2f, %.2f)\n",
		//			targetBoneName.c_str(), bonePos.x, bonePos.y, bonePos.z);
		//	}
		//	else
		//	{
		//		printf("Bone [%s] not found!\n", targetBoneName.c_str());
		//		return;
		//	}

		//	// m_pColl を CapsuleCollider にキャスト
		//	std::shared_ptr<CapsuleCollider> capsule =
		//		std::dynamic_pointer_cast<CapsuleCollider>(m_pColl);
		//	if (!capsule)
		//	{
		//		printf("Failed to cast m_pColl to CapsuleCollider.\n");
		//		return;
		//	}

		//	// カプセル設定・描画
		//	//原因
		//	capsule->Draw(transform);
		//}



}

void BossSlashState::LateUpdate()
{
}

void BossSlashState::Draw()
{
	UpdateBoneCheck();
}

void BossSlashState::Exit()
{
}

void BossSlashState::UpdateBoneCheck()
{
	{
		//// ボスのトランスフォームをベースに作成
		//Transform def = m_pOwner->GetTransform();
		//std::shared_ptr<Transform> transform = std::make_shared<Transform>(def);

		//// スキンメッシュを取得（AttachMesh 経由）
		//std::shared_ptr<SkinMesh> staticMesh =
		//	std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
		//if (!staticMesh) return; // nullチェック

		//// 取得したいボーン名を指定
		//const char* targetBoneName = "blade_r_head";

		//// ボーンの位置を格納する変数
		//DirectX::XMFLOAT3 bonePos{};

		//// ボーンの位置を取得
		//if (staticMesh->GetPosFromBone(targetBoneName, &bonePos))
		//{
		//	// デバッグ出力で確認
		//	printf("Bone [%s] Pos = (%.2f, %.2f, %.2f)\n",
		//		targetBoneName, bonePos.x, bonePos.y, bonePos.z);

		//	// トランスフォームの位置をボーン位置に設定
		//	transform->SetPosition(bonePos);
		//}
		//else
		//{
		//	printf("Bone [%s] not found!\n", targetBoneName);
		//	return;
		//}

		//// 🔽 LTest のメンバ変数をキャスト
		//std::shared_ptr<CapsuleCollider> capsule =
		//	std::dynamic_pointer_cast<CapsuleCollider>(m_pColl);

		//if (!capsule)
		//{
		//	printf("CapsuleCollider dynamic_cast failed!\n");
		//	return;
		//}

		//// コライダー設定
		//capsule->SetHeight(5.0f);
		//capsule->SetRadius(10.0f);

		//// 描画（ボーン位置に表示される）
		//capsule->Draw(transform);
	}

	//
	if (m_pOwner->GetAttachMesh().expired()) return;

	//アニメーション切替
	if (GetAsyncKeyState('N') & 0x8000)
	{
	}
}


void BossSlashState::BossAttack()
{
	//現在のY軸回転角度を取得.
	const float currentRotationY = m_pOwner->GetRotationY();

	//Y軸周りの回転行列で変換.
	DirectX::XMMATRIX RotatedMatrix = DirectX::XMMatrixRotationY(currentRotationY);

	//基準ベクトル.
	DirectX::XMVECTOR InitialVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR ForWardVec = DirectX::XMVector3Transform(InitialVec, RotatedMatrix);
	//正規化.
	ForWardVec = DirectX::XMVector3Normalize(ForWardVec);

	//後退しているときだけ入る.
	DirectX::XMVECTOR currentPosXM;

	if (m_Attacktime < m_BackWardDuration)
	{
		float BackWardDT = m_Attacktime / m_BackWardDuration;
		float currentBackWardDist = m_BackWardDistance * BackWardDT;

		// 後ろに移動するベクトル = 前方ベクトルの逆方向.
		DirectX::XMVECTOR backwardVec = DirectX::XMVectorScale(ForWardVec, -currentBackWardDist);

		// 新しい位置 = スタート位置 + 後退ベクトル.
		currentPosXM = DirectX::XMVectorAdd(XMLoadFloat3(&m_StartPos), backwardVec);

		// 位置を更新
		DirectX::XMFLOAT3 posToSet;
		DirectX::XMStoreFloat3(&posToSet, currentPosXM);
		m_pOwner->SetPosition(posToSet);
	}
	//後退が終わったら、最終後退位置で固定.
	else
	{
		//最終的な後退位置を計算して固定.
		DirectX::XMVECTOR finalBackwardVec = DirectX::XMVectorScale(ForWardVec, +m_BackWardDistance);
		currentPosXM = DirectX::XMVectorAdd(XMLoadFloat3(&m_StartPos), finalBackwardVec);

		// 位置を更新/
		DirectX::XMFLOAT3 posToSet;
		DirectX::XMStoreFloat3(&posToSet, currentPosXM);
		m_pOwner->SetPosition(posToSet);
	}



	////【当たり判定のチェック】: 後退後、攻撃モーションのピーク時 (例: 0.2秒後) に実行
	//if (m_Attacktime >= 0.2f && !m_HasHit)
	//{
	//	const DirectX::XMFLOAT3& bossCurrentPosF = m_pOwner->GetPosition();
	//	DirectX::XMVECTOR bossPosXM = XMLoadFloat3(&bossCurrentPosF);

	//	DirectX::XMVECTOR playerPosXM = XMLoadFloat3(&m_pOwner->m_TargetPos);

	//	// --- 縦斬りの扇形判定 ---
	//	DirectX::XMVECTOR toPlayerVec = DirectX::XMVectorSubtract(playerPosXM, bossPosXM);
	//	toPlayerVec = DirectX::XMVectorSetY(toPlayerVec, 0.0f); // XZ平面のみで判定

	//	// 距離のチェック
	//	float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(toPlayerVec));
	//	if (distSq <= m_SlashRange * m_SlashRange)
	//	{
	//		// 角度のチェック (前方ベクトルとの角度)
	//		DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(ForWardVec), DirectX::XMVector3Normalize(toPlayerVec));
	//		float cosAngle = DirectX::XMVectorGetX(dotVec);

	//		if (cosAngle >= std::cos(m_SlashAngle / 2.0f))
	//		{
	//			// ダメージ処理の実行
	//		}
	//	}

	//	m_HasHit = true;
	//}
}
