/*==============================================================================

   [Field3D.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once


#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


////////////////
//構造体
////////////////

class Field3D
{
	protected:
		XMFLOAT3	Position;
		XMFLOAT3	Scale;
		XMFLOAT3	Rotate;
		int			TexID;

		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;    // ★ 追加
		UINT          IndexCount;     // ★ 追加
		ID3D11VertexShader* VertexShader; //頂点シェーダーオブジェクト
		ID3D11PixelShader* PixelShader; //ピクセルシェーダーオブジェクト
		ID3D11InputLayout* VertexLayout; //頂点レイアウトオブジェクト

	public:
		HRESULT Init(void);
		void Finalize(void);
		void Update(void);
		void Draw(void);
};
