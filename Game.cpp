#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "keyboard.h"
#include "Manager.h"
#include "Camera.h"
#include "texture.h"

#include	"Sprite2D.h"

#include	"Field3D.h"
#include	"PolygonModel.h"
#include	"VertexDirectionalLighting.h"
#include	"PixelDirectionalLighting.h"
#include	"PixelLightingBlinnPhong.h"
#include	"HemiSphereLighting.h"

//===============================================
//グローバル変数
 
Camera		CameraObject;
Sprite2D	test2D;

Field3D			Field;
PolygonModel	Model;
VertexDirectionalLighting	VDLModel;
PixelDirectionalLighting	PDLModel;
PixelLightingBlinnPhong		PLBModel;
HemiSphereLighting			HSLModel;

//ポーズフラグ
static	bool	pause = false;

//===============================================
//ポーズフラグセット
void	SetPause(bool flg)
{
	pause = flg;
}
//===============================================
//ポーズフラグ取得
bool	GetPause()
{
	return pause;
}

//===============================================
//ゲームシーン初期化
void InitGame()
{
	TextureInitialize(GetDevice());
	InitCamera();

	test2D.Init();

	Field.Init();
	Model.Init();
	VDLModel.Init();
	PDLModel.Init();
	PLBModel.Init();
	HSLModel.Init();
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
	FinalizeCamera();
	test2D.Finalize();

	Field.Finalize();
	Model.Finalize();
	VDLModel.Finalize();
	PDLModel.Finalize();
	PLBModel.Finalize();
	HSLModel.Finalize();


	TextureFinalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
		UpdateCamera();
		test2D.Update();

		Field.Update();
		Model.Update();
		VDLModel.Update();
		PDLModel.Update();
		PLBModel.Update();
		HSLModel.Update();

	}

}

//===============================================
//ゲームシーン描画
void DrawGame()
{

	// 2D用マトリクス設定
	{
		SetWorldViewProjection2D();
		test2D.Draw();
	}
	//3D用マトリクス設定
	{
		SetDepthEnable(true);		//奥行き処理有効
		DrawCamera();

		Field.Draw();
		Model.Draw();
		VDLModel.Draw();
		PDLModel.Draw();
		PLBModel.Draw();
		HSLModel.Draw();

	}
}
