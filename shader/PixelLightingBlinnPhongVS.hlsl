
#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp; // World * View * Projection
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);

    //------------------------------
    float4 worldNormal, normal;

    normal = float4(In.Normal.xyz, 0.0f);//w要素は0にする
    worldNormal = mul(normal, World);//法線を回転する
    worldNormal = normalize(worldNormal);//正規化する
    Out.Normal = worldNormal;//法線を出力

    Out.Diffuse = In.Diffuse;//頂点色
    Out.TexCoord = In.TexCoord;//テクスチャ座標の出力  
    
    //ワールド変換だけした頂点座標を出力
    Out.WorldPosition = mul( In.Position, World );
    
}






