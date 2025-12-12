#include "Key.h"

Key::Key()
{
	//今表示したとき黒色なんだけど今は直さない,
	AttachMesh(MeshManager::GetInstance().GetStaticMesh("KeyConfing"));
	DirectX::XMFLOAT3 pos = { 0.05f, 2.5f, 20.05f };
	DirectX::XMFLOAT3 scale = { 1.5f, 1.5f, 1.5f };


	m_Transform->SetPosition(pos);

	m_Transform->SetScale(scale);

}

Key::~Key()
{
}

void Key::Update()
{
	//ここに毎フレーム回転する処理を追加する.
}

void Key::LateUpdate()
{
}

void Key::Draw()
{
	MeshObject::Draw();
}
