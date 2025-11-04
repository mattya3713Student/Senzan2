#pragma once
#include "System/Singleton/Debug/Log/DebugLog.h"

#include "../ColliderBase.h"


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

private:
	float m_Radius; // 半径.
	float m_Height;	// 高さ.

#if _DEBUG
public:

	// デバッグ描画用設定.
	void SetDebugInfo() override;
#endif // _DEBUG.

};
