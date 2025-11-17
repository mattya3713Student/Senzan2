#include "Setting.h"

// 必要なインクルード
#include "Game/01_GameObject/00_MeshObject/01_Key/Key.h"
#include "System/Singleton/CameraManager/CameraManager.h"
#include "Game/02_Camera/ThirdPersonCamera/ThirdPersonCamera.h"
#include "Graphic/Light/DirectionLight/DirectionLight.h"
#include "Graphic/Light/LightManager.h"
#include <iostream>
#include <conio.h>      // _kbhit(), _getch()
#include <Windows.h>    // GetAsyncKeyStateを使用
#include <string>

#include "System/Singleton/SceneManager/SceneManager.h"

#include "..//..//05_InputDevice/VirtualPad.h"

// Setting::Setting() コンストラクタは変更なし
Setting::Setting()
    : SceneBase()
    , m_pLight(std::make_shared<DirectionLight>())
    , m_pKey(std::make_unique<Key>())
    , m_pCamera(std::make_shared<ThirdPersonCamera>())
    , m_pKeyboardConfig(std::make_unique<TestKeyBoud>()) // キーボード初期化
    , m_pCConfig(std::make_unique<XInputConfig>())
{
    m_pKeyboardConfig->LoadData(); // JSONから読み込み

    VirtualPad::GetInstance().SetKeyConfig(m_pKeyboardConfig.get());

    m_pCConfig->LoadData();

    VirtualPad::GetInstance().SetControllerConfig(m_pCConfig.get());

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
    // 初期化処理
}

void Setting::Create()
{
    // 生成処理
}

void Setting::Update()
{
    m_pCamera->Update();

    static bool isDisplaying = false;

    //// 仮想キーコードを文字列に変換する簡易ヘルパー関数を定義
    //// ★ 本来は TestKeyBoud クラスに GetKeyName(int keyCode) として実装すべきです
    //auto getKeyName = [](int keyCode) -> std::string {
    //    if (keyCode >= 'A' && keyCode <= 'Z') return std::string(1, (char)keyCode);
    //    if (keyCode == VK_SPACE) return "Space";
    //    if (keyCode == VK_LBUTTON) return "LMB";
    //    if (keyCode == VK_RBUTTON) return "RMB";
    //    if (keyCode == VK_LSHIFT) return "L.Shift";
    //    if (keyCode == VK_ESCAPE) return "Escape";
    //    if (keyCode == -1) return "Unbound";
    //    return "Code:" + std::to_string(keyCode);
    //};

    //// ------------------------------------
    //// 1. コンソール表示ロジック
    //// ------------------------------------
    //if (!isDisplaying)
    //{
    //    // 画面をクリアする代わりに、改行で区切って見やすくします
    //    std::cout << "\n\n==============================================\n";
    //    std::cout << "🎮 現在のキーコンフィグ設定 (Rキーでリセット)\n";
    //    std::cout << "==============================================\n";

    //    // ★ 表示改善適用
    //    std::cout << "  [前進 (MoveUp)]:    " << getKeyName(m_pKeyboardConfig->GetKeyCode("MoveUp")) << "\n";
    //    std::cout << "  [後退 (MoveDown)]:  " << getKeyName(m_pKeyboardConfig->GetKeyCode("MoveDown")) << "\n";
    //    std::cout << "  [左移動 (MoveLeft)]:" << getKeyName(m_pKeyboardConfig->GetKeyCode("MoveLeft")) << "\n";
    //    std::cout << "  [右移動 (MoveRight)]:" << getKeyName(m_pKeyboardConfig->GetKeyCode("MoveRight")) << "\n";

    //    std::cout << "\n----------------------------------------------\n";
    //    std::cout << "  【操作方法】:\n";
    //    std::cout << "  1. 変更したいキーに割り当てられているキー (例: W) を押す。\n";
    //    // ... (操作方法のメッセージは省略)
    //    std::cout << "  5. 完了したら Zキー を押して GameMain へ戻る。\n";
    //    std::cout << "==============================================\n\n";

    //    isDisplaying = true;
    //}


    //// ------------------------------------
    //// 2. キーボード入力チェック（キーの再割り当て処理）
    //// ------------------------------------
    //if (_kbhit())
    //{
    //    int key = _getch(); // キーコード取得

    //    if (key == 'Z' || key == 'z' || key == 'R' || key == 'r')
    //    {
    //        // 無視
    //    }
    //    else
    //    {
    //        // 押し間違えなどで小文字で取得された場合も大文字に変換 ('w' -> 'W'など)
    //        // 仮想キーコードとして扱うため
    //        if (key >= 'a' && key <= 'z') key = key - ('a' - 'A');

    //        std::string action = m_pKeyboardConfig->GetActionName(key);

    //        if (!action.empty())
    //        {
    //            std::cout << "--- Keyboard Key DOWN ---" << std::endl;
    //            std::cout << "KeyCode: " << key << ", Action: " << action << std::endl;

    //            // キー再割り当て確認
    //            std::cout << "このアクションのキーを変更しますか？ (y/n): ";
    //            char c = _getch();
    //            std::cout << c << std::endl;

    //            if (c == 'y' || c == 'Y')
    //            {
    //                std::cout << "新しいキーを押してください: ";
    //                int newKey = _getch();
    //                if (newKey >= 'a' && newKey <= 'z') newKey = newKey - ('a' - 'A'); // 新しいキーも大文字化

    //                std::cout << getKeyName(newKey) << " が選択されました。" << std::endl;

    //                // 既に新しいキーに他のアクションが割り当てられていないかチェック
    //                std::string existAction = m_pKeyboardConfig->GetActionName(newKey);

    //                // ★ 上書き解除ロジック
    //                if (!existAction.empty() && existAction != action) // 既に割り当てられているアクションが、今設定しようとしているアクションと異なる場合のみ
    //                {
    //                    std::cout << "警告: " << existAction << " に割り当てられています。上書きし、元のアクションを解除します。\n";
    //                    m_pKeyboardConfig->RebindKey(existAction, -1);
    //                }

    //                // キーを再割り当て
    //                m_pKeyboardConfig->RebindKey(action, newKey);

    //                // 保存
    //                m_pKeyboardConfig->SaveData();

    //                // ★ 必須修正: VirtualPad に新しい設定を反映させる
    //                VirtualPad::GetInstance().SetKeyConfig(m_pKeyboardConfig.get());

    //                // 表示をリセットして、最新の情報を再表示させます
    //                isDisplaying = false;
    //            }
    //        }
    //    }
    //}

    //// ------------------------------------
    //// 3. リセット機能（Rキー）のチェック
    //// ------------------------------------
    //if (GetAsyncKeyState('R') & 0x8000)
    //{
    //    std::cout << "\n--- Rキーが押されました。キーコンフィグを初期設定に戻します ---\n";

    //    // TestKeyBoud を再構築
    //    m_pKeyboardConfig->LoadDefaultData();
    //    m_pKeyboardConfig->SaveData();

    //    // ★ 必須修正: VirtualPad にリセット後の設定を反映させる
    //    VirtualPad::GetInstance().SetKeyConfig(m_pKeyboardConfig.get());

    //    std::cout << "--- 初期設定に戻し、保存しました。ZキーでGameMainへ戻ってください ---\n";

    //    // 表示をリセットして、新しい設定を表示させる
    //    isDisplaying = false;
    //}

XINPUT_STATE state;
ZeroMemory(&state, sizeof(XINPUT_STATE));

if (XInputGetState(0, &state) == ERROR_SUCCESS)
{
    WORD buttons = state.Gamepad.wButtons;

    // もしボタンが押されたら表示
    if (buttons != 0)
    {
        std::cout << "押されているボタン: ";

        if (buttons & XINPUT_GAMEPAD_A) std::cout << "[A] ";
        if (buttons & XINPUT_GAMEPAD_B) std::cout << "[B] ";
        if (buttons & XINPUT_GAMEPAD_X) std::cout << "[X] ";
        if (buttons & XINPUT_GAMEPAD_Y) std::cout << "[Y] ";
        if (buttons & XINPUT_GAMEPAD_DPAD_UP) std::cout << "[Up] ";
        if (buttons & XINPUT_GAMEPAD_DPAD_DOWN) std::cout << "[Down] ";
        if (buttons & XINPUT_GAMEPAD_DPAD_LEFT) std::cout << "[Left] ";
        if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) std::cout << "[Right] ";
        if (buttons & XINPUT_GAMEPAD_START) std::cout << "[Start] ";
        if (buttons & XINPUT_GAMEPAD_BACK) std::cout << "[Back] ";
        if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) std::cout << "[L1] ";
        if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) std::cout << "[R1] ";
        if (buttons & XINPUT_GAMEPAD_LEFT_THUMB) std::cout << "[L3] ";
        if (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) std::cout << "[R3] ";
        std::cout << std::endl;
    }
}

    // ------------------------------------
// 5. コントローラー設定の表示と再割り当て
// ------------------------------------
    {
        static bool isControllerDisplaying = false;

        // XInput 状態の取得
        XINPUT_STATE state{};
        DWORD dwResult = XInputGetState(0, &state); // コントローラー1番ポート
        if (dwResult == ERROR_SUCCESS)
        {
            if (!isControllerDisplaying)
            {
                std::cout << "\n\n==============================================\n";
                std::cout << "🎮 現在のコントローラー設定 (Cキーでリセット)\n";
                std::cout << "==============================================\n";
                std::cout << "  [前進 (MoveUp)]    : Button " << m_pCConfig->GetController("Up") << "\n";
                std::cout << "  [後退 (MoveDown)]  : Button " << m_pCConfig->GetController("Down") << "\n";
                std::cout << "  [左移動 (MoveLeft)]: Button " << m_pCConfig->GetController("Left") << "\n";
                std::cout << "  [右移動 (MoveRight)]: Button " << m_pCConfig->GetController("Right") << "\n";
                std::cout << "----------------------------------------------\n";
                std::cout << "  ボタンを押すと割り当てられたアクションが表示されます。\n";
                std::cout << "  Yキーで再割り当てモード、Cキーでリセット、Zキーで終了。\n";
                std::cout << "==============================================\n\n";
                isControllerDisplaying = true;
            }

            // ボタン入力チェック
            WORD buttons = state.Gamepad.wButtons;

            // どのボタンが押されたかをループで確認（16ビット分）
            for (int i = 0; i < 16; ++i)
            {
                if (buttons & (1 << i))
                {
                    std::string action = m_pCConfig->GetActionName(i);
                    std::cout << "--- Controller Button Pressed ---\n";
                    std::cout << "Button: " << i << ", Action: " << (action.empty() ? "なし" : action) << "\n";

                    // 割り当て変更確認
                    std::cout << "このアクションを変更しますか？ (y/n): ";
                    char c = _getch();
                    std::cout << c << "\n";

                    if (c == 'y' || c == 'Y')
                    {
                        std::cout << "新しいボタンを押してください...\n";

                        // 新しいボタンが押されるまで待つ
                        XINPUT_STATE newState{};
                        int newButton = -1;
                        while (true)
                        {
                            XInputGetState(0, &newState);
                            WORD newButtons = newState.Gamepad.wButtons;
                            for (int j = 0; j < 16; ++j)
                            {
                                if (newButtons & (1 << j))
                                {
                                    newButton = j;
                                    break;
                                }
                            }
                            if (newButton != -1)
                                break;
                            Sleep(100); // CPU負荷を避ける
                        }

                        std::cout << "新しいボタン " << newButton << " が選択されました。\n";

                        // 他のアクションが使っていたら解除
                        std::string existAction = m_pCConfig->GetActionName(newButton);
                        if (!existAction.empty() && existAction != action)
                        {
                            std::cout << "警告: " << existAction << " に割り当てられています。解除します。\n";
                            m_pCConfig->RebindController(existAction, -1);
                        }

                        // 再割り当て
                        m_pCConfig->RebindController(action, newButton);
                        m_pCConfig->SaveData();
                        VirtualPad::GetInstance().SetControllerConfig(m_pCConfig.get());

                        std::cout << "新しいボタン設定を保存しました。\n";
                        isControllerDisplaying = false;
                    }
                    break; // 1回の押下で1つだけ処理
                }
            }

            // リセット（Cキー）
            if (GetAsyncKeyState('C') & 0x8000)
            {
                std::cout << "\n--- Cキーが押されました。コントローラー設定を初期化します ---\n";
                m_pCConfig->LoadDefaultData();
                m_pCConfig->SaveData();
                VirtualPad::GetInstance().SetControllerConfig(m_pCConfig.get());
                std::cout << "--- 初期設定に戻しました ---\n";
                isControllerDisplaying = false;
            }
        }
        else
        {
            if (!isControllerDisplaying)
            {
                std::cout << "\n⚠ コントローラーが接続されていません。\n";
                isControllerDisplaying = true;
            }
        }
    }


    // ------------------------------------
    // 4. シーン遷移（Zキー）のチェック
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