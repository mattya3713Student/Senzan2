#include "Character.h"
#include "Game/03_Collision/ColliderBase.h"
#include "Game/03_Collision/Box/BoxCollider.h"
#include "Game/03_Collision/Capsule/CapsuleCollider.h"
#include "Game/03_Collision/Sphere/SphereCollider.h"

Character::Character()
	: MeshObject()
	, m_pPressCollider(std::make_shared<CapsuleCollider>(m_Transform))
{
	m_pPressCollider->SetColor(Color::eColor::Cyan);
	m_pPressCollider->SetPositionOffset(0.f, 50.f, 0.f);
	m_pPressCollider->SetRadius(50.f);
	m_pPressCollider->SetHeight(50.f);
}

Character::~Character()
{
}

void Character::Update()
{
}

void Character::LateUpdate()
{
    HandleCollisionResponse();
}

void Character::Draw()
{
	MeshObject::Draw();
	m_pPressCollider->SetDebugInfo();
}

// 衝突応答処理.
void Character::HandleCollisionResponse()
{
    if (!m_pPressCollider) return;

    // 衝突イベントリストを取得.
    const auto events = m_pPressCollider->GetCollisionEvents();

    // ターゲットグループのビットマスクを定義.
    constexpr uint32_t PRESS_GROUP = (uint32_t)ColliderBase::eCollisionGroup::Press;

    for (const auto& info : events)
    {
        if (!info.IsHit) continue;
        const ColliderBase* otherCollider = info.ColliderB;
        if (!otherCollider) { continue; }

        // 相手のグループが Press であるか (このPressグループとの衝突のみを処理する)
        if ((otherCollider->GetGroup() & PRESS_GROUP) == 0) { continue; }

        if (info.PenetrationDepth > 0.0f)
        {
            // 押し戻しベクトル = Normal * Depth
            // info.Normal は A (自分) を B (相手) から押し出す方向
            DirectX::XMVECTOR v_correction = DirectX::XMVectorScale(info.Normal, info.PenetrationDepth);
            DirectX::XMFLOAT3 correction = {};
            DirectX::XMStoreFloat3(&correction , v_correction);

			AddPosition(correction);
        }
    }
}
