#include "Audio.h"
#include "D3D12Common.h"
#include <fstream>
#include <cassert>

void Audio::StaticInitialize()
{
	// COMを初期化
	Result result = CoInitialize(NULL);
}

void Audio::Initialize(const std::wstring& fileName)
{
	Result result;
	// FilterGraphを生成
	result = CoCreateInstance(CLSID_FilterGraph,
		NULL, CLSCTX_INPROC,
		IID_IGraphBuilder,
		(LPVOID*)&graphBuilder);

	// MediaControlインターフェース取得
	result = graphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&mediaControl);
	result = graphBuilder->QueryInterface(IID_IMediaPosition, (LPVOID*)&mediaPosition);
	result = graphBuilder->QueryInterface(IID_IBasicAudio, (LPVOID*)&basicAudio);

	std::wstring fullpath = L"Resources/audios/" + fileName;
	// Graphを生成
	result = mediaControl->RenderFile((BSTR)fullpath.c_str());
}

void Audio::Finalize()
{
	CoUninitialize(); // COM終了
}