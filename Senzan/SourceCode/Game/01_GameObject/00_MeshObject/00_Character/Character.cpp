#include "Character.h"
#include "Game/03_Collision/00_Core/ColliderBase.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"
#include "Game/03_Collision/00_Core/00_Box/BoxCollider.h"
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"
#include "Game/03_Collision/00_Core/02_Sphere/SphereCollider.h"

Character::Character()
	: MeshObject    ()
	, m_upColliders ()
	, m_MaxHP       (100.f)
	, m_HP          (100.f)
{
	m_upColliders = std::make_unique<CompositeCollider>();
}

Character::~Character()
{
}

void Character::Update()
{
	m_upColliders->SetDebugInfo();
}

void Character::LateUpdate()
{
	HandleCollisionResponse();
}

void Character::Draw()
{
	MeshObject::Draw();
}

// 衝突応答処理.
void Character::HandleCollisionResponse()
{
	if (!m_upColliders) return;

	// このオブジェクトの当たり判定を取得.
	const  std::vector<std::unique_ptr<ColliderBase>>& internal_colliders = m_upColliders->GetInternalColliders();

	// 当たり判定を走査.
	for (const std::unique_ptr<ColliderBase>& collider_ptr : internal_colliders)
	{
		const ColliderBase* current_collider = collider_ptr.get();
		if (!current_collider) { continue; }

		// 自分がPress属性出ないならreturn.
		if ((current_collider->GetMyMask() & eCollisionGroup::Press) == eCollisionGroup::None) {
			continue;
		}

		// 衝突イベントリストを取得.
		const std::vector<CollisionInfo>& events = current_collider->GetCollisionEvents();

		// イベントを走査.
		for (const CollisionInfo& info : events)
		{
			if (!info.IsHit) continue;
			const ColliderBase* otherCollider = info.ColliderB;
			if (!otherCollider) { continue; }

			// 相手のグループが Press であるか (このPressグループとの衝突のみを処理する).
			if ((otherCollider->GetMyMask() & eCollisionGroup::BossPress) == eCollisionGroup::None) { continue; }

			if (info.PenetrationDepth > 0.0f)
			{
				DirectX::XMVECTOR v_normal = DirectX::XMLoadFloat3(&info.Normal);
				// yは無視する.
				v_normal = DirectX::XMVectorSetY(v_normal, 0.0f);

				// 補正量 = 法線 * penetrationDepth.
				DirectX::XMVECTOR v_correction = DirectX::XMVectorScale(v_normal, info.PenetrationDepth);

				// y成分を0にする.
				v_correction = DirectX::XMVectorSetY(v_correction, 0.0f);

				DirectX::XMFLOAT3 correction = {};
				DirectX::XMStoreFloat3(&correction, v_correction);

				// 位置を加算して押し返す.
				AddPosition(correction);
			}
		}
	}
}

void Character::ApplyDamage(float damageAmount)
{
	// すでに死亡している場合は何もしない
	if (0.f >= m_HP) return;

	// 無敵時間中ならダメージを受け付けない (必要であれば)
	// if (m_InvincibleTimer > 0.0f) return;

	// HPを減らす
	m_HP -= damageAmount;
	// 修正後：0.0f と m_MaxHP の間に収める
	m_HP = std::clamp(m_HP - damageAmount, 0.0f, m_MaxHP);

	// HPが0以下になったら死亡処理
	if (m_HP <= 0.0f)
	{
		m_HP = 0.0f;

		// 死亡ステートへ（まだ作っていなければログやフラグのみでもOK）
		// ChangeState(PlayerState::eID::Die); 
		Log::GetInstance().Info("Player", "Player is Dead.");
	}
	else
	{
		// 被ダメージ時の共通処理（SE再生や一瞬だけ赤く光らせるフラグなど）

		// 無敵時間の設定 (例: 1.0秒間無敵)
		// m_InvincibleTimer = 1.0f; 
	}
}
