
#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp; // World * View * Projection
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);

    //------------------------------
    //光源計算　PDFの17ページからの処理
    float4 worldNormal, normal;

    normal = float4(In.Normal.xyz, 0.0f);//w要素は0にする
////------------------------------------
//    normal.x = In.Normal.x;
//    normal.y = In.Normal.y;
//    normal.z = In.Normal.z;
//    normal.w = 0.0f;
////-------------------------------------    
//    normal.xyz = In.Normal.xyz;
//    normal.w = 0.0f;
    worldNormal = mul(normal, World);//法線を回転する
    worldNormal = normalize(worldNormal);//正規化する
    Out.Normal = worldNormal;//法線を出力

    //明るさを計算 光ベクトルと法線の内積計算
    float light = -dot(Light.Direction.xyz, worldNormal.xyz);
    light = saturate(light);    //明るさを0.0～1.0にする
    //lightに頂点の明るさができる
    //-------------------------------

    //明るさの出力
    Out.Diffuse.rgb = light * In.Diffuse;  //明るさ*頂点色
    //α値の出力
    Out.Diffuse.a = In.Diffuse.a;   //αは頂点カラーを使う
    //テクスチャ座標の出力
    Out.TexCoord = In.TexCoord;
    
}






