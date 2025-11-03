#pragma once

/**********************************************************************************
* @author   : 淵脇 未来.
* @date     : 2025/09/08.
* @brief    : 八分木やAABBなどで効率化を図った当たり判定システム.
**********************************************************************************/

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <array>
#include <utility>
#include <cassert>
#include <algorithm>

// 前方宣言
class ColliderBase;
class SphereCollider;
class BoxCollider;
class CapsuleCollider;

class CollisionSystem final
{
public:
	CollisionSystem();
	~CollisionSystem();

    void Update(
        const std::vector<std::shared_ptr<Transform>>& transforms,
        const std::vector<std::shared_ptr<ColliderBase>>& colliders
    );

private:

    // AABBを更新するヘルパー関数.
    void UpdateAABBFromSphereCollider(const std::shared_ptr<Transform>& transform, const std::shared_ptr<SphereCollider>& collider);
    void UpdateAABBFromBoxCollider(const std::shared_ptr<Transform>& transform, const std::shared_ptr<BoxCollider>& collider);
    void UpdateAABBFromCapsuleCollider(const std::shared_ptr<Transform>& transform, const std::shared_ptr<CapsuleCollider>& collider);

    // 衝突判定を実行する関数.
    bool HandleCollision(
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<ColliderBase>& colliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<ColliderBase>& colliderB
    );
    bool HandleSphereCollisionWithOthers(
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<ColliderBase>& colliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<ColliderBase>& colliderB
    );
    bool HandleBoxCollisionWithOthers(
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<ColliderBase>& colliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<ColliderBase>& colliderB
    );
    bool HandleCapsuleCollisionWithOthers(
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<ColliderBase>& colliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<ColliderBase>& colliderB
    );

    // 衝突判定の詳細関数.
    bool IsSphereCollisionWithSphere(
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<SphereCollider>& colliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<SphereCollider>& colliderB
    );
    bool IsBoxCollisionWithBox(
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<BoxCollider>& colliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<BoxCollider>& colliderB
    );
    bool IsCapsuleCollisionWithCapsule(
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<CapsuleCollider>& colliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<CapsuleCollider>& colliderB
    );
    bool IsSepartedAxis(
        const DirectX::XMVECTOR& axis,
        const std::shared_ptr<Transform>& transformA,
        const std::shared_ptr<BoxCollider>& boxColliderA,
        const std::shared_ptr<Transform>& transformB,
        const std::shared_ptr<BoxCollider>& boxColliderB,
        const DirectX::XMVECTOR& d
    );
    float SegmentSegmentDistance(
        const DirectX::XMVECTOR& vP1,
        const DirectX::XMVECTOR& vQ1,
        const DirectX::XMVECTOR& vP2,
        const DirectX::XMVECTOR& vQ2
    );

    // ヘルパー関数.
    DirectX::XMVECTOR CalculateOffsetPosition(
        const DirectX::XMMATRIX& worldMatrix,
        const DirectX::XMFLOAT3& offset
    );
    std::array<DirectX::XMVECTOR, 2> CalculateCapuleEnds(
        const std::shared_ptr<Transform>& transform,
        const std::shared_ptr<CapsuleCollider>& capsuleCollider
    );

};