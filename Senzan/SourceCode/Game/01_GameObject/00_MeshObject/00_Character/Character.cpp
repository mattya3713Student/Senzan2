#include "Character.h"
#include "Game/03_Collision/00_Core/ColliderBase.h"
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h"
#include "Game/03_Collision/00_Core/00_Box/BoxCollider.h"
#include "Game/03_Collision/00_Core/01_Capsule/CapsuleCollider.h"
#include "Game/03_Collision/00_Core/02_Sphere/SphereCollider.h"

// 押し戻し判定.
constexpr eCollisionGroup PRESS_GROUP = eCollisionGroup::Press;

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
        if ((current_collider->GetMyMask() & PRESS_GROUP) == eCollisionGroup::None) {
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
            if ((otherCollider->GetMyMask() & PRESS_GROUP) == eCollisionGroup::None) { continue; }

            if (info.PenetrationDepth > 0.0f)
            {
                DirectX::XMVECTOR v_normal = DirectX::XMLoadFloat3(&info.Normal);
                DirectX::XMVECTOR v_correction = DirectX::XMVectorScale(v_normal, -info.PenetrationDepth);
                // このゲームにy座標の概念はないのでyは切り捨て.
                v_correction = DirectX::XMVectorSetY(v_correction, 0.0f);
                DirectX::XMFLOAT3 correction = {};
                DirectX::XMStoreFloat3(&correction, v_correction);

                AddPosition(correction);
            }
        }
    }
}
