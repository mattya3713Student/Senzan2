#include "ResourceManager.h"

ResourceManager::ResourceManager()
	: m_hWnd( nullptr )
{	
}

//------------------------------------------------------------------------.

ResourceManager::~ResourceManager()
{
}

//------------------------------------------------------------------------.

bool ResourceManager::LoadMesh()
{
	if (MeshManager::LoadAllMeshes()	== false) { return false; }
	if (SpriteManager::LoadSprites()	== false) { return false; }	
	
	return true;
}

//------------------------------------------------------------------------.

bool ResourceManager::LoadSounds()
{
	return SoundManager::LoadSounds(GetInstance().m_hWnd);
}

//------------------------------------------------------------------------.

bool ResourceManager::LoadEffects()
{
	if (EffectResource::GetInstance().Create()	== false) { return false; }
	if (EffectResource::GetInstance().LoadData()	== false) { return false; }
	
	return true;
}

//------------------------------------------------------------------------.

std::shared_ptr<StaticMesh> ResourceManager::GetStaticMesh(const std::string& name)
{
	return MeshManager::GetStaticMesh(name);
}

//------------------------------------------------------------------------.

std::shared_ptr<SkinMesh> ResourceManager::GetSkinMesh(const std::string& name)
{
	return MeshManager::GetSkinMesh(name);
}


//------------------------------------------------------------------------.

std::shared_ptr<Sprite2D> ResourceManager::GetSprite2D(const std::string& name)
{
	return SpriteManager::GetSprite2D(name);
}

//------------------------------------------------------------------------.

void ResourceManager::SethWnd(HWND hWnd)
{
	GetInstance().m_hWnd = hWnd;
}

//------------------------------------------------------------------------.

HWND ResourceManager::GethWnd()
{
	return GetInstance().m_hWnd;
}
