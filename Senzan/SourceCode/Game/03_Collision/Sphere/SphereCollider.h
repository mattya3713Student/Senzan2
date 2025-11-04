#pragma once
#include "../ColliderBase.h"


/**********************************************************************************
* @author    : 淵脇 未来.
* @date      : 2025/10/5.
* @brief     : 球体コリジョン.
**********************************************************************************/

class SphereCollider final
	: public ColliderBase
{
public:
	SphereCollider();
	SphereCollider(std::weak_ptr<const Transform> parentTransform);
	~SphereCollider() override;

	void Update() override;

	// 半径の取得.
	inline float GetRadius() const noexcept override { return m_Radius; }

	// 半径を設定する.
	inline void SetRadius(float Radius) noexcept override { m_Radius = Radius; }

private:
	float m_Radius; // 半径.

#if _DEBUG
public:

	// デバッグ描画用設定.
	void SetDebugInfo() override;
#endif // _DEBUG.

};

