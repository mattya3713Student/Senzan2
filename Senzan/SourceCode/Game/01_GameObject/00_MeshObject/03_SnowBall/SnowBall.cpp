#include "SnowBall.h"

SnowBall::SnowBall()
{
	AttachMesh(MeshManager::GetInstance().GetSkinMesh("snowball_nomal"));
	//雪玉の配置とサイズの変更.
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f, 0.0f };
	DirectX::XMFLOAT3 scale = { 0.05f, 0.05f, 0.05f };
	m_Transform->SetPosition(pos);
	m_Transform->SetScale(scale);
}

SnowBall::~SnowBall()
{
}

void SnowBall::Update()
{
}

void SnowBall::LateUpdate()
{
}

void SnowBall::Draw()
{
	MeshObject::Draw();
}
