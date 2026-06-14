#include "shader/Common.hlsl"

// C++側の WallSpotLight と一致
struct SpotLight
{
    float4 Position;
    float4 Direction;
    float4 Color;
    float CosInner;
    float CosOuter;
    float2 Padding;
};

// スロット7で受け取る
cbuffer WallLightingBuffer : register(b7)
{
    float4 PointLightPosition;
    float4 PointLightColor;
    float4 RimColor;
    float RimPower;
    float PointLightRadius;
    float2 BufferPadding;
    SpotLight SpotLights[3];
};

// テクスチャとサンプラ
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

float4 main(PS_IN In) : SV_Target
{
    // 警告 X3206 の修正：In.Normal は float4 なので .xyz を明示して float3 に合わせる
    float3 N = normalize(In.Normal.xyz);
    float3 V = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    
    // エラー X3004 の修正：Material は使わず、直接テクスチャの色を取得する
    float4 textureColor = g_Texture.Sample(g_SamplerState, In.TexCoord);

    float3 totalDiffuse = float3(0.0f, 0.0f, 0.0f);

    // -------------------------------------------------------------
    // 1. ポイントライトの計算
    // -------------------------------------------------------------
    float3 pointLightVec = PointLightPosition.xyz - In.WorldPosition.xyz;
    float pointDist = length(pointLightVec);
    float3 pointL = normalize(pointLightVec);
    
    float pointAtten = max(0.0f, 1.0f - (pointDist / PointLightRadius));
    float pointNDotL = max(dot(N, pointL), 0.0f);
    totalDiffuse += PointLightColor.xyz * pointNDotL * pointAtten;

    // -------------------------------------------------------------
    // 2. スポットライトの計算
    // -------------------------------------------------------------
    for (int i = 0; i < 3; i++)
    {
        float3 spotLightVec = SpotLights[i].Position.xyz - In.WorldPosition.xyz;
        float3 spotL = normalize(spotLightVec);
        
        float3 lightToPixel = -spotL;
        float spotCos = dot(lightToPixel, normalize(SpotLights[i].Direction.xyz));
        
        float spotFactor = smoothstep(SpotLights[i].CosOuter, SpotLights[i].CosInner, spotCos);
        float spotNDotL = max(dot(N, spotL), 0.0f);
        totalDiffuse += SpotLights[i].Color.xyz * spotNDotL * spotFactor;
    }

    // -------------------------------------------------------------
    // 3. リムライトの計算
    // -------------------------------------------------------------
    float rimFactor = 1.0f - max(dot(V, N), 0.0f);
    float3 rimLight = RimColor.xyz * pow(rimFactor, RimPower);

    // 修正: Material.Ambient ではなく、システム側に定義されている Light.Ambient を使う
    float3 finalColor = textureColor.xyz * (totalDiffuse + Light.Ambient.xyz) + rimLight;

    return float4(finalColor, textureColor.a * In.Diffuse.a);
}