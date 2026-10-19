Texture2D g_TextureOriginal : register(t0); // 0番: 元画像
Texture2D g_TextureBlur : register(t1); // 1番: ブラー画像
SamplerState g_SamplerState : register(s0);

struct PS_IN
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PS_IN In) : SV_TARGET
{
    float4 colorOrig = g_TextureOriginal.Sample(g_SamplerState, In.TexCoord);
    float4 colorBlur = g_TextureBlur.Sample(g_SamplerState, In.TexCoord);
    
    // 加算合成（1.5fはブルームの強さ。お好みで調整してください）
    float4 finalColor = colorOrig + colorBlur * 3.0f;
    finalColor.a = 1.0f;
    return finalColor;
}