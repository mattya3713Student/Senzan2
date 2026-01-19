#include "Boss.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossIdolState/BossIdolState.h"
#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossMoveState/BossMoveState.h"

#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"


#include "BossAttackStateBase/BossAttackStateBase.h"
#include "BossAttackStateBase/BossStompState/BossStompState.h"
#include "BossAttackStateBase/BossSlashState/BossSlashState.h"
#include "BossAttackStateBase/BossChargeSlashState/BossChargeSlashState.h"
#include "BossAttackStateBase/BossShoutState/BossShoutState.h"

#include "System/Utility/StateMachine/StateMachine.h"

#include "BossAttackStateBase/BossSpecialState/BossSpecialState.h"
#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossLaserState/BossLaserState.h"
#include "BossAttackStateBase/BossParryState/BossParryState.h"

#include "Resource/Mesh/02_Skin/SkinMesh.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossDeadState/BossDeadState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeState/BossChargeState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"

#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "System/Singleton/ImGui/CImGuiManager.h"


constexpr float HP_Max = 10000.0f;

Boss::Boss()
	: Character()
	, m_State(std::make_unique<StateMachine<Boss>>(this))
	, m_PlayerPos{}
	, m_TurnSpeed(0.1f)
	, m_MoveSpeed(0.3f)
	, m_vCurrentMoveVelocity(0.f, 0.f, 0.f)
	, deleta_time(0.f)
	, m_HitPoint(0.0f)

	, m_pSlashCollider(nullptr)

	, m_pStompCollider(nullptr)

	, m_pShoutCollider(nullptr)
{
	AttachMesh(MeshManager::GetInstance().GetSkinMesh("boss"));

	//DirectX::XMFLOAT3 pos = { 0.05f, 10.0f, 20.05f };
	DirectX::XMFLOAT3 pos = { 0.05f, 0.05f, 20.05f };
	DirectX::XMFLOAT3 scale = { 10.0f, 10.0f, 10.0f };
	DirectX::XMFLOAT3 Rotation = { 0.0f,0.0f,0.0f };
	m_spTransform->SetPosition(pos);
	m_spTransform->SetScale(scale);
	m_spTransform->SetRotationDegrees(Rotation);

	m_MaxHP = 1000000.f;
	m_HP = m_MaxHP;

	// 攻撃の追加.
	std::unique_ptr<CapsuleCollider> attackCollider = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pAttackCollider = attackCollider.get();

	attackCollider->SetActive(false);
	attackCollider->SetColor(Color::eColor::Red);
	attackCollider->SetAttackAmount(5.0f);
	attackCollider->SetHeight(0.0);
	attackCollider->SetRadius(0.0);
	attackCollider->SetPositionOffset(0.f, -50.0f, -20.f);
	attackCollider->SetMyMask(eCollisionGroup::Enemy_Attack);
	attackCollider->SetTarGetTargetMask(eCollisionGroup::Player_Damage | eCollisionGroup::Player_Dodge | eCollisionGroup::Player_JustDodge | eCollisionGroup::Player_Parry);

	m_upColliders->AddCollider(std::move(attackCollider));

	//ボスの最大体力.
	m_HitPoint = HP_Max;

	// 被ダメの追加.
	std::unique_ptr<CapsuleCollider> damage_collider = std::make_unique<CapsuleCollider>(m_spTransform);

	damage_collider->SetColor(Color::eColor::Yellow);
	damage_collider->SetHeight(30.0f);
	damage_collider->SetRadius(10.0f);
	damage_collider->SetPositionOffset(0.f, 1.5f, 0.f);
	damage_collider->SetAttackAmount(10.f);
	damage_collider->SetMyMask(eCollisionGroup::Enemy_Damage);
	damage_collider->SetTarGetTargetMask(eCollisionGroup::Player_Attack);

	m_upColliders->AddCollider(std::move(damage_collider));

	// プレスの追加.
	std::unique_ptr<CapsuleCollider> press_collider = std::make_unique<CapsuleCollider>(m_spTransform);

	press_collider->SetColor(Color::eColor::Cyan);
	press_collider->SetHeight(20.0f);
	press_collider->SetRadius(5.1f);
	press_collider->SetPositionOffset(0.f, 1.5f, 0.f);
	press_collider->SetMyMask(eCollisionGroup::BossPress);
	press_collider->SetTarGetTargetMask(eCollisionGroup::Press);

	m_upColliders->AddCollider(std::move(press_collider));

	//通常攻撃の当たり判定作成.
	auto slashCol = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pSlashCollider = slashCol.get();
	m_pSlashCollider->SetMyMask(eCollisionGroup::Enemy_Attack);
	m_pSlashCollider->SetTarGetTargetMask(eCollisionGroup::Player_Damage);
	m_pSlashCollider->SetAttackAmount(10.0f); 
	m_pSlashCollider->SetRadius(15.0f);         
	m_pSlashCollider->SetHeight(40.0f);         
	m_pSlashCollider->SetPositionOffset(0.0f, 0.0f, 0.0f); 
	m_pSlashCollider->SetActive(false);
	m_pSlashCollider->SetColor(Color::eColor::Red);

	m_upColliders->AddCollider(std::move(slashCol));

	auto stompCol = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pStompCollider = stompCol.get();

	m_pStompCollider->SetMyMask(eCollisionGroup::Enemy_Attack);
	m_pStompCollider->SetTarGetTargetMask(eCollisionGroup::Player_Damage );

	m_pStompCollider->SetAttackAmount(5.0f);
	m_pStompCollider->SetRadius(30.0f);
	m_pStompCollider->SetHeight(15.0f);

	m_pStompCollider->SetActive(false);
	m_pStompCollider->SetColor(Color::eColor::Gray);

	m_upColliders->AddCollider(std::move(stompCol));

	std::unique_ptr<CapsuleCollider> Shout_collider = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pShoutCollider = Shout_collider.get(); 

	Shout_collider->SetColor(Color::eColor::White);
	Shout_collider->SetHeight(75.0f);
	Shout_collider->SetRadius(50.0f);
	Shout_collider->SetPositionOffset(0.f, 1.5f, 0.f);
	Shout_collider->SetAttackAmount(10.f);
	Shout_collider->SetMyMask(eCollisionGroup::Enemy_Attack); 
	Shout_collider->SetTarGetTargetMask(eCollisionGroup::Player_Attack);

	m_pShoutCollider->SetActive(false);
	m_upColliders->AddCollider(std::move(Shout_collider));

    m_State->ChangeState(std::make_shared<BossSlashState>(this));
    /* BossSlashState
 BossChargeState
 BossChargeSlashState
 BossLaserState
 BossShoutState
 BossSlashState
 BossSpecialState
 BossStompState
 BossThrowingState*/
	CollisionDetector::GetInstance().RegisterCollider(*m_upColliders);
}

Boss::~Boss()
{
	CollisionDetector::GetInstance().UnregisterCollider(*m_upColliders);
}

void Boss::Update()
{
	Character::Update();

	//距離の計算後にステートを更新する.
	m_State->Update();

#if _DEBUG
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
        m_State->ChangeState(std::make_shared<BossSlashState>(this));
	}
#endif
}

void Boss::LateUpdate()
{
	Character::LateUpdate();

	if (!m_State) {
		return;
	}

	// ステートマシーンの最終更新を実行.
	m_State->LateUpdate();

    // アクティブなコライダーがあれば対応するボーンから Transform を更新して外部供給する
    if (m_pSlashCollider && m_pSlashCollider->GetActive()) {
        // convert degree Euler offsets to quaternion
        DirectX::XMFLOAT3 deg = m_SlashRotOffsetDeg;
        DirectX::XMVECTOR rotRad = DirectX::XMVectorSet(DirectX::XMConvertToRadians(deg.x), DirectX::XMConvertToRadians(deg.y), DirectX::XMConvertToRadians(deg.z), 0.0f);
        DirectX::XMVECTOR qx = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1,0,0,0), DirectX::XMVectorGetX(rotRad));
        DirectX::XMVECTOR qy = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,1,0,0), DirectX::XMVectorGetY(rotRad));
        DirectX::XMVECTOR qz = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,0,1,0), DirectX::XMVectorGetZ(rotRad));
        DirectX::XMVECTOR q = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionMultiply(qz, qy), qx);
        DirectX::XMFLOAT4 rotOffset; DirectX::XMStoreFloat4(&rotOffset, q);

        if (!m_pSlashBoneFrame) {
            if (auto skin = std::dynamic_pointer_cast<SkinMesh>(GetAttachMesh().lock())) {
                m_pSlashBoneFrame = skin->GetFrameByName("boss_Hand_R");
            }
        }

        if (m_pSlashBoneFrame) {
            MYFRAME* frame = (MYFRAME*)m_pSlashBoneFrame;
            DirectX::XMMATRIX bone_local_matrix = D3DXMatrixToXMMatrix(frame->CombinedTransformationMatrix);
            DirectX::XMMATRIX boss_world_matrix = m_spTransform->GetWorldMatrix();
            DirectX::XMMATRIX bone_world_matrix = bone_local_matrix * boss_world_matrix;

            DirectX::XMVECTOR v_final_pos, v_final_quat, v_final_scale;
            DirectX::XMMatrixDecompose(&v_final_scale, &v_final_quat, &v_final_pos, bone_world_matrix);
            DirectX::XMStoreFloat3(&m_SlashBoneWorldTransform.Position, v_final_pos);
            
            DirectX::XMVECTOR q_offset = DirectX::XMLoadFloat4(&rotOffset);
            DirectX::XMVECTOR q_result = DirectX::XMQuaternionMultiply(q_offset, v_final_quat);
            DirectX::XMStoreFloat4(&m_SlashBoneWorldTransform.Quaternion, q_result);
            DirectX::XMStoreFloat3(&m_SlashBoneWorldTransform.Scale, v_final_scale);
            m_SlashBoneWorldTransform.UpdateRotationFromQuaternion();

            DirectX::XMVECTOR b_pos, b_quat, b_scale;
            DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, boss_world_matrix);
            DirectX::XMVECTOR relative_pos = DirectX::XMVectorSubtract(v_final_pos, b_pos);
            DirectX::XMFLOAT3 f_relative_pos; DirectX::XMStoreFloat3(&f_relative_pos, relative_pos);
            m_pSlashCollider->SetPositionOffset(f_relative_pos.x, f_relative_pos.y, f_relative_pos.z);
        }
        else {
            UpdateColliderFromBone("boss_Hand_R", m_pSlashCollider, m_SlashBoneWorldTransform, true, rotOffset);
        }
    }
    if (m_pStompCollider && m_pStompCollider->GetActive()) {
        DirectX::XMFLOAT3 deg = m_StompRotOffsetDeg;
        DirectX::XMVECTOR rotRad = DirectX::XMVectorSet(DirectX::XMConvertToRadians(deg.x), DirectX::XMConvertToRadians(deg.y), DirectX::XMConvertToRadians(deg.z), 0.0f);
        DirectX::XMVECTOR qx = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1,0,0,0), DirectX::XMVectorGetX(rotRad));
        DirectX::XMVECTOR qy = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,1,0,0), DirectX::XMVectorGetY(rotRad));
        DirectX::XMVECTOR qz = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,0,1,0), DirectX::XMVectorGetZ(rotRad));
        DirectX::XMVECTOR q = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionMultiply(qz, qy), qx);
        DirectX::XMFLOAT4 rotOffset; DirectX::XMStoreFloat4(&rotOffset, q);

        if (!m_pStompBoneFrame) {
            if (auto skin = std::dynamic_pointer_cast<SkinMesh>(GetAttachMesh().lock())) {
                m_pStompBoneFrame = skin->GetFrameByName("boss_pSphere28");
            }
        }

        if (m_pStompBoneFrame) {
            MYFRAME* frame = (MYFRAME*)m_pStompBoneFrame;
            DirectX::XMMATRIX bone_local_matrix = D3DXMatrixToXMMatrix(frame->CombinedTransformationMatrix);
            DirectX::XMMATRIX boss_world_matrix = m_spTransform->GetWorldMatrix();
            DirectX::XMMATRIX bone_world_matrix = bone_local_matrix * boss_world_matrix;

            DirectX::XMVECTOR v_final_pos, v_final_quat, v_final_scale;
            DirectX::XMMatrixDecompose(&v_final_scale, &v_final_quat, &v_final_pos, bone_world_matrix);
            DirectX::XMStoreFloat3(&m_StompBoneWorldTransform.Position, v_final_pos);

            DirectX::XMVECTOR q_offset = DirectX::XMLoadFloat4(&rotOffset);
            DirectX::XMVECTOR q_result = DirectX::XMQuaternionMultiply(q_offset, v_final_quat);
            DirectX::XMStoreFloat4(&m_StompBoneWorldTransform.Quaternion, q_result);
            DirectX::XMStoreFloat3(&m_StompBoneWorldTransform.Scale, v_final_scale);
            m_StompBoneWorldTransform.UpdateRotationFromQuaternion();

            DirectX::XMVECTOR b_pos, b_quat, b_scale;
            DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, boss_world_matrix);
            DirectX::XMVECTOR relative_pos = DirectX::XMVectorSubtract(v_final_pos, b_pos);
            DirectX::XMFLOAT3 f_relative_pos; DirectX::XMStoreFloat3(&f_relative_pos, relative_pos);
            m_pStompCollider->SetPositionOffset(f_relative_pos.x, f_relative_pos.y, f_relative_pos.z);
        }
        else {
            UpdateColliderFromBone("boss_pSphere28", m_pStompCollider, m_StompBoneWorldTransform, true, rotOffset);
        }
    }
    if (m_pShoutCollider && m_pShoutCollider->GetActive()) {
        DirectX::XMFLOAT3 deg = m_ShoutRotOffsetDeg;
        DirectX::XMVECTOR rotRad = DirectX::XMVectorSet(DirectX::XMConvertToRadians(deg.x), DirectX::XMConvertToRadians(deg.y), DirectX::XMConvertToRadians(deg.z), 0.0f);
        DirectX::XMVECTOR qx = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(1,0,0,0), DirectX::XMVectorGetX(rotRad));
        DirectX::XMVECTOR qy = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,1,0,0), DirectX::XMVectorGetY(rotRad));
        DirectX::XMVECTOR qz = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0,0,1,0), DirectX::XMVectorGetZ(rotRad));
        DirectX::XMVECTOR q = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionMultiply(qz, qy), qx);
        DirectX::XMFLOAT4 rotOffset; DirectX::XMStoreFloat4(&rotOffset, q);

        if (!m_pShoutBoneFrame) {
            if (auto skin = std::dynamic_pointer_cast<SkinMesh>(GetAttachMesh().lock())) {
                m_pShoutBoneFrame = skin->GetFrameByName("boss_Shout");
            }
        }

        if (m_pShoutBoneFrame) {
            MYFRAME* frame = (MYFRAME*)m_pShoutBoneFrame;
            DirectX::XMMATRIX bone_local_matrix = D3DXMatrixToXMMatrix(frame->CombinedTransformationMatrix);
            DirectX::XMMATRIX boss_world_matrix = m_spTransform->GetWorldMatrix();
            DirectX::XMMATRIX bone_world_matrix = bone_local_matrix * boss_world_matrix;

            DirectX::XMVECTOR v_final_pos, v_final_quat, v_final_scale;
            DirectX::XMMatrixDecompose(&v_final_scale, &v_final_quat, &v_final_pos, bone_world_matrix);
            DirectX::XMStoreFloat3(&m_ShoutBoneWorldTransform.Position, v_final_pos);

            DirectX::XMVECTOR q_offset = DirectX::XMLoadFloat4(&rotOffset);
            DirectX::XMVECTOR q_result = DirectX::XMQuaternionMultiply(q_offset, v_final_quat);
            DirectX::XMStoreFloat4(&m_ShoutBoneWorldTransform.Quaternion, q_result);
            DirectX::XMStoreFloat3(&m_ShoutBoneWorldTransform.Scale, v_final_scale);
            m_ShoutBoneWorldTransform.UpdateRotationFromQuaternion();

            DirectX::XMVECTOR b_pos, b_quat, b_scale;
            DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, boss_world_matrix);
            DirectX::XMVECTOR relative_pos = DirectX::XMVectorSubtract(v_final_pos, b_pos);
            DirectX::XMFLOAT3 f_relative_pos; DirectX::XMStoreFloat3(&f_relative_pos, relative_pos);
            m_pShoutCollider->SetPositionOffset(f_relative_pos.x, f_relative_pos.y, f_relative_pos.z);
        }
        else {
            UpdateColliderFromBone("boss_Shout", m_pShoutCollider, m_ShoutBoneWorldTransform, true, rotOffset);
        }
    }

    // 衝突処理
    HandleParryDetection();
    HandleDamageDetection();
    HandleAttackDetection();
    HandleDodgeDetection();
}

void Boss::Draw()
{
	MeshObject::Draw();
	m_State->Draw();

    // ImGui: コライダー回転オフセット（度数法、X/Y/Z）
#if _DEBUG
    if (ImGui::Begin(IMGUI_JP("Boss Collider Offsets"))) {
        ImGui::Text(IMGUI_JP("回転オフセット (度) - X:Pitch, Y:Yaw, Z:Roll"));
        ImGui::DragFloat3(IMGUI_JP("Slash Rot (deg)"), &m_SlashRotOffsetDeg.x, 1.0f, -180.0f, 180.0f);
        ImGui::DragFloat3(IMGUI_JP("Stomp Rot (deg)"), &m_StompRotOffsetDeg.x, 1.0f, -180.0f, 180.0f);
        ImGui::DragFloat3(IMGUI_JP("Shout Rot (deg)"), &m_ShoutRotOffsetDeg.x, 1.0f, -180.0f, 180.0f);

        // 現在のワールド回転（度）を表示
        DirectX::XMFLOAT3 slashRot = m_SlashBoneWorldTransform.GetRotationDegrees();
        DirectX::XMFLOAT3 stompRot = m_StompBoneWorldTransform.GetRotationDegrees();
        DirectX::XMFLOAT3 shoutRot = m_ShoutBoneWorldTransform.GetRotationDegrees();
        ImGui::Separator();
        ImGui::Text(IMGUI_JP("Current Collider Rotation (deg)"));
        ImGui::Text(IMGUI_JP("Slash:  X=%.1f  Y=%.1f  Z=%.1f"), slashRot.x, slashRot.y, slashRot.z);
        ImGui::Text(IMGUI_JP("Stomp:  X=%.1f  Y=%.1f  Z=%.1f"), stompRot.x, stompRot.y, stompRot.z);
        ImGui::Text(IMGUI_JP("Shout:  X=%.1f  Y=%.1f  Z=%.1f"), shoutRot.x, shoutRot.y, shoutRot.z);

        ImGui::End();
    }
#endif
}

void Boss::Init()
{
}

StateMachine<Boss>* Boss::GetStateMachine()
{
	return m_State.get();
}

LPD3DXANIMATIONCONTROLLER Boss::GetAnimCtrl() const
{
	return m_pAnimCtrl;
}

void Boss::Hit()
{
	//ボスの体力の最小値.
	constexpr float zero = 0.0f;
	//ボスがPlayerからの攻撃を受けるダメージ変数.
	//このダメージは今は仮でおいているだけです
	//通常攻撃.
	constexpr float ten = 10.0f;
	//必殺技.
	constexpr float twenty = 20.0f;
	//ジャスト回避時の攻撃.
	constexpr float Five = 5.0f;
	//パリィの時の与えるダメージ.
	constexpr float Fifteen = 15.0f;

	//Bossの体力でのステートにいれる.
	constexpr float Dead_HP = zero;


	//いったんこの10ダメだけにしておく.
	//最後はTenをBaseにして+や-を使用する感じになると思っている.
	m_HitPoint -= ten;
	if (m_HitPoint <= 0.0f)
	{
		//死んだときにDeadStateclassに入る.
		m_State->ChangeState(std::make_shared<BossDeadState>(this));
	}

	Update();
}

void Boss::SetTargetPos(const DirectX::XMFLOAT3 Player_Pos)
{
	m_PlayerPos = Player_Pos;
}


// 衝突_被ダメージ.
void Boss::HandleDamageDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Enemy_Damage) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();

			if ((other_group & eCollisionGroup::Player_Attack) != eCollisionGroup::None)
			{
				// ダメージを適用 
				ApplyDamage(info.AttackAmount);

				Time::GetInstance().SetWorldTimeScale(0.1f, 0.016f * 5);
				CameraManager::GetInstance().ShakeCamera(0.1f, 2.5f); // カメラを少し揺らす.

				// 1フレームに1回.
				return;
			}
		}
	}
}

void Boss::HandleAttackDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Enemy_Attack) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();

			if ((other_group & eCollisionGroup::Player_Damage) != eCollisionGroup::None)
			{
				SetAttackColliderActive(false);

				// 1フレームに1回.
				return;
			}
		}
	}
}

void Boss::HandleDodgeDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Player_JustDodge) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();

			if ((other_group & eCollisionGroup::Player_Attack) != eCollisionGroup::None)
			{
				//Parry();
				// 1フレームに1回.
				return;
			}
		}
	}
}

void Boss::HandleParryDetection()
{
	if (!m_upColliders) return;

	const auto& internal_colliders = m_upColliders->GetInternalColliders();

	for (const auto& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();

		if ((current_collider->GetMyMask() & eCollisionGroup::Enemy_Attack) == eCollisionGroup::None) {
			continue;
		}

		for (const CollisionInfo& info : current_collider->GetCollisionEvents())
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			eCollisionGroup other_group = otherCollider->GetMyMask();

			if ((other_group & eCollisionGroup::Player_Parry) != eCollisionGroup::None)
			{
				// 別ステートへ遷移させる（共通のパリィステート）
				m_pAttackCollider->SetActive(false);
				m_State->ChangeState(std::make_shared<BossParryState>(this));

				// 一フレーム1回.
				return;
			}
		}
	}
}

ColliderBase* Boss::GetSlashCollider() const
{
	return m_pSlashCollider;
}

ColliderBase* Boss::GetStompCollider() const
{
	return m_pStompCollider;
}

ColliderBase* Boss::GetShoutCollider() const
{
	return m_pShoutCollider;
}

void Boss::SetColliderActiveByName(const std::string& name, bool active)
{
	// NOTE: 文字列は typo を避けるため定数化推奨
	if (name == "boss_Hand_R")
	{
		if (auto* col = GetSlashCollider()) col->SetActive(active);
		return;
	}
	if (name == "boss_pSphere28")
	{
		if (auto* col = GetStompCollider()) col->SetActive(active);
		return;
	}
	if (name == "boss_Shout")
	{
		if (auto* col = GetShoutCollider()) col->SetActive(active);
		return;
	}
}

bool Boss::UpdateColliderFromBone(
    const std::string& boneName,
    ColliderBase* collider,
    Transform& outTransform,
    bool updateRotation,
    const DirectX::XMFLOAT4& rotationOffset)
{


    if (!collider || GetAttachMesh().expired()) return false;
    auto skinMesh = std::dynamic_pointer_cast<SkinMesh>(GetAttachMesh().lock());
    if (!skinMesh) return false;

    DirectX::XMMATRIX bone_local_matrix;
    if (!skinMesh->GetMatrixFromBone(boneName.c_str(), &bone_local_matrix))
    {
        DirectX::XMFLOAT3 boneWorldPos{};
        if (!skinMesh->GetPosFromBone(boneName.c_str(), &boneWorldPos)) return false;
        outTransform.Position = boneWorldPos;
        DirectX::XMMATRIX bossWorldMatrix = m_spTransform->GetWorldMatrix();
        if (updateRotation) {
            DirectX::XMVECTOR b_pos, b_quat, b_scale;
            DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, bossWorldMatrix);
            // apply rotation offset if provided
            DirectX::XMVECTOR q_bone = b_quat;
            DirectX::XMVECTOR q_offset = DirectX::XMLoadFloat4(&rotationOffset);
            DirectX::XMVECTOR q_result = DirectX::XMQuaternionMultiply(q_offset, q_bone);
            DirectX::XMStoreFloat4(&outTransform.Quaternion, q_result);
            DirectX::XMStoreFloat3(&outTransform.Scale, b_scale);
            outTransform.UpdateRotationFromQuaternion();
        } else {
            outTransform.Quaternion = DirectX::XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f};
            outTransform.Scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};
        }
        return true;
    }

    DirectX::XMMATRIX boss_world_matrix = m_spTransform->GetWorldMatrix();
    DirectX::XMMATRIX bone_world_matrix = bone_local_matrix * boss_world_matrix;

    DirectX::XMVECTOR v_final_pos, v_final_quat, v_final_scale;
    DirectX::XMMatrixDecompose(&v_final_scale, &v_final_quat, &v_final_pos, bone_world_matrix);
    DirectX::XMStoreFloat3(&outTransform.Position, v_final_pos);
    if (updateRotation) {
        // apply rotation offset
        DirectX::XMVECTOR q_bone = v_final_quat;
        DirectX::XMVECTOR q_offset = DirectX::XMLoadFloat4(&rotationOffset);
        DirectX::XMVECTOR q_result = DirectX::XMQuaternionMultiply(q_offset, q_bone);
        DirectX::XMStoreFloat4(&outTransform.Quaternion, q_result);
        DirectX::XMStoreFloat3(&outTransform.Scale, v_final_scale);
        outTransform.UpdateRotationFromQuaternion();
    }

    // compute relative position offset for collider
    DirectX::XMVECTOR b_pos, b_quat, b_scale;
    DirectX::XMMatrixDecompose(&b_scale, &b_quat, &b_pos, boss_world_matrix);
    DirectX::XMVECTOR relative_pos = DirectX::XMVectorSubtract(v_final_pos, b_pos);
    DirectX::XMFLOAT3 f_relative_pos; DirectX::XMStoreFloat3(&f_relative_pos, relative_pos);
    collider->SetPositionOffset(f_relative_pos.x, f_relative_pos.y, f_relative_pos.z);



    return true;
}
