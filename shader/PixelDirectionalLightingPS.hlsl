
#include "Common.hlsl"

Texture2D g_Texture : register(t0); 
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target )
{
    //法線を正規化
    float4 normal = normalize(In.Normal);
    //光源計算
    float light = -dot(normal.xyz, Light.Direction.xyz);
    //lightをサチュレート
    light = saturate(light);
    
    outDiffuse.rgba = g_Texture.Sample(g_SamplerState, In.TexCoord);//テクセルを取得
    //テクセルに明るさを乗算する
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.a = In.Diffuse.a;//α値は別処理

    //カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); //正規化する

    //光の反射ベクトルを計算
    float3 refv = reflect(Light.Direction.xyz, normal.xyz);
    refv = normalize(refv); //正規化する    
    
    float specular = -dot( eyev, refv ); //鏡面反射の計算
    specular = saturate(specular); //値をサチュレート    
    specular = pow(specular, 20);

    outDiffuse.rgb += specular;
   
}









