// 定数バッファ
struct LIGHT
{
    bool Enable;
    bool3 Dummy;
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
    float4 Position;
    float4 PointLightParam;
    float4 SkyColor;
    float4 GroundColor;
    float4 GroundNormal;
};

// ★追加：プログラム側から色（マテリアル）を受け取る
struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    float3 Dummy;
};

cbuffer MaterialBuffer : register(b3)
{
    MATERIAL Material;
}

cbuffer LightBuffer : register(b4)
{
    LIGHT Light;
}
cbuffer CameraBuffer : register(b5)
{
    float4 CameraPosition;
}

struct PS_IN
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION0;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PS_IN In) : SV_TARGET
{
    float3 normal = normalize(In.Normal.xyz);
    float3 lightDir = normalize(Light.Direction.xyz);
    float3 viewDir = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    
    // ① デフューズ：マテリアルの色を使う
    float baseLuminance = max(-dot(normal, lightDir), 0.0f);
    float3 diffuse = Material.Diffuse.rgb * baseLuminance;

    // ② スペキュラ：マテリアルの鏡面反射色を使う
    float3 halfVector = normalize(viewDir - lightDir);
    float specularIntensity = pow(max(dot(normal, halfVector), 0.0f), 30.0f);
    float3 specular = Material.Specular.rgb * specularIntensity;

    // ③ 環境光：マテリアルのアンビエント色を使う
    float3 ambient = Material.Ambient.rgb;

    float4 finalColor = float4(diffuse + specular + ambient, Material.Diffuse.a);

    // ブルーム用ハイライト増幅（スペキュラがある場合のみ）
    if (specularIntensity > 0.5f)
    {
        finalColor.rgb += specular * 3.0f;
    }

    return finalColor;
}