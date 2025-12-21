#pragma once
#include "Game/01_GameObject/00_MeshObject/MeshObject.h"
class CompositeCollider;


/**************************************************
*   キャラクタークラス
**/
class Character
	: public MeshObject
{
public:
	Character();
	virtual ~Character();

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Draw() override;

	inline float GetMaxHP() const noexcept { return m_MaxHP; }
	inline float GetHP() const noexcept { return m_HP; }


protected:

	// 衝突応答処理.
	void HandleCollisionResponse();

	// ダメージ適用.
	void ApplyDamage(float damageAmount);

	// 衝突_被ダメージ.
	virtual void HandleDamageDetection() = 0;
	// 衝突_攻撃判定.
	virtual void HandleAttackDetection() = 0;
	// 衝突_回避.
	virtual void HandleDodgeDetection() = 0;


protected:
	std::unique_ptr<CompositeCollider>	m_upColliders;	// 衝突.

	float m_MaxHP;							// 最大.
	float m_HP;								// 体力.
};