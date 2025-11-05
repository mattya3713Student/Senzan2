#pragma once
#include "System/Singleton/Debug/Log/DebugLog.h"

#include "../ColliderBase.h"

class CollisionDetector;

/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2025/10/5.
* @brief     : カプセルコリジョン.
**********************************************************************************/

class CapsuleCollider final
	: public ColliderBase
{
public:
	CapsuleCollider();
	CapsuleCollider(std::weak_ptr<const Transform> parentTransform);
	~CapsuleCollider() override;

	void Update() override;


	// 半径の取得.
	inline float GetRadius() const noexcept override { return m_Radius; }

	// 高さを取得する.
	inline float GetHeight() const noexcept override { return m_Height; }

	// 半径を設定する.
	inline void SetRadius(float Radius) noexcept override { m_Radius = Radius; }
	// 高さを設定する.
	inline void SetHeight(float Height) noexcept override { m_Height = Height; }

	// 衝突判定.
	bool CheckCollision(const ColliderBase& other) const override;

protected:
	bool DispatchCollision(const SphereCollider& other) override;
	bool DispatchCollision(const CapsuleCollider& other) override;
	bool DispatchCollision(const BoxCollider& other) override;

private:
	float m_Radius; // 半径.
	float m_Height;	// 高さ.

#if _DEBUG
public:

	// デバッグ描画用設定.
	void SetDebugInfo() override;
#endif // _DEBUG.

};
