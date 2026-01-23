#pragma once
#pragma once
#include "02_UIObject/UIObject.h"

/*********************************************
*	ULT演出クラス.
**/

class ULTSparkle
{
public:
    ULTSparkle(std::shared_ptr<UIObject> pObje);
    ~ULTSparkle();

    void Update();
    void LateUpdate();
    void Draw();

    void Create();

    // スパークルのアニメーションを実行.
    void DoPeakAnim();
    // ゲージが最大かどうかを外部からセットする
    void SetULTGaugeStatus(bool isMax, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 size);

private:
    void UpDateParticles(float dt);
private:
    std::shared_ptr<UIObject> m_pMainSparkle;
    std::vector<std::shared_ptr<UIObject>> m_Particles;

    DirectX::XMFLOAT2 m_GaugeX;
    DirectX::XMFLOAT2 m_GaugeY;

    bool   m_IsAnimating;
    float  m_Timer;
    float  m_Duration;
    float  m_StartScale;
    float  m_PeakScale;

    bool  m_IsGaugeMax;
    float m_SpawnTimer;
    float m_SpawnInterval;
    float m_ParticlesDecAlpha;
};
