struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

// 受け取り側（LuminancePSなど）と同じ構造に合わせます
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
    float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    // 行列変換を一切行わず、入力された頂点座標をそのまま画面の2D座標として出力します
    vout.pos = float4(vin.pos, 1.0f);
    
    // ピクセルシェーダーがエラーを起こさないように、一応データを入れて渡します
    vout.normal = float4(vin.normal, 0.0f);
    vout.color = vin.color;
    vout.tex = vin.tex;
    
    return vout;
}