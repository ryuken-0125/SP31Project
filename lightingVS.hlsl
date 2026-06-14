#include "shader/Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    // 頂点座標をワールド空間、ビュー空間、プロジェクション空間へ変換
    float4 worldPos = mul(In.Position, World);
    float4 viewPos = mul(worldPos, View);
    Out.Position = mul(viewPos, Projection);

    // ピクセルシェーダーにワールド空間上の正確な座標を渡す
    Out.WorldPosition = worldPos;

    // 修正箇所：法線ベクトルを float3 として計算し、最後に float4 に変換して代入する
    Out.Normal = float4(normalize(mul(In.Normal.xyz, (float3x3) World)), 0.0f);

    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;
}