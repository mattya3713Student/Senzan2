#pragma once
#include "ColliderBase.h"

/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2025/11/6.
* @brief     : 衝突判定の実行役.
* @note      : ColliderBase の friend クラスとして DispatchCollision を実行する.
**********************************************************************************/
class CollisionDetector
{
public:
    // 全ての衝突判定の入り口.
    static bool CheckCollision(const ColliderBase& colliderA, const ColliderBase& colliderB)
    {
        // フィルタリングチェック.
        if (!colliderA.ShouldCollide(colliderB)) {
            return false;
        }

        // 二重ディスパッチの開始.
        return colliderB.DispatchCollision(colliderA);
    }
};