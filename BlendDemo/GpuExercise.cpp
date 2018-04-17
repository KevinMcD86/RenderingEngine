//***************************************************************************************
// GpuExercise.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "GpuExercise.h"
#include "Effects.h"
#include <algorithm>
#include <vector>
#include <cassert>

using Microsoft::WRL::ComPtr;

GpuExercise::GpuExercise(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
	md3dDevice = device;

	BuildResources(cmdList);

	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPSOs();
}

void GpuExercise::BuildResources(ID3D12GraphicsCommandList* cmdList)
{
	// Random vectors with magnitudes between [1, 10]
	for (int i = 0; i < 64; i++)
	{
		int mag = MathHelper::Rand(1, 11);
		float theta = MathHelper::RandF(0, 2 * 3.141592);
		float phi = MathHelper::RandF(0, 2 * 3.141592);

		InputData data;
		data.v1.x = mag * cos(theta) * sin(phi);
		data.v1.y = mag * sin(theta) * sin(phi);
		data.v1.z = mag * cos(phi);

		//data.mag = mag;

		mDatas.push_back(data);
	}

	UINT64 inputByteSize = mDatas.size() * sizeof(InputData);
	UINT64 outputByteSize = mDatas.size() * sizeof(OutputData);

	// Input SRV
	mInputBuffer = d3dUtil::CreateDefaultBuffer(
		md3dDevice,
		cmdList,
		mDatas.data(),
		inputByteSize,
		mInputUploadBuffer);

	// Output UAV
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(outputByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&mOutputBuffer)));

	// System memory buffer to read results from
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(outputByteSize),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&mReadBackBuffer)));
}

void GpuExercise::BuildRootSignature()
{
	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsShaderResourceView(0);
	slotRootParameter[1].InitAsUnorderedAccessView(0);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_NONE);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void GpuExercise::BuildShadersAndInputLayout()
{
	mShaders["exerciseCS"] = d3dUtil::CompileShader(L"Shaders\\Exercise.hlsl", nullptr, "exerciseCS", "cs_5_0");
}

void GpuExercise::BuildPSOs()
{
	//
	// PSO for exercise from book
	//
	D3D12_COMPUTE_PIPELINE_STATE_DESC exercisePSO = {};
	exercisePSO.pRootSignature = mRootSignature.Get();
	exercisePSO.CS =
	{
		reinterpret_cast<BYTE*>(mShaders["exerciseCS"]->GetBufferPointer()),
		mShaders["exerciseCS"]->GetBufferSize()
	};
	exercisePSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateComputePipelineState(&exercisePSO, IID_PPV_ARGS(&mPSOs["exercise"])));
}

void GpuExercise::Run(
	ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(mPSOs["exercise"].Get());
	cmdList->SetComputeRootSignature(mRootSignature.Get());

	cmdList->SetComputeRootShaderResourceView(0, mInputBuffer->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(1, mOutputBuffer->GetGPUVirtualAddress());

	cmdList->Dispatch(64, 1, 1);

	CopyRunResultsToCPU(cmdList);
	OutputResultsToFile();
}

void GpuExercise::CopyRunResultsToCPU(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		mOutputBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_SOURCE));

	cmdList->CopyResource(mReadBackBuffer.Get(), mOutputBuffer.Get());

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		mOutputBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_COMMON));
}

void GpuExercise::OutputResultsToFile()
{
	OutputData* mappedData = nullptr;
	ThrowIfFailed(mReadBackBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));

	std::ofstream fout("results.txt");

	for (int i = 0; i < 64; i++)
	{
		fout << "( " << mDatas[i].v1.x << ", " << mDatas[i].v1.y << ", " << mDatas[i].v1.z << " ) = ";

		//fout << "Expectd: " << mDatas[i].mag << " Actual: ";

		fout << mappedData[i].mag << std::endl;
	}

	mReadBackBuffer->Unmap(0, nullptr);
}



