#include "ULTSparkle.h"
#include "02_UIObject/UILoader/UILoader.h"

#include "Graphic/DirectX/DirectX11/DirectX11.h"
#include "Game/04_Time/Time.h"
#include "Math/Easing/Easing.h"
#include "Utility/Math/Random/Random.h"
#include "ImGui/CImGuiManager.h"

ULTSparkle::ULTSparkle(std::shared_ptr<UIObject> pObje)
    : m_pMainSparkle    ( pObje )
    , m_IsAnimating     ( false )
    , m_Timer           ( 0.0f )
    , m_Duration        ( 0.8f )
    , m_StartScale      ( 0.0f )
    , m_PeakScale       ( 1.f )
    , m_IsGaugeMax      ( false )
    , m_SpawnTimer      ( 0.0f )
    , m_SpawnInterval   ( 0.4f )
{
    Create();
}

//----------------------------------------------------------------.

ULTSparkle::~ULTSparkle()
{
}

//----------------------------------------------------------------.

void ULTSparkle::Create()
{
    m_pMainSparkle->SetAlpha(0.0f);
    m_pMainSparkle->SetScale({ 0.0f,0.0f,0.0f });
}

//----------------------------------------------------------------.

void ULTSparkle::DoPeakAnim()
{
    m_IsAnimating = true;
    m_Timer = 0.0f;

}

//----------------------------------------------------------------.

void ULTSparkle::SetULTGaugeStatus(bool isMax, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 size)
{
    m_IsGaugeMax = isMax;
    m_GaugeX = { pos.x - (size.x / 2),pos.x + (size.x / 2) };
    m_GaugeY = { pos.y - (size.y / 2),pos.y + (size.y / 2) };
}

//----------------------------------------------------------------.

void ULTSparkle::UpDateParticles(float dt)
{
    if (m_IsGaugeMax)
    {
        m_SpawnTimer += dt;
        if (m_SpawnTimer >= m_SpawnInterval)
        {
            m_SpawnTimer = 0.0f;

            auto pNewUI = std::make_shared<UIObject>();
            pNewUI->AttachSprite(m_pMainSparkle->GetSprite());

            float x = MyRand::GetRandomPercentage(m_GaugeX.x, m_GaugeX.y);
            float y = MyRand::GetRandomPercentage(m_GaugeY.x, m_GaugeY.y);

            pNewUI->SetPosition({ x, y, 0.0f });

            pNewUI->SetScale({ 0.1f, 0.1f, 1.0f });
            pNewUI->SetColor(m_pMainSparkle->GetColor());
            pNewUI->SetAlpha(1.0f);

            m_Particles.push_back(pNewUI);
        }
    }

    // --- 2. 個別更新と削除のループ ---.
    auto it = m_Particles.begin();
    while (it != m_Particles.end())
    {
        auto& pUI = *it;

        float currentAlpha = pUI->GetAlpha();
        currentAlpha -= dt;
        pUI->SetAlpha(currentAlpha);

        if (currentAlpha <= 0.0f)
        {
            it = m_Particles.erase(it);
        }
        else
        {
            float currentRot = pUI->GetRotation().z;
            pUI->SetRotationZ(currentRot + dt * 3.0f);

            pUI->Update();
            ++it;
        }
    }
}

//----------------------------------------------------------------.

void ULTSparkle::Update()
{
    float dt = Time::GetInstance().GetDeltaTime();
    UpDateParticles(dt);

    if (m_IsAnimating)
    {
        m_Timer += Time::GetInstance().GetDeltaTime();
        float duration = 1.f; // 全体時間.
        float progress = m_Timer / duration;
        if (progress > 1.0f) progress = 1.0f;

        // --- 閾値の微調整 ---.
        const float peakStart = 0.2f; // 最大までの時間.
        const float peakEnd = 0.6f; // ドヤ顔時間.

        float currentScale = 0.0f;
        float currentAlpha = 0.0f;
        float rotationTarget = 0.0f;

        if (progress < peakStart)
        {
            // --- 1. 拡大フェーズ ---.
            float t = progress / peakStart;
            // 拡大.
            MyEasing::UpdateEasing(MyEasing::Type::OutCirc, t, 1.0f, 0.0f, m_PeakScale, currentScale);
            MyEasing::UpdateEasing(MyEasing::Type::OutQuint, t, 1.0f, 0.0f, 1.0f, currentAlpha);
            // 回転.
            rotationTarget = t * 3.0f;
        }
        else if (progress < peakEnd)
        {
            // --- 2. 静止維持フェーズ ---.
            currentScale = m_PeakScale;
            currentAlpha = 1.0f;
            // 維持中もゆっくり回し続ける.
            float t = (progress - peakStart) / (peakEnd - peakStart);
            rotationTarget = 10.0f + t * 2.0f;
        }
        else
        {
            // --- 3. 急加速縮小フェーズ ---.
            float t = (progress - peakEnd) / (1.0f - peakEnd);
            // InExpoで吸い込まれるように一瞬で消す.
            MyEasing::UpdateEasing(MyEasing::Type::InCirc, t, 1.0f, m_PeakScale, 0.0f, currentScale);
            MyEasing::UpdateEasing(MyEasing::Type::InExpo, t, 1.0f, 1.0f, 0.0f, currentAlpha);
            rotationTarget = t * 4.0f;
        }

        m_pMainSparkle->SetRotation({ 0, 0, rotationTarget });
        m_pMainSparkle->SetScale({ currentScale, currentScale, 1.0f });
        m_pMainSparkle->SetAlpha(currentAlpha);

        if (progress >= 1.0f)
        {
            m_IsAnimating = false;
            m_pMainSparkle->SetAlpha(0.0f);
        }
    }
    m_pMainSparkle->Update();
}

//----------------------------------------------------------------.

void ULTSparkle::LateUpdate()
{
}

//----------------------------------------------------------------.

void ULTSparkle::Draw()
{
    DirectX11::GetInstance().SetDepth(false);

    // パーティクルを全部描画
    for (auto& part : m_Particles) {
        part->Draw();
    }

    // 主張演出を描画
    m_pMainSparkle->Draw();

    DirectX11::GetInstance().SetDepth(true);
}
