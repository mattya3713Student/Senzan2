#include "CollisionSystem.h"
#include "System/Utility/Transform/Transform.h"
#include "Game/03_Collision/ColliderBase.h"
#include "Game/03_Collision/Capsule/CapsuleCollider.h"
#include "Game/03_Collision/Sphere/SphereCollider.h"
#include "Game/03_Collision/Box/BoxCollider.h"

//---------------------------------------------------------------------------------------------------------------------------------------.

CollisionSystem::CollisionSystem()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------.

CollisionSystem::~CollisionSystem()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------.

void CollisionSystem::Update(
    const std::vector<std::shared_ptr<Transform>>& transforms,
    const std::vector<std::shared_ptr<ColliderBase>>& colliders)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------.

void CollisionSystem::UpdateAABBFromSphereCollider(
    const std::shared_ptr<Transform>& transform,
    const std::shared_ptr<SphereCollider>& collider)
{
    assert(transform && collider);

    // オフセット位置をワールド空間に変換
    // 【修正】GetPositionOffset() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 offset = collider->GetPositionOffset();
    DirectX::XMVECTOR v_offset_position = CalculateOffsetPosition(
        transform->GetWorldMatrix(), offset);
    DirectX::XMFLOAT3 offset_position;
    DirectX::XMStoreFloat3(&offset_position, v_offset_position);

    // スフィアのスケールを計算
    const DirectX::XMFLOAT3& scale = transform->Scale;
    float max_scale = (std::max)({ scale.x, scale.y, scale.z });
    float scaled_radius = collider->GetRadius() * max_scale;

    // AABBの最小・最大位置を計算
    DirectX::XMFLOAT3 min, max;
    min = { offset_position.x - scaled_radius, offset_position.y - scaled_radius, offset_position.z - scaled_radius };
    max = { offset_position.x + scaled_radius, offset_position.y + scaled_radius, offset_position.z + scaled_radius };
}

//---------------------------------------------------------------------------------------------------------------------------------------.

void CollisionSystem::UpdateAABBFromBoxCollider(
    const std::shared_ptr<Transform>& transform,
    const std::shared_ptr<BoxCollider>& collider)
{
    assert(transform && collider);

    DirectX::XMVECTOR vMin = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0.0f);
    DirectX::XMVECTOR vMax = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.0f);

    // ローカルオフセットとワールド行列
    // 【修正】GetPositionOffset() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 localOffset = collider->GetPositionOffset();
    DirectX::XMVECTOR vLocalOffset = DirectX::XMLoadFloat3(&localOffset);
    DirectX::XMMATRIX worldMatrix = transform->GetWorldMatrix();

    // 頂点ごとに処理
    for (const auto& vertex : collider->GetVertices())
    {
        // vertexはループ変数（参照）なので問題なし
        DirectX::XMVECTOR vLocalPos = DirectX::XMLoadFloat3(&vertex);
        DirectX::XMVECTOR vLocalPosWithOffset = DirectX::XMVectorAdd(vLocalPos, vLocalOffset);
        DirectX::XMVECTOR vWorldPos = DirectX::XMVector3Transform(vLocalPosWithOffset, worldMatrix);

        vMin = DirectX::XMVectorMin(vMin, vWorldPos);
        vMax = DirectX::XMVectorMax(vMax, vWorldPos);
    }

    DirectX::XMFLOAT3 min, max;
    DirectX::XMStoreFloat3(&min, vMin);
    DirectX::XMStoreFloat3(&max, vMax);
}

//---------------------------------------------------------------------------------------------------------------------------------------.

void CollisionSystem::UpdateAABBFromCapsuleCollider(
    const std::shared_ptr<Transform>& transform,
    const std::shared_ptr<CapsuleCollider>& collider)
{
    assert(transform && collider);

    // カプセルの始点・終点を計算
    auto capsule_ends = CalculateCapuleEnds(transform, collider);
    DirectX::XMVECTOR v_p0 = capsule_ends[0];
    DirectX::XMVECTOR v_p1 = capsule_ends[1];

    // 最大スケールを取得
    const DirectX::XMFLOAT3& world_scale = transform->Scale;
    float max_scale = (std::max)({ world_scale.x, world_scale.y, world_scale.z });

    // スケール後の半径
    float scaled_radius = collider->GetRadius() * max_scale;
    DirectX::XMVECTOR v_radius = DirectX::XMVectorReplicate(scaled_radius);

    DirectX::XMVECTOR v_min = DirectX::XMVectorSubtract(DirectX::XMVectorMin(v_p0, v_p1), v_radius);
    DirectX::XMVECTOR v_max = DirectX::XMVectorAdd(DirectX::XMVectorMax(v_p0, v_p1), v_radius);

    DirectX::XMFLOAT3 min, max;
    DirectX::XMStoreFloat3(&min, v_min);
    DirectX::XMStoreFloat3(&max, v_max);
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::HandleCollision(
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<ColliderBase>& colliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<ColliderBase>& colliderB)
{
    assert(colliderA && colliderB);

    switch (colliderA->GetShapeType())
    {
        case ColliderBase::ShapeType::Sphere:
            return HandleSphereCollisionWithOthers(transformA, colliderA, transformB, colliderB);
        case ColliderBase::ShapeType::Box:
            return HandleBoxCollisionWithOthers(transformA, colliderA, transformB, colliderB);
        case ColliderBase::ShapeType::Capsule:
            return HandleCapsuleCollisionWithOthers(transformA, colliderA, transformB, colliderB);
        default:
            return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------.

DirectX::XMVECTOR CollisionSystem::CalculateOffsetPosition(
    const DirectX::XMMATRIX& worldMatrix,
    const DirectX::XMFLOAT3& offset)
{
    // offsetはconst参照なので問題なし
    DirectX::XMVECTOR v_offset = DirectX::XMLoadFloat3(&offset);
    DirectX::XMVECTOR v_offset_position = DirectX::XMVector3Transform(v_offset, worldMatrix);

    return v_offset_position;
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::HandleSphereCollisionWithOthers(
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<ColliderBase>& colliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<ColliderBase>& colliderB)
{
    assert(colliderA && colliderB);

    switch (colliderB->GetShapeType())
    {
        case ColliderBase::ShapeType::Sphere:
        {
            auto sphere_a = std::dynamic_pointer_cast<SphereCollider>(colliderA);
            auto sphere_b = std::dynamic_pointer_cast<SphereCollider>(colliderB);
            if (!sphere_a || !sphere_b) {
                return false;
            }
            return IsSphereCollisionWithSphere(transformA, sphere_a, transformB, sphere_b);
        }
        case ColliderBase::ShapeType::Box:
            // TODO: スフィア-ボックス衝突判定を実装
            return false;
        default:
            return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::IsSphereCollisionWithSphere(
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<SphereCollider>& colliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<SphereCollider>& colliderB)
{
    assert(transformA && colliderA && transformB && colliderB);

    // 【修正】transformA->Position の結果をローカル変数に格納
    const DirectX::XMFLOAT3 posA = transformA->Position;
    DirectX::XMVECTOR v_position_a = DirectX::XMLoadFloat3(&posA);
    // 【修正】colliderA->GetPositionOffset() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 offsetA = colliderA->GetPositionOffset();
    DirectX::XMVECTOR v_offset_a = DirectX::XMLoadFloat3(&offsetA);
    DirectX::XMVECTOR v_offset_position_a = DirectX::XMVectorAdd(v_position_a, v_offset_a);

    // 【修正】transformB->Position の結果をローカル変数に格納
    const DirectX::XMFLOAT3 posB = transformB->Position;
    DirectX::XMVECTOR v_position_b = DirectX::XMLoadFloat3(&posB);
    // 【修正】colliderB->GetPositionOffset() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 offsetB = colliderB->GetPositionOffset();
    DirectX::XMVECTOR v_offset_b = DirectX::XMLoadFloat3(&offsetB);
    DirectX::XMVECTOR v_offset_position_b = DirectX::XMVectorAdd(v_position_b, v_offset_b);

    DirectX::XMVECTOR v_diff = DirectX::XMVectorSubtract(v_offset_position_b, v_offset_position_a);

    const DirectX::XMFLOAT3& scale_a = transformA->Scale;
    const DirectX::XMFLOAT3& scale_b = transformB->Scale;

    float max_scale_a = (std::max)({ scale_a.x, scale_a.y, scale_a.z });
    float max_scale_b = (std::max)({ scale_b.x, scale_b.y, scale_b.z });

    float scaled_radius_a = colliderA->GetRadius() * max_scale_a;
    float scaled_radius_b = colliderB->GetRadius() * max_scale_b;

    float distance_sq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(v_diff));
    float radius_sum = scaled_radius_a + scaled_radius_b;

    return distance_sq <= (radius_sum * radius_sum);
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::HandleBoxCollisionWithOthers(
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<ColliderBase>& colliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<ColliderBase>& colliderB)
{
    assert(colliderA && colliderB);

    switch (colliderB->GetShapeType())
    {
        case ColliderBase::ShapeType::Sphere:
            // TODO: ボックス-スフィア衝突判定を実装
            return false;
        case ColliderBase::ShapeType::Box:
        {
            auto box_a = std::dynamic_pointer_cast<BoxCollider>(colliderA);
            auto box_b = std::dynamic_pointer_cast<BoxCollider>(colliderB);
            if (!box_a || !box_b) {
                return false;
            }
            return IsBoxCollisionWithBox(transformA, box_a, transformB, box_b);
        }
        default:
            return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::IsSepartedAxis(
    const DirectX::XMVECTOR& axis,
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<BoxCollider>& boxColliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<BoxCollider>& boxColliderB,
    const DirectX::XMVECTOR& d)
{
    assert(transformA && boxColliderA && transformB && boxColliderB);

    if (DirectX::XMVector3Equal(axis, DirectX::XMVectorZero())) {
        return false;
    }

    DirectX::XMVECTOR normAxis = DirectX::XMVector3Normalize(axis);

    // Aの各軸.
    // 【修正】GetRight(), GetUp(), GetForward() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 a_right = transformA->GetRight();
    DirectX::XMVECTOR axis_a_x = DirectX::XMLoadFloat3(&a_right);
    const DirectX::XMFLOAT3 a_up = transformA->GetUp();
    DirectX::XMVECTOR axis_a_y = DirectX::XMLoadFloat3(&a_up);
    const DirectX::XMFLOAT3 a_forward = transformA->GetForward();
    DirectX::XMVECTOR axis_a_z = DirectX::XMLoadFloat3(&a_forward);

    // Bの各軸.
    // 【修正】GetRight(), GetUp(), GetForward() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 b_right = transformB->GetRight();
    DirectX::XMVECTOR axis_b_x = DirectX::XMLoadFloat3(&b_right);
    const DirectX::XMFLOAT3 b_up = transformB->GetUp();
    DirectX::XMVECTOR axis_b_y = DirectX::XMLoadFloat3(&b_up);
    const DirectX::XMFLOAT3 b_forward = transformB->GetForward();
    DirectX::XMVECTOR axis_b_z = DirectX::XMLoadFloat3(&b_forward);


    const DirectX::XMFLOAT3 size_a = boxColliderA->GetSize();
    const DirectX::XMFLOAT3 size_b = boxColliderB->GetSize();

    const DirectX::XMFLOAT3 scale_a = transformA->Scale;
    const DirectX::XMFLOAT3 scale_b = transformB->Scale;

    DirectX::XMFLOAT3 scaled_half_size_a = {
        size_a.x * scale_a.x * 0.5f,
        size_a.y * scale_a.y * 0.5f,
        size_a.z * scale_a.z * 0.5f
    };
    DirectX::XMFLOAT3 scaled_half_size_b = {
        size_b.x * scale_b.x * 0.5f,
        size_b.y * scale_b.y * 0.5f,
        size_b.z * scale_b.z * 0.5f
    };

    float ra =
        std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis_a_x, normAxis))) * scaled_half_size_a.x +
        std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis_a_y, normAxis))) * scaled_half_size_a.y +
        std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis_a_z, normAxis))) * scaled_half_size_a.z;

    float rb =
        std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis_b_x, normAxis))) * scaled_half_size_b.x +
        std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis_b_y, normAxis))) * scaled_half_size_b.y +
        std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(axis_b_z, normAxis))) * scaled_half_size_b.z;

    float distance = std::abs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, normAxis)));

    return distance > (ra + rb);
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::IsBoxCollisionWithBox(
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<BoxCollider>& colliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<BoxCollider>& colliderB)
{
    assert(transformA && colliderA && transformB && colliderB);

    // 【修正】GetPositionOffset() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 offsetA = colliderA->GetPositionOffset();
    DirectX::XMVECTOR v_offset_a = DirectX::XMLoadFloat3(&offsetA);
    const DirectX::XMFLOAT3 offsetB = colliderB->GetPositionOffset();
    DirectX::XMVECTOR v_offset_b = DirectX::XMLoadFloat3(&offsetB);

    // 【修正】transform->Position の結果をローカル変数に格納
    const DirectX::XMFLOAT3 posA = transformA->Position;
    DirectX::XMVECTOR pos_a = DirectX::XMLoadFloat3(&posA);
    const DirectX::XMFLOAT3 posB = transformB->Position;
    DirectX::XMVECTOR pos_b = DirectX::XMLoadFloat3(&posB);

    DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(
        DirectX::XMVectorAdd(pos_b, v_offset_b),
        DirectX::XMVectorAdd(pos_a, v_offset_a)
    );

    // Aの各軸.
    // 【修正】GetRight(), GetUp(), GetForward() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 a_right = transformA->GetRight();
    DirectX::XMVECTOR axis_a_x = DirectX::XMLoadFloat3(&a_right);
    const DirectX::XMFLOAT3 a_up = transformA->GetUp();
    DirectX::XMVECTOR axis_a_y = DirectX::XMLoadFloat3(&a_up);
    const DirectX::XMFLOAT3 a_forward = transformA->GetForward();
    DirectX::XMVECTOR axis_a_z = DirectX::XMLoadFloat3(&a_forward);
    std::array<DirectX::XMVECTOR, 3> axis_a = std::array{ axis_a_x , axis_a_y , axis_a_z };

    // Bの各軸.
    // 【修正】GetRight(), GetUp(), GetForward() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 b_right = transformB->GetRight();
    DirectX::XMVECTOR axis_b_x = DirectX::XMLoadFloat3(&b_right);
    const DirectX::XMFLOAT3 b_up = transformB->GetUp();
    DirectX::XMVECTOR axis_b_y = DirectX::XMLoadFloat3(&b_up);
    const DirectX::XMFLOAT3 b_forward = transformB->GetForward();
    DirectX::XMVECTOR axis_b_z = DirectX::XMLoadFloat3(&b_forward);
    std::array<DirectX::XMVECTOR, 3> axis_b = std::array{ axis_b_x , axis_b_y , axis_b_z };


    // 15軸で分離をチェック.
    for (int i = 0; i < 3; ++i) {
        if (IsSepartedAxis(axis_a[i], transformA, colliderA, transformB, colliderB, diff)) {
            return false;
        }
        if (IsSepartedAxis(axis_b[i], transformA, colliderA, transformB, colliderB, diff)) {
            return false;
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            DirectX::XMVECTOR cross_axis = DirectX::XMVector3Cross(axis_a[i], axis_b[j]);
            if (IsSepartedAxis(cross_axis, transformA, colliderA, transformB, colliderB, diff)) {
                return false;
            }
        }
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::HandleCapsuleCollisionWithOthers(
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<ColliderBase>& colliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<ColliderBase>& colliderB)
{
    assert(colliderA && colliderB);

    switch (colliderB->GetShapeType())
    {
        case ColliderBase::ShapeType::Sphere:
            // TODO: カプセル-スフィア衝突判定を実装
            return false;
        case ColliderBase::ShapeType::Box:
            // TODO: カプセル-ボックス衝突判定を実装
            return false;
        case ColliderBase::ShapeType::Capsule:
        {
            auto capsule_a = std::dynamic_pointer_cast<CapsuleCollider>(colliderA);
            auto capsule_b = std::dynamic_pointer_cast<CapsuleCollider>(colliderB);
            if (!capsule_a || !capsule_b) {
                return false;
            }
            return IsCapsuleCollisionWithCapsule(transformA, capsule_a, transformB, capsule_b);
        }
        default:
            return false;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------.

std::array<DirectX::XMVECTOR, 2> CollisionSystem::CalculateCapuleEnds(
    const std::shared_ptr<Transform>& transform,
    const std::shared_ptr<CapsuleCollider>& capsuleCollider)
{
    assert(transform && capsuleCollider);

    // 【修正】transform->Position の結果をローカル変数に格納
    const DirectX::XMFLOAT3 pos = transform->Position;
    DirectX::XMVECTOR v_position = DirectX::XMLoadFloat3(&pos);
    // 【修正】capsuleCollider->GetPositionOffset() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 offset = capsuleCollider->GetPositionOffset();
    DirectX::XMVECTOR v_offset = DirectX::XMLoadFloat3(&offset);
    DirectX::XMVECTOR v_offset_position = DirectX::XMVectorAdd(v_position, v_offset);

    // 【修正】transform->GetUp() の結果をローカル変数に格納
    const DirectX::XMFLOAT3 up_axis = transform->GetUp();
    DirectX::XMVECTOR v_up_axis = DirectX::XMLoadFloat3(&up_axis);

    DirectX::XMVECTOR v_scale = DirectX::XMLoadFloat3(&transform->Scale);
    float direction_length = DirectX::XMVectorGetX(DirectX::XMVector3Length(v_scale));

    float cylinder_height = capsuleCollider->GetHeight() - (capsuleCollider->GetRadius() * 2);
    float half_height = (cylinder_height * 0.5f) * direction_length;

    DirectX::XMVECTOR v_offset_end = DirectX::XMVectorScale(v_up_axis, half_height);

    DirectX::XMVECTOR v_p0 = DirectX::XMVectorAdd(v_offset_position, v_offset_end);
    DirectX::XMVECTOR v_p1 = DirectX::XMVectorSubtract(v_offset_position, v_offset_end);

    return { v_p0, v_p1 };
}

//---------------------------------------------------------------------------------------------------------------------------------------.

float CollisionSystem::SegmentSegmentDistance(
    const DirectX::XMVECTOR& vP1,
    const DirectX::XMVECTOR& vQ1,
    const DirectX::XMVECTOR& vP2,
    const DirectX::XMVECTOR& vQ2)
{
    DirectX::XMVECTOR d1 = DirectX::XMVectorSubtract(vQ1, vP1);
    DirectX::XMVECTOR d2 = DirectX::XMVectorSubtract(vQ2, vP2);
    DirectX::XMVECTOR r = DirectX::XMVectorSubtract(vP1, vP2);

    float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d1, d1));
    float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d2, d2));
    float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d2, r));

    float s, t;

    if (a <= 1e-6f && e <= 1e-6f) {
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(r));
    }

    if (a <= 1e-6f) {
        s = 0.0f;
        t = std::clamp(f / e, 0.0f, 1.0f);
    }
    else {
        float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d1, r));
        if (e <= 1e-6f) {
            t = 0.0f;
            s = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else {
            float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d1, d2));
            float denom = a * e - b * b;

            if (denom != 0.0f) {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else {
                s = 0.0f;
            }

            t = (b * s + f) / e;

            if (t < 0.0f) {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f) {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    DirectX::XMVECTOR c1 = DirectX::XMVectorAdd(vP1, DirectX::XMVectorScale(d1, s));
    DirectX::XMVECTOR c2 = DirectX::XMVectorAdd(vP2, DirectX::XMVectorScale(d2, t));
    DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(c1, c2);

    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(diff));
}

//---------------------------------------------------------------------------------------------------------------------------------------.

bool CollisionSystem::IsCapsuleCollisionWithCapsule(
    const std::shared_ptr<Transform>& transformA,
    const std::shared_ptr<CapsuleCollider>& colliderA,
    const std::shared_ptr<Transform>& transformB,
    const std::shared_ptr<CapsuleCollider>& colliderB)
{
    assert(transformA && colliderA && transformB && colliderB);

    std::array<DirectX::XMVECTOR, 2> v_capsule_ends_a = CalculateCapuleEnds(transformA, colliderA);
    std::array<DirectX::XMVECTOR, 2> v_capsule_ends_b = CalculateCapuleEnds(transformB, colliderB);

    float distance_squared = SegmentSegmentDistance(
        v_capsule_ends_a[0], v_capsule_ends_a[1],
        v_capsule_ends_b[0], v_capsule_ends_b[1]);

    const DirectX::XMFLOAT3& scale_a = transformA->Scale;
    const DirectX::XMFLOAT3& scale_b = transformB->Scale;

    float max_scale_a = (std::max)({ scale_a.x, scale_a.y, scale_a.z });
    float max_scale_b = (std::max)({ scale_b.x, scale_b.y, scale_b.z });

    float scaled_radius_a = colliderA->GetRadius() * max_scale_a;
    float scaled_radius_b = colliderB->GetRadius() * max_scale_b;

    float radius_sum = scaled_radius_a + scaled_radius_b;

    return distance_squared <= (radius_sum * radius_sum);
}