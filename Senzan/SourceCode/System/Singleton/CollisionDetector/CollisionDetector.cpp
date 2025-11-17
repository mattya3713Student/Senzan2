#include "CollisionDetector.h"
#include "Game/03_Collision/ColliderBase.h" 

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
    for (size_t i = 0; i < m_Colliders.size(); ++i)
    {
        for (size_t j = i + 1; j < m_Colliders.size(); ++j)
        {
            ColliderBase* colliderA = m_Colliders[i].get();
            ColliderBase* colliderB = m_Colliders[j].get();

            if (!colliderA || !colliderB) { continue; }

            // フィルタの判断と接触の判断を行う.
            CollisionInfo info = colliderA->CheckCollision(*colliderB);

			// 当たってたら情報を記録して伝達.
            if (!info.IsHit) { continue; }

            // Detector側の記録リストに追加.
            m_PendingResponses.push_back(info);
            
            // Collider A への情報追加.
            colliderA->AddCollisionInfo(info);

            // Collider B への情報追加.
            CollisionInfo info_reverse = info;

            // 法線ベクトルを反転.
            info_reverse.Normal = DirectX::XMVectorNegate(info.Normal);

            // ポインタを入れ替え.
            const ColliderBase* temp_collider = info.ColliderA;
            info_reverse.ColliderA = info.ColliderB;
            info_reverse.ColliderB = temp_collider;

            // 接触点はワールド座標なのでそのまま使用.
            colliderB->AddCollisionInfo(info_reverse);

        }
    }
}

// コライダーの登録.
void CollisionDetector::RegisterCollider(std::shared_ptr<ColliderBase> collider)
{
    if (collider)
    {
        m_Colliders.push_back(collider);
    }
}

// コライダーの解除.
void CollisionDetector::UnregisterCollider(std::shared_ptr<ColliderBase> collider)
{
    auto it = std::remove(m_Colliders.begin(), m_Colliders.end(), collider);
    m_Colliders.erase(it, m_Colliders.end());
}
