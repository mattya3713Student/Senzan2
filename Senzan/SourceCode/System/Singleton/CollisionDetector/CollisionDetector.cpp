#include "CollisionDetector.h"
#include "Game/03_Collision/00_Core/ColliderBase.h" 
#include "Game/03_Collision/00_Core/Ex_CompositeCollider/CompositeCollider.h" 

void CollisionDetector::ExecuteCollisionDetection()
{
    // 前フレームの情報をクリア.
    for (const auto& collider : m_Colliders)
    {
        if (collider)
        {
            collider->ClearCollisionEvents();
        }
    }
    m_PendingResponses.clear();

    // 2. コライダーリストの総当たりチェック
    const size_t n = m_Colliders.size();
    if (n < 2) return;

    // 事前に必要量を確保して再割り当てを減らす
    m_PendingResponses.reserve(std::min<size_t>(m_PendingResponses.capacity() + 16, n * 2));

    // アクティブなコライダーのみのインデックスを作る（小さいループの方がキャッシュに優しい）
    std::vector<ColliderBase*> activeColliders;
    activeColliders.reserve(n);
    for (ColliderBase* c : m_Colliders) {
        if (c && c->GetActive()) activeColliders.push_back(c);
    }

    const size_t activeCount = activeColliders.size();
    for (size_t i = 0; i < activeCount; ++i)
    {
        // jはi+1から開始
        for (size_t j = i + 1; j < activeCount; ++j)
        {
            ColliderBase* colliderA = activeColliders[i];
            ColliderBase* colliderB = activeColliders[j];
            if (!colliderA || !colliderB) { continue; }

            // フィルタの判断と接触の判断.
            CollisionInfo info = colliderA->CheckCollision(*colliderB);
            if (!info.IsHit) { continue; }

            info.AttackAmount = colliderB->GetAttackAmount();

            // Detector側の記録リストに追加.
            m_PendingResponses.push_back(info);

            // Collider A への情報追加.
            colliderA->AddCollisionInfo(info);

            // Collider B への情報追加.
            CollisionInfo info_reverse = info;

            // ポインタを入れ替え.
            const ColliderBase* temp_collider = info.ColliderA;
            info_reverse.ColliderA = info.ColliderB;
            info_reverse.ColliderB = temp_collider; 
            info_reverse.AttackAmount = colliderA->GetAttackAmount();

            // 接触点はワールド座標なのでそのまま使用.
            colliderB->AddCollisionInfo(info_reverse);
        }
    }
}

// コライダーの登録.
void CollisionDetector::RegisterCollider(ColliderBase& Collider)
{
    m_Colliders.push_back(&Collider);
}

// コライダーの登録.
void CollisionDetector::RegisterCollider(const CompositeCollider& Collider)
{
    const std::vector<std::unique_ptr<ColliderBase>>& internal_colliders = Collider.GetInternalColliders();

    for (const std::unique_ptr<ColliderBase>& collider_ptr : internal_colliders)
    {
        if (collider_ptr)
        {
            m_Colliders.push_back(collider_ptr.get());
        }
    }
}

// コライダーの解除.
void CollisionDetector::UnregisterCollider(ColliderBase* Collider)
{
    auto it = std::remove(m_Colliders.begin(), m_Colliders.end(), static_cast<ColliderBase*>(Collider));
    m_Colliders.erase(it, m_Colliders.end());
}

// コライダーの解除.
void CollisionDetector::UnregisterCollider(const CompositeCollider& Collider)
{
    const std::vector<std::unique_ptr<ColliderBase>>& internal_colliders = Collider.GetInternalColliders();

    for (const std::unique_ptr<ColliderBase>& collider_ptr : internal_colliders)
    {
        if (collider_ptr)
        {
            UnregisterCollider(collider_ptr.get());
        }
    }
}
