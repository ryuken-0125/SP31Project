#pragma once
#include "main.h"
#include "renderer.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }
#endif

class PostEffect
{
private:
    // 描画先テクスチャ（4枚に増えます）
    ID3D11RenderTargetView* m_OriginalRTV = nullptr;  // 元のシーン
    ID3D11RenderTargetView* m_LuminanceRTV = nullptr; // 高輝度抽出用
    ID3D11RenderTargetView* m_BlurXRTV = nullptr;     // 横ブラー用
    ID3D11RenderTargetView* m_BlurYRTV = nullptr;     // 縦ブラー用

    // 読み込み用テクスチャリソース
    ID3D11ShaderResourceView* m_OriginalSRV = nullptr;
    ID3D11ShaderResourceView* m_LuminanceSRV = nullptr;
    ID3D11ShaderResourceView* m_BlurXSRV = nullptr;
    ID3D11ShaderResourceView* m_BlurYSRV = nullptr;

    ID3D11Buffer* m_VertexBuffer = nullptr;
    ID3D11InputLayout* m_VertexLayout = nullptr;
    ID3D11VertexShader* m_VertexShader = nullptr;

    // ピクセルシェーダー
    ID3D11PixelShader* m_LuminancePS = nullptr;
    ID3D11PixelShader* m_BlurXPS = nullptr;
    ID3D11PixelShader* m_BlurYPS = nullptr;
    ID3D11PixelShader* m_BlendPS = nullptr;

public:
    HRESULT Init();
    void Uninit();
    void BeginRenderScene();
    void EndRenderScene();
};