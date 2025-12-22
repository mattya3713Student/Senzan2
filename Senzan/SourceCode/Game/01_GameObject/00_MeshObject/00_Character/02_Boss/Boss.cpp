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

#include "Resource/Mesh/02_Skin/SkinMesh.h"

#include "Game/01_GameObject/00_MeshObject/00_Character/02_Boss/BossDeadState/BossDeadState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossChargeState/BossChargeState.h"

#include "00_MeshObject/00_Character/02_Boss/BossAttackStateBase/BossThrowingState/BossThrowingState.h"

#include "System/Singleton/CollisionDetector/CollisionDetector.h"
#include "System/Singleton/CameraManager/CameraManager.h"


constexpr float HP_Max = 100.0f;

Boss::Boss()
	: Character()
	, m_State(std::make_unique<StateMachine<Boss>>(this))
	, m_PlayerPos{}
	, m_TurnSpeed(0.1f)
	, m_MoveSpeed(0.3f)
	, m_vCurrentMoveVelocity(0.f, 0.f, 0.f)
	, deleta_time(0.f)
	, m_HitPoint(0.0f)
{
	AttachMesh(MeshManager::GetInstance().GetSkinMesh("boss"));

	//DirectX::XMFLOAT3 pos = { 0.05f, 10.0f, 20.05f };
	DirectX::XMFLOAT3 pos = { 0.05f, 0.05f, 20.05f };
	DirectX::XMFLOAT3 scale = { 10.0f, 10.0f, 10.0f };
	DirectX::XMFLOAT3 Rotation = { 0.0f,0.0f,0.0f };
	m_spTransform->SetPosition(pos);
	m_spTransform->SetScale(scale);
	m_spTransform->SetRotationDegrees(Rotation);

	m_MaxHP = 100.f;
	m_HP = m_MaxHP;

	// ステートマシンの初期ステートを、SlashChargeStateに設定
	//Idolに遷移させるんだけど
	//アニメーションの再生系統を今日するのでここを変更していく.
	//m_State->ChangeState(std::make_shared<BossStompState>(this));
	m_State->ChangeState(std::make_shared<BossIdolState>(this));
	//m_State->ChangeState(std::make_shared<BossShoutState>());


	// 攻撃の追加.
	std::unique_ptr<CapsuleCollider> attackCollider = std::make_unique<CapsuleCollider>(m_spTransform);

	m_pAttackCollider = attackCollider.get();

	attackCollider->SetActive(false);
	attackCollider->SetColor(Color::eColor::Red);
	attackCollider->SetAttackAmount(5.0f);
	attackCollider->SetHeight(30.0f);
	attackCollider->SetRadius(10.0f);
	attackCollider->SetPositionOffset(0.f, 1.5f, -20.f);
	attackCollider->SetMyMask(eCollisionGroup::Enemy_Attack);
	attackCollider->SetTarGetTargetMask(eCollisionGroup::Player_Damage | eCollisionGroup::Player_Dodge | eCollisionGroup::Player_JustDodge | eCollisionGroup::Player_Parry);

	m_upColliders->AddCollider(std::move(attackCollider));

	//攻撃動作の確認用のために書いている.
	//m_State->ChangeState(std::make_shared<BossStompState>(this));

	//ボスの最大体力.
	m_HitPoint = HP_Max;

	//動きをゆっくりにするコード.
	//コメントアウトすることで速度がもとに戻る.
	m_TimeScale = 0.1;

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
	press_collider->SetMyMask(eCollisionGroup::Press);
	press_collider->SetTarGetTargetMask(eCollisionGroup::Press);

	m_upColliders->AddCollider(std::move(press_collider));

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
		Hit();
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

	// 衝突イベント処理を実行
	HandleParryDetection();
	HandleDamageDetection();
	HandleAttackDetection();
	HandleDodgeDetection();
}

void Boss::Draw()
{
	MeshObject::Draw();
	m_State->Draw();

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

				Time::GetInstance().SetWorldTimeScale(0.1f, 0.016f * 8);
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
				auto* currentState = m_State->m_pOwner; // 現在の状態を取得
				if (BossAttackStateBase* attackState = dynamic_cast<BossAttackStateBase*>(currentState))
				{
					// 現在のステートが BossAttackStateBase だった場合のみ実行される
					attackState->ParryTime();
					return;
				}

				m_pAttackCollider->SetActive(false);

				// 一フレーム1回.
				return;
			}
		}
	}
}
