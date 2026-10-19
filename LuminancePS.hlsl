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
    float4 color = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    // 輝度の計算
    float luminance = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
    
    // ① まぶしい強い光を取り出す（既存のブルーム）
    float bloomFactor = saturate((luminance - 0.8f) * 5.0f);
    float3 brightColor = color.rgb * bloomFactor;
    
    // ② ★追加：画面全体に光の靄（もや）をかける
    // 画面全体の景色を 15% ほど抽出してぼかし処理に回し、少しだけ青白い空気感を足す
    float3 fogColor = color.rgb * 0.15f + float3(0.02f, 0.02f, 0.05f);
    
    // まぶしい光と靄を合わせてブラー処理へ送る
    return float4(brightColor + fogColor, color.a);
}