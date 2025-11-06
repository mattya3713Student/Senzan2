#include "CollisionDetector.h"
#include "Game/03_Collision/ColliderBase.h" 

void CollisionDetector::ExecuteCollisionDetection()
{
    // 前フレームの情報をクリア.
    for (const auto& collider : m_colliders)
    {
        if (collider)
        {
            collider->ClearCollisionEvents();
        }
    }
    m_pendingResponses.clear();

    // 2. コライダーリストの総当たりチェック
    for (size_t i = 0; i < m_colliders.size(); ++i)
    {
        for (size_t j = i + 1; j < m_colliders.size(); ++j)
        {
            ColliderBase* colliderA = m_colliders[i].get();
            ColliderBase* colliderB = m_colliders[j].get();

            if (!colliderA || !colliderB) { continue; }

            // CheckCollision は A -> B の視点で情報を計算し、ポインタを A:this, B:&other に修正済み。
            CollisionInfo info = colliderA->CheckCollision(*colliderB);

            if (!info.IsHit) { continue; }
			// 当たってたら情報を記録して伝達.

            // 3. 衝突情報の記録と伝達

            // Detector側の記録リストに追加
            m_pendingResponses.push_back(info);

            // Collider A への情報追加 (A -> B の視点)
            // info は CheckCollision 内で A視点に修正されているため、そのまま A に追加
            colliderA->AddCollisionInfo(info);

            // Collider B への情報追加 (B -> A の視点)
            // Bから見ると、法線は逆方向 (-Normal) となり、AとBのポインタが逆転する。
            CollisionInfo info_reverse = info;

            // 法線ベクトルを反転 (BをAから押し出す Normal を、AをBから押し出す -Normal に変換)
            info_reverse.Normal = DirectX::XMVectorNegate(info.Normal);

            // ポインタを入れ替え (AをBに、BをAに)
            const ColliderBase* temp_collider = info.ColliderA;
            info_reverse.ColliderA = info.ColliderB;
            info_reverse.ColliderB = temp_collider;

            // 接触点はワールド座標なのでそのまま使用

            colliderB->AddCollisionInfo(info_reverse);

        }
    }
}

// コライダーの登録.
void CollisionDetector::RegisterCollider(std::shared_ptr<ColliderBase> collider)
{
    if (collider)
    {
        m_colliders.push_back(collider);
    }
}

// コライダーの解除.
void CollisionDetector::UnregisterCollider(std::shared_ptr<ColliderBase> collider)
{
    auto it = std::remove(m_colliders.begin(), m_colliders.end(), collider);
    m_colliders.erase(it, m_colliders.end());
}
