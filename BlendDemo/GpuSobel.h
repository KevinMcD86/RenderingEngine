//***************************************************************************************
// GpuWaves.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs the calculations for the wave simulation using the ComputeShader on the GPU.  
// The solution is saved to a floating-point texture.  The client must then set this 
// texture as a SRV and do the displacement mapping in the vertex shader over a grid.
//***************************************************************************************

#ifndef GPUSOBEL_H
#define GPUSOBEL_H

#include "../../../Common/d3dUtil.h"
#include "../../../Common/GameTimer.h"

struct InputData
{
	DirectX::XMFLOAT3 v1;
	//float mag;
};

struct OutputData
{
	float mag;
};

class GpuSobel
{
public:
	GpuSobel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	GpuSobel(const GpuSobel& rhs) = delete;
	GpuSobel& operator=(const GpuSobel& rhs) = delete;
	~GpuSobel() = default;

	void BuildResources(ID3D12GraphicsCommandList* cmdList);
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPSOs();

	void Run(ID3D12GraphicsCommandList* cmdList);

	void OutputResultsToFile();

private:
	void CopyRunResultsToCPU(ID3D12GraphicsCommandList* cmdList);

	std::vector<InputData> mDatas;

	ID3D12Device* md3dDevice = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

	Microsoft::WRL::ComPtr<ID3D12Resource> mInputBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mOutputBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mInputUploadBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> mReadBackBuffer = nullptr;
};

#endif // GPUSOBEL_H
