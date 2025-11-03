#include "Loader.h"
#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "System/Singleton/ResourceManager/ResourceManager.h"
#include "02_UIObject/UIObject.h"
#include <chrono>

Loader::Loader()
    : m_IsLoadResult        ()
    , m_pBackGroundImage    ( std::make_shared<Sprite2D>() )
    , m_pTextImage          ( std::make_shared<Sprite2D>() )
    , m_pBackGroundObject   ( std::make_unique<UIObject>() )
    , m_pTextObject         ( std::make_unique<UIObject>() )
    , m_TextAlpha           ( 1.0f )
{
}

//-------------------------------------------------------------------.

Loader::~Loader()
{
}

//-------------------------------------------------------------------.

void Loader::LoadData()
{
    // 画像の読み込み.
    m_pBackGroundImage->Initialize("Data\\Image\\Loading\\LoadingBackImg.png");
    m_pTextImage->Initialize("Data\\Image\\Loading\\LoadingTextImg.png");
    
    // 画像を接続.
    m_pBackGroundObject->AttachSprite(m_pBackGroundImage);
    m_pTextObject->AttachSprite(m_pTextImage);   
}

//-------------------------------------------------------------------.

void Loader::Update()
{
    m_TextAlpha -= 0.01f;
    if (m_TextAlpha <= 0.0f) { m_TextAlpha = 1.0f; }
    m_pTextObject->SetAlpha(m_TextAlpha);    
}

//-------------------------------------------------------------------.

void Loader::Draw()
{
    //バックバッファをクリアにする.
    DirectX11::GetInstance().ClearBackBuffer();

    DirectX11::GetInstance().SetDepth(false);
    m_pBackGroundObject->Draw();
    m_pTextObject->Draw();
    DirectX11::GetInstance().SetDepth(true);

    // 画面に表示.
    DirectX11::GetInstance().Present();
}

//-------------------------------------------------------------------.

void Loader::StartLoading()
{
    // スレッドに関数を渡して非同期で読み込み開始.
    m_IsLoadResult.emplace_back(std::async(std::launch::async, &Loader::LoadGraphicsAssets, this));
//    m_IsLoadResult.emplace_back(std::async(std::launch::async, &Loader::LoadSounds , this));
//    m_IsLoadResult.emplace_back(std::async(std::launch::async, &Loader::LoadEffects, this));
}

//-------------------------------------------------------------------.

const bool Loader::IsLoadCompletion()
{
    // 一つでも読み込みが完了していなければfalseを返す.
    for (const auto& result : m_IsLoadResult) {
        if (result.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------.

bool Loader::LoadGraphicsAssets()
{
    return ResourceManager::LoadMesh();
}

//-------------------------------------------------------------------.

bool Loader::LoadSounds()
{
    return true; //ResourceManager::LoadSounds();
}

//-------------------------------------------------------------------.

bool Loader::LoadEffects()
{
    return true; //ResourceManager::LoadEffects();
}
