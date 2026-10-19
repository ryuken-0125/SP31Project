#include "Field3D.h"
#include "texture.h"
#include <vector> // ★ std::vector を使うために追加
#include <cmath>

HRESULT Field3D::Init(void)
{
	TexID = TextureLoad(L"asset\\texture\\sura.jpg");

	CreateVertexShader(&VertexShader, &VertexLayout, "PixelDirectionalLightingVS.cso");
	CreatePixelShader(&PixelShader, "PixelDirectionalLightingPS.cso");

	// ==============================================================
	// ★ 地面を細かく分割し、凹凸のある地形データを生成する
	// ==============================================================
	const int GRID_SIZE = 50;   // 50マス×50マスに分割
	const float SIZE = 20.0f;   // 地面の全体の広さ

	std::vector<VERTEX_3D> vertices;
	std::vector<UINT> indices;

	// 1. 頂点の生成
	for (int z = 0; z <= GRID_SIZE; z++) {
		for (int x = 0; x <= GRID_SIZE; x++) {
			VERTEX_3D v;

			// XとZの座標を計算（中心を0,0にする）
			float px = (x - GRID_SIZE / 2.0f) * (SIZE / GRID_SIZE);
			float pz = (z - GRID_SIZE / 2.0f) * (SIZE / GRID_SIZE);

			// ★ 高さ（Y座標）の計算：波を組み合わせて自然な起伏を作る
			// （※起伏が激しすぎると影がめり込むので、高さは 0.05f など低めに抑えています）
			float py = sinf(px * 2.0f) * 0.05f + cosf(pz * 2.0f) * 0.05f
				+ sinf(px * 5.0f + pz * 3.0f) * 0.02f;

			v.Position = XMFLOAT3(px, py, pz);

			// テクスチャ座標（全体で5回画像をリピートさせる）
			v.TexCoord = XMFLOAT2((float)x / GRID_SIZE * 5.0f, (float)z / GRID_SIZE * 5.0f);
			v.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// 起伏に沿って光が当たるように法線（傾き）を計算
			float dx = (sinf((px + 0.01f) * 2.0f) * 0.05f + cosf(pz * 2.0f) * 0.05f + sinf((px + 0.01f) * 5.0f + pz * 3.0f) * 0.02f) - py;
			float dz = (sinf(px * 2.0f) * 0.05f + cosf((pz + 0.01f) * 2.0f) * 0.05f + sinf(px * 5.0f + (pz + 0.01f) * 3.0f) * 0.02f) - py;
			XMVECTOR norm = XMVector3Normalize(XMVectorSet(-dx, 0.01f, -dz, 0.0f));
			XMStoreFloat3(&v.Normal, norm);

			vertices.push_back(v);
		}
	}

	// 2. インデックス（頂点の結び順）の生成
	for (int z = 0; z < GRID_SIZE; z++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			int idx = z * (GRID_SIZE + 1) + x;

			// 1つの四角形を2つの三角形（6つの頂点）で結ぶ
			indices.push_back(idx);
			indices.push_back(idx + (GRID_SIZE + 1));
			indices.push_back(idx + 1);

			indices.push_back(idx + 1);
			indices.push_back(idx + (GRID_SIZE + 1));
			indices.push_back(idx + (GRID_SIZE + 1) + 1);
		}
	}

	IndexCount = (UINT)indices.size();

	// 3. バッファの作成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * (UINT)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertices.data();
	GetDevice()->CreateBuffer(&bd, &sd, &VertexBuffer);

	bd.ByteWidth = sizeof(UINT) * IndexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	sd.pSysMem = indices.data();
	GetDevice()->CreateBuffer(&bd, &sd, &IndexBuffer);

	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

void Field3D::Finalize(void)
{
	if (VertexLayout) VertexLayout->Release();
	if (VertexShader) VertexShader->Release();
	if (PixelShader) PixelShader->Release();
	if (VertexBuffer) VertexBuffer->Release();
	if (IndexBuffer) IndexBuffer->Release();
}

void Field3D::Update(void) {}

void Field3D::Draw(void)
{
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	ID3D11ShaderResourceView* tex = GetTexture(TexID);
	GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

	XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotate.x), XMConvertToRadians(Rotate.y), XMConvertToRadians(Rotate.z));
	XMMATRIX ScalingMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
	SetWorldMatrix(WorldMatrix);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	MATERIAL material;
	ZeroMemory(&material, sizeof(MATERIAL));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f); // 影になじむように暗めに設定
	SetMaterial(material);

	// ★ 修正：インデックスを使って描画する DrawIndexed に変更
	GetDeviceContext()->DrawIndexed(IndexCount, 0, 0);
}