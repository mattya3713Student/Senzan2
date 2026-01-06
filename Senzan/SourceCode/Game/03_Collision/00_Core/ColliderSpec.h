#pragma once

#include <cstdint>
#include <DirectXMath.h>
#include "ColliderBase.h"

// コライダー生成時の仕様(struct).
struct ColliderSpec
{
    // 形状パラメータ.
    float Radius = 1.0f;
    float Height = 1.0f;

    // オフセット.
    DirectX::XMFLOAT3 Offset = {0.0f, 0.0f, 0.0f};

    // 攻撃力を入れる.
    // TODO : 当たり判定間で数字のやり取りがしやすいようにするための仕組みを検討.
    float AttackAmount = 0.0f;

    // マスク: uint32_t で保持 (eCollisionGroup のビットを格納する)
    uint32_t MyMask = 0;
    uint32_t TargetMask = 0;

    // デバッグ色.
    DirectX::XMFLOAT4 DebugColor = {1.0f, 0.0f, 0.0f, 1.0f};

    // 初期アクティブフラグ.
    bool Active = false;
};
