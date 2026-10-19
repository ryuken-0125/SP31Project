#include "PostEffect.h"

HRESULT PostEffect::Init()
{
    // 1. テクスチャを4枚作成（ブラー用は少し軽くするためにサイズを半分にしてもOKですが、今回は等倍にします）
    CreateRenderTarget(SCREEN_WIDTH, SCREEN_HEIGHT, &m_OriginalRTV, &m_OriginalSRV);
    CreateRenderTarget(SCREEN_WIDTH, SCREEN_HEIGHT, &m_LuminanceRTV, &m_LuminanceSRV);
    CreateRenderTarget(SCREEN_WIDTH, SCREEN_HEIGHT, &m_BlurXRTV, &m_BlurXSRV);
    CreateRenderTarget(SCREEN_WIDTH, SCREEN_HEIGHT, &m_BlurYRTV, &m_BlurYSRV);

    // 2. 画面全体ポリゴンの作成
    VERTEX_3D vertex[4];
    float w = 1.0f;
    float h = 1.0f;
    vertex[0].Position = XMFLOAT3(-w, h, 0.0f); vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
    vertex[1].Position = XMFLOAT3(w, h, 0.0f); vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
    vertex[2].Position = XMFLOAT3(-w, -h, 0.0f); vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
    vertex[3].Position = XMFLOAT3(w, -h, 0.0f); vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);
    for (int i = 0; i < 4; i++) {
        vertex[i].Normal = XMFLOAT3(0, 0, -1);
        vertex[i].Diffuse = XMFLOAT4(1, 1, 1, 1);
    }

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = vertex;
    GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

    // 3. シェーダーの読み込み
    CreateVertexShader(&m_VertexShader, &m_VertexLayout, "PostEffectVS.cso");
    CreatePixelShader(&m_LuminancePS, "LuminancePS.cso");
    CreatePixelShader(&m_BlurXPS, "BlurXPS.cso");
    CreatePixelShader(&m_BlurYPS, "BlurYPS.cso");
    CreatePixelShader(&m_BlendPS, "BloomBlendPS.cso");

    return S_OK;
}

void PostEffect::Uninit()
{
    SAFE_RELEASE(m_BlendPS);
    SAFE_RELEASE(m_BlurYPS);
    SAFE_RELEASE(m_BlurXPS);
    SAFE_RELEASE(m_LuminancePS);
    SAFE_RELEASE(m_VertexShader);
    SAFE_RELEASE(m_VertexLayout);
    SAFE_RELEASE(m_VertexBuffer);

    SAFE_RELEASE(m_BlurYSRV); SAFE_RELEASE(m_BlurYRTV);
    SAFE_RELEASE(m_BlurXSRV); SAFE_RELEASE(m_BlurXRTV);
    SAFE_RELEASE(m_LuminanceSRV); SAFE_RELEASE(m_LuminanceRTV);
    SAFE_RELEASE(m_OriginalSRV); SAFE_RELEASE(m_OriginalRTV);
}

void PostEffect::BeginRenderScene()
{
    SetRenderTarget(m_OriginalRTV, GetDepthStencilView());
    ClearRenderTarget(m_OriginalRTV, GetDepthStencilView());
}

void PostEffect::EndRenderScene()
{
    ID3D11DeviceContext* context = GetDeviceContext();
    SetDepthEnable(false);
    ResetWorldViewProjection3D();

    context->IASetInputLayout(m_VertexLayout);
    context->VSSetShader(m_VertexShader, NULL, 0);

    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };

    // ==========================================
    // パス1：高輝度抽出
    // 描画先を m_LuminanceRTV に変更し、元画像を読み込んで輝度抽出シェーダーで描画
    // ==========================================
    SetRenderTarget(m_LuminanceRTV, nullptr); // 2D処理にZバッファは不要
    context->PSSetShader(m_LuminancePS, NULL, 0);
    context->PSSetShaderResources(0, 1, &m_OriginalSRV);
    context->Draw(4, 0);

    // テクスチャのロックを解除（これをしないと次のパスでエラーになります）
    context->PSSetShaderResources(0, 2, nullSRV);

    // ==========================================
    // パス2：横方向ブラー
    // 描画先を m_BlurXRTV に変更し、輝度画像を読み込んで横にぼかす
    // ==========================================
    SetRenderTarget(m_BlurXRTV, nullptr);
    context->PSSetShader(m_BlurXPS, NULL, 0);
    context->PSSetShaderResources(0, 1, &m_LuminanceSRV);
    context->Draw(4, 0);
    context->PSSetShaderResources(0, 2, nullSRV);

    // ==========================================
    // パス3：縦方向ブラー
    // 描画先を m_BlurYRTV に変更し、横ぼかし画像を読み込んでさらに縦にぼかす
    // ==========================================
    SetRenderTarget(m_BlurYRTV, nullptr);
    context->PSSetShader(m_BlurYPS, NULL, 0);
    context->PSSetShaderResources(0, 1, &m_BlurXSRV);
    context->Draw(4, 0);
    context->PSSetShaderResources(0, 2, nullSRV);

    // ==========================================
    // パス4：ブルーム合成（本来の画面へ出力）
    // 描画先をバックバッファに戻し、元画像(t0)とブラー画像(t1)を合成する
    // ==========================================
    RestoreRenderTarget();
    context->PSSetShader(m_BlendPS, NULL, 0);

    ID3D11ShaderResourceView* blendSRVs[2] = { m_OriginalSRV, m_BlurYSRV };
    context->PSSetShaderResources(0, 2, blendSRVs);
    context->Draw(4, 0);

    context->PSSetShaderResources(0, 2, nullSRV);
}