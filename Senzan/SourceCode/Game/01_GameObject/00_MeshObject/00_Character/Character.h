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

protected:

	// 衝突応答処理.
	void HandleCollisionResponse();

	// 衝突_被ダメージ.
	virtual void HandleDamageDetection() = 0;
	// 衝突_攻撃判定.
	virtual void HandleAttackDetection() = 0;
	// 衝突_回避.
	virtual void HandleDodgeDetection() = 0;


protected:
	std::unique_ptr<CompositeCollider>	m_upColliders;	// 衝突.
};
//中二病激発させるエフェクト、Playerの手触りにこだわりました