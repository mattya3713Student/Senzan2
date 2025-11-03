#include "BossChargeSlashState.h"

#include "Game//04_Time//Time.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//02_Boss//Boss.h"
#include "Game//01_GameObject//00_MeshObject//00_Character//01_Player//Player.h"
#include "..//..//BossMoveState//BossMoveState.h" // 攻撃後の遷移先

//ボーンの位置を取得するために使用するImGuiです.
#include "System/Singleton/ImGui/CImGuiManager.h"

// Transform.h のインクルードが不足している場合は追加してください
#include <DirectXMath.h>
#include <memory> 
#include <cstdio> // printf のため

constexpr float MY_PI = 3.1415926535f;

BossChargeSlashState::BossChargeSlashState(Boss* owner)
	: BossAttackStateBase(owner)
	, m_ChargeDuration(1.5f)
	, m_SlashDuration(0.3f)
	, m_CooldownDuration(1.0f)
	, m_SlashRange(4.0f)
	, m_SlashAngle(MY_PI / 2.0f)
	, m_CurrentPhase(Phase::Charge)
	, m_PhaseTime(0.0f)
	, m_StartPos()
	, m_HasHit(false)
{
}

BossChargeSlashState::~BossChargeSlashState()
{
}

void BossChargeSlashState::Enter()
{
	m_PhaseTime = 0.0f;
	m_CurrentPhase = Phase::Charge;
	m_HasHit = false;

	//ボスの向きを設定.
	const DirectX::XMFLOAT3 BossPosF = m_pOwner->GetPosition();
	DirectX::XMVECTOR BossPosXM = DirectX::XMLoadFloat3(&BossPosF);

	const DirectX::XMFLOAT3 PlayerPosF = m_pOwner->m_PlayerPos;
	DirectX::XMVECTOR PlayerPosXM = DirectX::XMLoadFloat3(&PlayerPosF);

	//XMVectorSubtract: 引数の左から右を引く関数.
	DirectX::XMVECTOR Direction = DirectX::XMVectorSubtract(PlayerPosXM, BossPosXM);
	//X,Z方向の平面の方向ベクトル.
	Direction = DirectX::XMVectorSetY(Direction, 0.0f);

	//Y軸回転角度を計算し、ボスをプレイヤーに向かせる.
	float dx = DirectX::XMVectorGetX(Direction);
	float dz = DirectX::XMVectorGetZ(Direction);
	float angle_radian = std::atan2f(dx, dz);
	m_pOwner->SetRotationY(angle_radian);

	//初期位置を保存.
	DirectX::XMStoreFloat3(&m_StartPos, BossPosXM);
}

void BossChargeSlashState::Update()
{
	const float Time_Rate = 1.0f;
	m_Attacktime += Time_Rate * Time::GetInstance().GetDeltaTime();

	m_PhaseTime += Time::GetInstance().GetDeltaTime();

	m_AnimNo = 5;		//登場アニメーション
	m_AnimTimer = 0.0;	//アニメーション経過時間初期化

	m_pOwner->ChangeAnim(m_AnimNo);
	m_pOwner->SetIsLoop(true);
}

void BossChargeSlashState::LateUpdate()
{
}

void BossChargeSlashState::Draw()
{
	BoneDraw(); 
}

void BossChargeSlashState::Exit()
{
}

void BossChargeSlashState::BoneDraw()
{
	static bool isAnimSpeed = true;
	static DirectX::XMFLOAT3 ColliderOffset;
	ImGui::Begin("Collider Debug");

	if (ImGui::Button("isAnimSpeed"))
	{
		isAnimSpeed = !isAnimSpeed;
	}

	// X軸オフセットのスライダー
	ImGui::SliderFloat("Offset X", &ColliderOffset.x, -10.0f, 10.0f);

	// Y軸オフセットのスライダー
	ImGui::SliderFloat("Offset Y", &ColliderOffset.y, -10.0f, 10.0f);

	// Z軸オフセットのスライダー
	ImGui::SliderFloat("Offset Z", &ColliderOffset.z, -10.0f, 10.0f);

	static float angle_radion = 180.f;
	// Z軸オフセットのスライダー
	ImGui::SliderFloat("Y", &angle_radion, 0.0f, 360.0f);

	m_pOwner->SetRotationY(angle_radion * (DirectX::XM_PI / 180.0f));

	ImGui::End();
	if (isAnimSpeed)
	{
		m_pOwner->SetAnimSpeed(0.0000000001f);
	}
	else
	{

		m_pOwner->SetAnimSpeed(m_AnimSpeed);
	}

	if (m_pOwner->GetAttachMesh().expired()) return;
	auto staticMesh = std::dynamic_pointer_cast<SkinMesh>(m_pOwner->GetAttachMesh().lock());
	if (!staticMesh) return;

	const std::string targetBoneName = "blade_r_head";
	DirectX::XMFLOAT3 bone_world_pos;

	// 剣の当たり判定のローカルオフセット
	//XMMATRIX ColliderOffsetMatrix = XMMatrixTranslation(0.0f, 5.0f, 0.0f);

	if (staticMesh->GetPosFromBone(targetBoneName.c_str(), &bone_world_pos))
	{
		// 座標.
		//DirectX::XMVECTOR v_bone_world_pos = DirectX::XMLoadFloat3(&bone_world_pos);
		//DirectX::XMVECTOR v_coll_local_offset = DirectX::XMLoadFloat3(&m_pColl->GetPositionOffset());

		//// 親の回転.
		//DirectX::XMVECTOR v_parent_world_quaternion = DirectX::XMLoadFloat4(&m_pOwner->GetTransform().Quaternion);

		//// ボーンの回転.
		//DirectX::XMMATRIX bone_matrix = {};
		//staticMesh->GetMatrixFromBone(targetBoneName.c_str(), &bone_matrix);
		//DirectX::XMVECTOR v_bone_local_quaternion = DirectX::XMQuaternionRotationMatrix(bone_matrix);

		//// 親のワールド回転とボーンのローカル回転を合成.
		//DirectX::XMVECTOR v_bone_world_quaternion = DirectX::XMQuaternionMultiply(v_bone_local_quaternion, v_parent_world_quaternion);

		//// ボーンの回転を考慮した座標.
		//DirectX::XMVECTOR v_rotated_offset = DirectX::XMVector3Rotate(v_coll_local_offset, v_bone_world_quaternion);

		//// 最終的な座標.
		//DirectX::XMVECTOR v_final_world_pos = DirectX::XMVectorAdd(v_bone_world_pos, v_rotated_offset);
		//DirectX::XMFLOAT3 final_position;
		//DirectX::XMStoreFloat3(&final_position, v_final_world_pos);

		//// 最終的なスケール.
		//DirectX::XMFLOAT3 final_scale = { 0.05f, 0.05f, 0.05f };

		////// 加える回転.
		////const float correction_angle = -DirectX::XM_PIDIV2;
		////DirectX::XMVECTOR v_correct_quaternion = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, correction_angle);

		////// 最終的な回転.
		////DirectX::XMVECTOR v_final_quaternion_corrected = DirectX::XMQuaternionMultiply(v_correct_quaternion, v_bone_quaternion);
		//DirectX::XMFLOAT4 final_quaternion;
		//DirectX::XMStoreFloat4(&final_quaternion, v_bone_world_quaternion);

		//std::shared_ptr<Transform> finalColliderTransform = std::make_shared<Transform>();
		//finalColliderTransform->SetPosition(final_position);
		//finalColliderTransform->SetQuaternion(final_quaternion);
		//finalColliderTransform->SetScale(final_scale);

		//m_pColl->Draw(finalColliderTransform);
	}
}


void BossChargeSlashState::BossAttack()
{
	if (m_CurrentPhase != Phase::Attack) return;
}