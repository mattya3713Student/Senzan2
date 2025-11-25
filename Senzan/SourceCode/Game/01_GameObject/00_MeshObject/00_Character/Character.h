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


protected:
	std::unique_ptr<CompositeCollider>	m_upColliders;	// 衝突.
};