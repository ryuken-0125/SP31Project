#include "main.h"
#include "renderer.h"
#include "wall.h"
#include <cmath>

void Wall::Init()
{
    m_Position = { 0.0f, 0.0f, 2.0f }; // スポットライトが当たりやすいように少し奥に配置
    m_Rotation = { 0.0f, 0.0f, 0.0f };
    m_Scale = { 5.0f, 3.0f, 1.0f }; // 横に長い壁にする

    VERTEX_3D vertex[4];
    vertex[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
    vertex[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);  vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
    vertex[2].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
    vertex[3].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);  vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

    for (int i = 0; i < 4; i++) {
        vertex[i].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f); // 手前を向く法線
        vertex[i].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = vertex;


    GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

    CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\lightingVS.cso");
    CreatePixelShader(&m_PixelShader, "shader\\lightingPS.cso");

    D3D11_BUFFER_DESC cbd{};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(WallLightingConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    GetDevice()->CreateBuffer(&cbd, NULL, &m_ConstantBuffer);
}

void Wall::Update()
{
}

void Wall::Draw()
{

    GetDeviceContext()->IASetInputLayout(m_VertexLayout);
    GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    XMMATRIX world = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
        XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) *
        XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);


    SetWorldMatrix(world);

    WallLightingConstantBuffer cb{};


    cb.PointLightPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    cb.PointLightColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    cb.PointLightRadius = 5.0f;

    cb.RimColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    cb.RimPower = 3.0f;

    // ライト1: 左側（赤）
    cb.SpotLights[0].Position = XMFLOAT4(-2.0f, 3.0f, -1.0f, 1.0f);
    cb.SpotLights[0].Direction = XMFLOAT4(0.3f, -0.8f, 1.0f, 0.0f);
    cb.SpotLights[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    cb.SpotLights[0].CosInner = cosf(XMConvertToRadians(2.0f));  
    cb.SpotLights[0].CosOuter = cosf(XMConvertToRadians(10.0f)); 

    // ライト2: 中央（緑）
    cb.SpotLights[1].Position = XMFLOAT4(0.0f, 3.0f, -1.0f, 1.0f);
    cb.SpotLights[1].Direction = XMFLOAT4(0.0f, -0.8f, 1.0f, 0.0f);
    cb.SpotLights[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    cb.SpotLights[1].CosInner = cosf(XMConvertToRadians(2.0f)); 
    cb.SpotLights[1].CosOuter = cosf(XMConvertToRadians(10.0f));

    // ライト3: 右側（青）
    cb.SpotLights[2].Position = XMFLOAT4(2.0f, 3.0f, -1.0f, 1.0f);
    cb.SpotLights[2].Direction = XMFLOAT4(-0.3f, -0.8f, 1.0f, 0.0f);
    cb.SpotLights[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
    cb.SpotLights[2].CosInner = cosf(XMConvertToRadians(2.0f));  
    cb.SpotLights[2].CosOuter = cosf(XMConvertToRadians(10.0f)); 

    GetDeviceContext()->UpdateSubresource(m_ConstantBuffer, 0, NULL, &cb, 0, 0);
    GetDeviceContext()->PSSetConstantBuffers(7, 1, &m_ConstantBuffer);
    GetDeviceContext()->VSSetConstantBuffers(7, 1, &m_ConstantBuffer);

    UINT stride = sizeof(VERTEX_3D), offset = 0;

    GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    GetDeviceContext()->Draw(4, 0);
}

void Wall::Uninit()
{
    if (m_VertexBuffer) m_VertexBuffer->Release();
    if (m_VertexLayout) m_VertexLayout->Release();
    if (m_VertexShader) m_VertexShader->Release();
    if (m_PixelShader)  m_PixelShader->Release();
    if (m_ConstantBuffer) m_ConstantBuffer->Release();
}