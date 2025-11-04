#pragma once

#include <vector>
#include <string>
#include <memory>

/**************************************************
*	当たり判定(基底).
*	担当 : 淵脇 未来.
**/

class ColliderBase
{
public:

	enum class eShapeType : uint32_t
	{
		Box = 0,
		Sphere,
		Capsule,
		_Max
	};

public:
	ColliderBase();
	ColliderBase(std::weak_ptr<const Transform> parentTransform);
	virtual ~ColliderBase();	

	virtual void Update() = 0;

	// 親のトランスフォームを取得する.
	//inline const std::weak_ptr<Transform>& GetParentTransform() const noexcept { return m_wpTransform; }

	// 座標を取得する.
	inline const DirectX::XMFLOAT3 GetPosition() const noexcept; 

	// 座標のオフセット値を取得する.
	inline const DirectX::XMFLOAT3 GetPositionOffset() const noexcept { return m_PositionOffset; }
	inline void SetPositionOffset(DirectX::XMFLOAT3 NewOffset) noexcept { m_PositionOffset = NewOffset; }
	inline void SetPositionOffset(float NewOffset_x, float NewOffset_y, float NewOffset_z) noexcept { m_PositionOffset = DirectX::XMFLOAT3(NewOffset_x, NewOffset_y, NewOffset_z); }

	// 大きさ取得(派生で実装).
	virtual float GetRadius() const noexcept { return 0.0f; }
	virtual float GetHeight() const noexcept { return 0.0f; }
	virtual DirectX::XMFLOAT3 GetSize() const noexcept { return DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f); }

	// 大きさ設定(派生で実装).
	virtual void SetRadius(float Radius) noexcept {}
	virtual void SetHeight(float Height) noexcept {}
	virtual void SetSize(DirectX::XMFLOAT3 Size) noexcept {}

	// 色の設定.
	void SetColor(Color::eColor NewColor) noexcept { m_Color = Color::GetColorValue(NewColor); }
	inline void SetColor(DirectX::XMFLOAT4 NewColor) noexcept { m_Color = NewColor; }
	inline void SetColor(float NewR, float NewG, float NewB, float NewA) noexcept { m_Color = DirectX::XMFLOAT4(NewR, NewG, NewB, NewA); }

protected:

	std::weak_ptr<const Transform> m_wpTransform; // 持ち主のトランスフォーム.
	DirectX::XMFLOAT3	m_PositionOffset;	// オフセット位置.

	DirectX::XMFLOAT4	m_Color;			// 表示色.
	
#if _DEBUG
public:
	//------------デバッグ描画用-----------.

	virtual void SetDebugInfo() = 0;
#endif // _DEBUG.

}; 

// 座標を取得する.
inline const DirectX::XMFLOAT3 ColliderBase::GetPosition() const noexcept
{
	if (auto spTransform = m_wpTransform.lock())
	{
		DirectX::XMVECTOR v_position = DirectX::XMLoadFloat3(&spTransform->Position);
		DirectX::XMVECTOR v_offset = DirectX::XMLoadFloat3(&m_PositionOffset);

		DirectX::XMVECTOR v_result_pos = DirectX::XMVectorAdd(v_position, v_offset);

		DirectX::XMFLOAT3 result_pos = {};
		DirectX::XMStoreFloat3(&result_pos, v_result_pos);
		return result_pos;
	}
	return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}
