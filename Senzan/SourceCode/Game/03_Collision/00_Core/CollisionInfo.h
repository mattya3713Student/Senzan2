#pragma once

#include <DirectXMath.h>

class ColliderBase;

// 衝突結果構造体 (response)
struct CollisionInfo
{
    // 衝突が発生したかどうか.
    bool IsHit = false;

    // 衝突法線.
    DirectX::XMFLOAT3 Normal = {};

    // めり込みの深さ.
    float PenetrationDepth = 0.0f;

    // 接触点（ワールド座標）
    DirectX::XMFLOAT3 ContactPoint = {};

    // 攻撃力を入れる.
    // TODO : 当たり判定間で数字のやり取りがしやすいようにするための仕組みを検討.
    float AttackAmount = 0.0f;

    // 衝突に関わったコライダーへのポインタ.
    const ColliderBase* ColliderA = nullptr;
    const ColliderBase* ColliderB = nullptr;
};

