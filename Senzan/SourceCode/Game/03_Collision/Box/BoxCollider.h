#pragma once
#include "../ColliderBase.h"

/**************************************************
*	四角コリジョン.
*	担当 : 淵脇 未来.
**/

class BoxCollider final
	: public ColliderBase
{
public:
	BoxCollider();
	BoxCollider(std::weak_ptr<const Transform> parentTransform);
	~BoxCollider() override;

	void Update() override;

	// 辺の長さの取得.
	inline DirectX::XMFLOAT3 GetSize() const noexcept override { return m_Size; }

	// 辺の長さの設定.
	inline void SetSize(DirectX::XMFLOAT3 size) noexcept override { m_Size = size; }

private:
	DirectX::XMFLOAT3 m_Size; // 一片の長さ.	

#if _DEBUG
public:

	// デバッグ描画用設定.
	void SetDebugInfo() override;
#endif // _DEBUG.

};