//Camera.cpp

#include "Camera.h"
#include "keyboard.h"
#include <cmath> // sinf, cosfを使うために追加

Camera g_Camera; //カメラ管理構造体

void InitCamera()
{
	//カメラの初期化
	g_Camera.Position = XMFLOAT3(0.0f, 1.0f, -5.0f);	//カメラ基本座標（見やすいように少し遠ざけました）
	g_Camera.UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);		//カメラの上方ベクトル
	g_Camera.AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);	//カメラの注視点座標
	g_Camera.Fov = 45.0f;								//画角
	g_Camera.Nearclip = 0.5f;							//近面クリップ
	g_Camera.Farclip = 1000.0f;							//遠面クリップ
	g_Camera.Rotation = 0.0f;

	// マウス操作用の初期化
	g_Camera.AngleX = 0.2f;
	g_Camera.AngleY = 0.0f;
	GetCursorPos(&g_Camera.OldCursorPos);
}

void FinalizeCamera()
{
}

void UpdateCamera()
{
	// 現在のマウス座標を取得
	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// 左クリックを押している間だけカメラを動かす
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		// 前回からのマウスの移動量（差分）を計算
		float dx = (float)(cursorPos.x - g_Camera.OldCursorPos.x);
		float dy = (float)(cursorPos.y - g_Camera.OldCursorPos.y);

		// マウス感度
		float sensitivity = 0.005f;

		// 移動量を角度に加算（Xの移動で横回転、Yの移動で縦回転）
		g_Camera.AngleY += dx * sensitivity;
		g_Camera.AngleX += dy * sensitivity;

		// 縦回転（ピッチ）の制限（真上や真下を越えてぐるんと一回転しないようにする）
		const float PI_HALF = 1.570796f; // 約90度（ラジアン）
		if (g_Camera.AngleX > PI_HALF - 0.1f)  g_Camera.AngleX = PI_HALF - 0.1f;
		if (g_Camera.AngleX < -PI_HALF + 0.1f) g_Camera.AngleX = -PI_HALF + 0.1f;
	}

	// 今回の座標を、次回（1フレーム後）の計算のために保存
	g_Camera.OldCursorPos = cursorPos;

	// 角度からカメラの座標を計算（ターゲットを中心にした球面座標系）
	float radius = 5.0f; // カメラとターゲットの距離
	g_Camera.Position.x = g_Camera.AtPosition.x + sinf(g_Camera.AngleY) * cosf(g_Camera.AngleX) * radius;
	g_Camera.Position.y = g_Camera.AtPosition.y + sinf(g_Camera.AngleX) * radius;
	g_Camera.Position.z = g_Camera.AtPosition.z - cosf(g_Camera.AngleY) * cosf(g_Camera.AngleX) * radius;
}

void DrawCamera() //3D使用時
{
	//プロジェクション行列を作成
	XMMATRIX ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(g_Camera.Fov),
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			g_Camera.Nearclip,
			g_Camera.Farclip
		);
	//プロジェクション行列をセット
	SetProjectionMatrix(ProjectionMatrix);

	//カメラ行列を作成
	XMVECTOR eyev = XMLoadFloat3(&g_Camera.AtPosition);
	XMVECTOR pos = XMLoadFloat3(&g_Camera.Position);
	XMVECTOR up = XMLoadFloat3(&g_Camera.UpVector);
	XMMATRIX ViewMatrix = XMMatrixLookAtLH(pos, eyev, up);

	//カメラ行列をセット
	SetViewMatrix(ViewMatrix);

	//カメラ座標セット（リムライトの計算などに必要）
	SetCameraPosition(g_Camera.Position);
}