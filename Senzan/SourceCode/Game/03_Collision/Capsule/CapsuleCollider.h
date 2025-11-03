#pragma once
#include "System/Singleton/Debug/Log/DebugLog.h"

#include "../ColliderBase.h"

/**************************************************
*	ƒJƒvƒZƒ‹ƒRƒŠƒWƒ‡ƒ“.
*	’S“– : •£˜e –¢—ˆ.
**/

class CapsuleCollider final
	: public ColliderBase
{
public:
	CapsuleCollider();
	CapsuleCollider(std::weak_ptr<const Transform> parentTransform);
	~CapsuleCollider() override;

	void Update() override;


	// ”¼Œa‚Ìæ“¾.
	inline float GetRadius() const noexcept override { return m_Radius; }

	// ‚‚³‚ğæ“¾‚·‚é.
	inline float GetHeight() const noexcept override { return m_Height; }

	// ”¼Œa‚ğİ’è‚·‚é.
	inline void SetRadius(float Radius) noexcept override { m_Radius = Radius; }
	// ‚‚³‚ğİ’è‚·‚é.
	inline void SetHeight(float Height) noexcept override { m_Height = Height; }

private:
	float m_Radius; // ”¼Œa.
	float m_Height;	// ‚‚³.

#if _DEBUG
public:

	// ƒfƒoƒbƒO•`‰æ—pİ’è.
	void SetDebugInfo() override;
#endif // _DEBUG.

};
