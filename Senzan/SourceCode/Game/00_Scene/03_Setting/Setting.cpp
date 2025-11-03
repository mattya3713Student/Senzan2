#include "Setting.h"

// 必要なインクルード
#include "Game/01_GameObject/00_MeshObject/01_Key/Key.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "Game/02_Camera/ThirdPersonCamera/ThirdPersonCamera.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"
#include <conio.h>      

#include "System/Singleton/SceneManager/SceneManager.h"

// Setting::Setting() コンストラクタは変更なし
Setting::Setting()
    : SceneBase()
    , m_pLight(std::make_shared<DirectionLight>())
    , m_pKey(std::make_unique<Key>())
    , m_pCamera(std::make_shared<ThirdPersonCamera>())
    , m_pKeyboardConfig(std::make_unique<TestKeyBoud>()) // キーボード初期化
{
    m_pKeyboardConfig->LoadData(); // JSONから読み込み

    // カメラ設定
    m_pCamera->SetPosition(DirectX::XMFLOAT3(0.0f, 5.0f, -5.0f));
    m_pCamera->SetLook(DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f));
    CameraManager::AttachCamera(m_pCamera);

    // ライト設定
    m_pLight->SetDirection(DirectX::XMFLOAT3(1.5f, 1.f, -1.f));
    LightManager::AttachDirectionLight(m_pLight);

    m_pGround = std::make_unique<Ground>();
}

// Setting::~Setting() デストラクタは変更なし
Setting::~Setting()
{
}

void Setting::Initialize()
{
}

void Setting::Create()
{
}

void Setting::Update()
{
    m_pCamera->Update();

    // isDisplaying フラグをstaticで定義して、一度だけコンソール表示を行うようにします。
    static bool isDisplaying = false;

    // ------------------------------------
    // 1. コンソール表示ロジック
    // ------------------------------------
    if (!isDisplaying)
    {
        // 画面をクリアする代わりに、改行で区切って見やすくします
        std::cout << "\n\n==============================================\n";
        std::cout << "🎮 現在のキーコンフィグ設定 (Rキーでリセット)\n";
        std::cout << "==============================================\n";

        // TestKeyBoud::GetKeyCode() が存在することを前提とします
        std::cout << "  [前進 (MoveUp)]:    " << (char)m_pKeyboardConfig->GetKeyCode("MoveUp") << "  (コード: " << m_pKeyboardConfig->GetKeyCode("MoveUp") << ")\n";
        std::cout << "  [後退 (MoveDown)]:  " << (char)m_pKeyboardConfig->GetKeyCode("MoveDown") << "  (コード: " << m_pKeyboardConfig->GetKeyCode("MoveDown") << ")\n";
        std::cout << "  [左移動 (MoveLeft)]:" << (char)m_pKeyboardConfig->GetKeyCode("MoveLeft") << "  (コード: " << m_pKeyboardConfig->GetKeyCode("MoveLeft") << ")\n";
        std::cout << "  [右移動 (MoveRight)]:" << (char)m_pKeyboardConfig->GetKeyCode("MoveRight") << " (コード: " << m_pKeyboardConfig->GetKeyCode("MoveRight") << ")\n";

        std::cout << "\n----------------------------------------------\n";
        std::cout << "  【操作方法】:\n";
        std::cout << "  1. 変更したいキーに割り当てられているキー (例: W) を押す。\n";
        std::cout << "  2. コンソールで 'y' を入力。\n";
        std::cout << "  3. 新しいキー (例: S) を押す。\n";
        std::cout << "  4. リセットは Rキー を押す。\n";
        std::cout << "  5. 完了したら Zキー を押して GameMain へ戻る。\n";
        std::cout << "==============================================\n\n";

        isDisplaying = true;
    }


    // ------------------------------------
    // 2. キーボード入力チェック（キーの再割り当て処理）
    // ------------------------------------
    if (_kbhit())
    {
        int key = _getch(); // キーコード取得

        // Z, RキーはGetAsyncKeyStateで処理するため、_getch()の対話処理から除外します
        if (key == 'Z' || key == 'z' || key == 'R' || key == 'r')
        {
            // 無視して次のGetAsyncKeyStateチェックへ
        }
        else
        {
            std::string action = m_pKeyboardConfig->GetActionName(key);

            if (!action.empty())
            {
                std::cout << "--- Keyboard Key DOWN ---" << std::endl;
                std::cout << "KeyCode: " << key << ", Action: " << action << std::endl;

                // キー再割り当て確認
                std::cout << "このアクションのキーを変更しますか？ (y/n): ";
                char c = _getch();
                std::cout << c << std::endl;

                if (c == 'y' || c == 'Y')
                {
                    std::cout << "新しいキーを押してください: ";
                    int newKey = _getch();
                    std::cout << (char)newKey << " が選択されました。" << std::endl;

                    // 既に新しいキーに他のアクションが割り当てられていないかチェック
                    std::string existAction = m_pKeyboardConfig->GetActionName(newKey);

                    // ★ 上書き解除ロジック
                    if (!existAction.empty())
                    {
                        std::cout << "警告: " << existAction << " に割り当てられています。上書きし、元のアクションを解除します。\n";
                        // 既に割り当てられているアクションからキーを解除 (キーコードを -1 に設定)
                        m_pKeyboardConfig->RebindKey(existAction, -1);
                    }

                    // キーを再割り当て
                    m_pKeyboardConfig->RebindKey(action, newKey);

                    // 保存
                    m_pKeyboardConfig->SaveData();

                    // 表示をリセットして、最新の情報を再表示させます
                    isDisplaying = false;
                }
            }
        }
    }

    // ------------------------------------
    // 3. リセット機能（Rキー）のチェック (既存ロジック)
    // ------------------------------------
    if (GetAsyncKeyState('R') & 0x8000)
    {
        std::cout << "\n--- Rキーが押されました。キーコンフィグを初期設定に戻します ---\n";

        m_pKeyboardConfig = std::make_unique<TestKeyBoud>();
        m_pKeyboardConfig->LoadDefaultData();
        m_pKeyboardConfig->SaveData();

        std::cout << "--- 初期設定に戻し、保存しました。ZキーでGameMainへ戻ってください ---\n";

        // 表示をリセットして、新しい設定を表示させる
        isDisplaying = false;
    }

    // ------------------------------------
    // 4. シーン遷移（Zキー）のチェック (既存ロジック)
    // ------------------------------------
    if (GetAsyncKeyState('Z') & 0x8000)
    {
        std::cout << "\n--- Zキーが押されました。設定を完了し、GameMainへ戻ります ---\n";
        SceneManager::GetInstance().LoadScene(eList::L); // LがGameMainシーンだと仮定
    }
}

void Setting::LateUpdate()
{
    // LateUpdate処理
}

void Setting::Draw()
{
    m_pKey->Draw();
    // m_pGround->Draw();
}