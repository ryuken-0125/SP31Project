Texture2D g_Texture : register(t0);
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
    float4 color = float4(0, 0, 0, 0);
    float2 offset = float2(0.0f, 1.0f / 720.0f); // 720は画面高さ。縦方向への移動
    
    float weight[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
    
    color += g_Texture.Sample(g_SamplerState, In.TexCoord) * weight[0];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord + offset * 1.0f) * weight[1];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord - offset * 1.0f) * weight[1];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord + offset * 2.0f) * weight[2];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord - offset * 2.0f) * weight[2];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord + offset * 3.0f) * weight[3];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord - offset * 3.0f) * weight[3];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord + offset * 4.0f) * weight[4];
    color += g_Texture.Sample(g_SamplerState, In.TexCoord - offset * 4.0f) * weight[4];
    
    return color;
}