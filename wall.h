#pragma once

// 16バイトアライメントを維持するためのスポットライト構造体
struct WallSpotLight {
    XMFLOAT4 Position;  // wは不使用
    XMFLOAT4 Direction; // wは不使用
    XMFLOAT4 Color;
    float CosInner;     // 内側コーン角度のコサイン値
    float CosOuter;     // 外側コーン角度のコサイン値
    float Padding[2];   // 16バイト境界アライメント用パディング
};

// 壁専用のライティング定数バッファ構造体（16バイトアライメント厳守）
struct WallLightingConstantBuffer {
    // CameraPosition は削除
    XMFLOAT4 PointLightPosition;
    XMFLOAT4 PointLightColor;
    XMFLOAT4 RimColor;
    float RimPower;
    float PointLightRadius;
    float Padding[2];
    WallSpotLight SpotLights[3];
};

class Wall
{
private:
    ID3D11Buffer* m_VertexBuffer = nullptr;
    ID3D11InputLayout* m_VertexLayout = nullptr;
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;

    // ライト情報をシェーダーに送るための定数バッファ
    ID3D11Buffer* m_ConstantBuffer = nullptr;

    XMFLOAT3 m_Position;
    XMFLOAT3 m_Rotation;
    XMFLOAT3 m_Scale;

public:
    void Init();
    void Uninit();
    void Update();
    void Draw();
};