
#include "Common.hlsl"

Texture2D g_Texture : register(t0); 
SamplerState g_SamplerState : register(s0);


void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 normal = normalize(In.Normal); //ピクセルの法線を正規化
    float light = -dot(normal.xyz, normalize(Light.Direction.xyz)); //光源計算をする

	//テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

    outDiffuse.rgb *= (In.Diffuse.rgb * light); //明るさを乗算
    outDiffuse.rgb += Light.Ambient.rgb; //環境光を追加
    outDiffuse.a *= In.Diffuse.a; //αに明るさは関係ないので別計算

	//カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); //正規化する

	//ハーフベクトルの作成
    float3 halfv = eyev + normalize(Light.Direction.xyz); //視線とライトベクトルを加算
    halfv = normalize(halfv); //正規化する
    float specular = -dot(halfv, normal.xyz); //ハーフベクトルと法線の内積を計算
    specular = saturate(specular); //サチュレートする
    specular = pow(specular, 30);
    outDiffuse.rgb += specular; //足しこむ
//-------------------------------------------------------------------    

	//半球ライティング
    float norm = dot(normal, Light.GroundNormal); //地面とピクセル法線の内積
    norm = (norm + 1.0f) / 2.0f; //内積をスケーリング
    float3 hemiColor = lerp(Light.GroundColor, Light.SkyColor, norm); //色の補間
    
    outDiffuse.rgb += hemiColor; //まとめて足しこむ





}




/*
void main(in PS_IN In, out float4 outDiffuse : SV_Target )
{
    //法線を正規化
    float4 normal = normalize(In.Normal);
    //光源計算
    float light = -dot(normal.xyz, Light.Direction.xyz);
    
    outDiffuse = 1.0f;//とりあず白で試す

    outDiffuse.rgb *= light;
    outDiffuse.a = In.Diffuse.a;
    
    
    
    
    
    
    //半球ライティング
    float norm = dot( normal, Light.GroundNormal);
    norm = (norm + 1.0f) / 2.0f;
    float3 hemiColor = lerp(Light.GroundColor, Light.SkyColor, norm);
    
    outDiffuse.rgb += hemiColor;

    return;
    
    
    
    //lightをサチュレート
    light = saturate(light);
    
    outDiffuse.rgba = g_Texture.Sample(g_SamplerState, In.TexCoord);//テクセルを取得
    //テクセルに明るさを乗算する
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;//α値は別処理
//--------------------------------------------------------------
    
    //カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); //正規化する

    //ハーフベクトル
    float3 halfv = eyev + Light.Direction.xyz;
    halfv = normalize(halfv);
    
    float specular = -dot( halfv, normal.xyz ); //鏡面反射の計算
    specular = saturate(specular); //値をサチュレート    
    specular = pow(specular, 30);

    outDiffuse.rgb += specular;
   
}

*/







