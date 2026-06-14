#pragma once

//Camera.h
#include "main.h"
#include "renderer.h"
#include <windows.h> // POINT構造体を使うために追加

class Camera
{
public:
	XMFLOAT3	Position;		//カメラの座標
	XMFLOAT3	AtPosition;		//カメラの注視点
	XMFLOAT3	UpVector;		//上方ベクトル
	float		Fov;			//視野角
	float		Nearclip;		//どこまで近くが見えるか
	float		Farclip;		//どこまで遠くが見えるか
	float		Rotation;

	// --- マウス操作用の変数を追加 ---
	float		AngleX;			//縦の回転角度
	float		AngleY;			//横の回転角度
	POINT		OldCursorPos;	//1フレーム前のマウス座標
};

void	InitCamera();
void	FinalizeCamera();
void	UpdateCamera();
void	DrawCamera();