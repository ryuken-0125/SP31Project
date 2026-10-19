#include "IronSphere.h"

HRESULT IronSphere::Init(void)
{
    // ブリン・フォン反射のシェーダーを再利用して、金属の光沢を表現します
    CreateVertexShader(&VertexShader, &VertexLayout, "PixelLightingBlinnPhongVS.cso");
    CreatePixelShader(&PixelShader, "IronSpherePS.cso");

    // 床の上（Y軸に少し浮かせた位置）に配置
    Position = XMFLOAT3(0.0f, 0.3f, 0.0f);
    Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Scale = XMFLOAT3(0.5f, 0.5f, 0.5f);

    // ★球体のモデルを読み込む（ご自身の環境のファイル名に合わせてください）
    Model = ModelLoad("asset\\model\\ball.fbx");

    // ★光の設定：左上側から降る光
    // Xがプラス（右）へ、Yがマイナス（下）へ、Zがプラス（奥）へ向かうベクトル
    XMVECTOR dir = XMVectorSet(1.0f, -1.0f, 0.5f, 0.0f);
    dir = XMVector3Normalize(dir);
    XMStoreFloat4(&Light.Direction, dir);

    // 鉄のような鋭い反射をさせるため、光を少し強めに
    Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    Light.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f); // 暗めの環境光で金属特有の重厚感を出す

    return S_OK;
}

void IronSphere::Finalize(void)
{
    if (VertexLayout) VertexLayout->Release();
    if (VertexShader) VertexShader->Release();
    if (PixelShader)  PixelShader->Release();
    ModelRelease(Model);
}

void IronSphere::Update(void)
{
    // ゆっくり回転させて、光の反射（ハイライト）が美しく動くのを確認できるようにします
    Rotate.y += 0.5f;
}

void IronSphere::Draw(void)
{
    GetDeviceContext()->IASetInputLayout(VertexLayout);
    GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
    GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

    SetLight(Light);

    // 基本となる行列の計算
    XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
    XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotate.x), XMConvertToRadians(Rotate.y), XMConvertToRadians(Rotate.z));
    XMMATRIX ScalingMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
    XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // --------------------------------------------------------
    // ① 鉄球本体の描画
    // --------------------------------------------------------
    SetWorldMatrix(WorldMatrix);

    MATERIAL material;
    ZeroMemory(&material, sizeof(MATERIAL));
    material.Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);  // 鉄のグレー
    material.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // 強い光沢
    material.Ambient = XMFLOAT4(0.1f, 0.12f, 0.15f, 1.0f); // 環境光
    SetMaterial(material);

    ModelDraw(Model);

    // --------------------------------------------------------
    // ② 影（平面シャドウ）の描画
    // --------------------------------------------------------
    // 床の平面（チラつき防止で 0.01f 浮かせる）
    XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, -0.01f);

    XMVECTOR lightDir = XMLoadFloat4(&Light.Direction);

    // ★修正：光の発生源に向かうベクトルにするため、lightDir にマイナスをつけます
    XMMATRIX ShadowMatrix = XMMatrixShadow(shadowPlane, -lightDir);

    XMMATRIX WorldShadowMatrix = WorldMatrix * ShadowMatrix;
    SetWorldMatrix(WorldShadowMatrix);

    // 影用のマテリアル（光沢ゼロ、半透明の黒）
    MATERIAL shadowMaterial;
    ZeroMemory(&shadowMaterial, sizeof(MATERIAL));
    shadowMaterial.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.6f);
    shadowMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    shadowMaterial.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    SetMaterial(shadowMaterial);

    ModelDraw(Model);
}