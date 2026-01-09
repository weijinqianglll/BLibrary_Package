// Copyright Epic Games, Inc. All Rights Reserved.

#include "BLibraryBP.h"
#include "BLibrary.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Engine/Engine.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/MessageDialog.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"

#ifdef _WIN32
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include <Shlobj.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#ifdef _WIN32
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

UBLibraryBP::UBLibraryBP(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FString UBLibraryBP::TakeScreenshot(const FString& ScreenshotName, const FString& SaveFolder, float TopPadding, float BottomPadding, float LeftPadding, float RightPadding)
{
	if (ScreenshotName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Screenshot name is empty!"));
		return TEXT("");
	}

	FString FinalSaveFolder = SaveFolder;
	if (FinalSaveFolder.IsEmpty())
	{
		FinalSaveFolder = FPaths::ProjectSavedDir() / TEXT("Screenshots");
	}

	FinalSaveFolder = FPaths::ConvertRelativePathToFull(FinalSaveFolder);
	FPaths::NormalizeDirectoryName(FinalSaveFolder);

	UE_LOG(LogTemp, Log, TEXT("Save folder: %s"), *FinalSaveFolder);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*FinalSaveFolder))
	{
		UE_LOG(LogTemp, Warning, TEXT("Directory does not exist, creating: %s"), *FinalSaveFolder);
		PlatformFile.CreateDirectoryTree(*FinalSaveFolder);
	}

	FString FileName = ScreenshotName + TEXT(".png");
	FString FullPath = FPaths::Combine(FinalSaveFolder, FileName);

	UE_LOG(LogTemp, Log, TEXT("Full path: %s"), *FullPath);

#ifdef _WIN32
	HDC hdcScreen = GetDC(NULL);
	if (!hdcScreen)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get screen DC"));
		return TEXT("");
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int captureX = FMath::RoundToInt(screenWidth * FMath::Clamp(LeftPadding, 0.0f, 1.0f));
	int captureY = FMath::RoundToInt(screenHeight * FMath::Clamp(TopPadding, 0.0f, 1.0f));
	int captureWidth = FMath::RoundToInt(screenWidth * (1.0f - FMath::Clamp(LeftPadding + RightPadding, 0.0f, 1.0f)));
	int captureHeight = FMath::RoundToInt(screenHeight * (1.0f - FMath::Clamp(TopPadding + BottomPadding, 0.0f, 1.0f)));

	if (captureWidth <= 0 || captureHeight <= 0)
	{
		ReleaseDC(NULL, hdcScreen);
		UE_LOG(LogTemp, Warning, TEXT("Invalid capture area: width=%d, height=%d"), captureWidth, captureHeight);
		return TEXT("");
	}

	UE_LOG(LogTemp, Log, TEXT("Capture area: x=%d, y=%d, width=%d, height=%d"), captureX, captureY, captureWidth, captureHeight);

	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	if (!hdcMem)
	{
		ReleaseDC(NULL, hdcScreen);
		UE_LOG(LogTemp, Warning, TEXT("Failed to create compatible DC"));
		return TEXT("");
	}

	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = captureWidth;
	bmi.bmiHeader.biHeight = captureHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HBITMAP hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
	if (!hBitmap)
	{
		DeleteDC(hdcMem);
		ReleaseDC(NULL, hdcScreen);
		UE_LOG(LogTemp, Warning, TEXT("Failed to create DIB section"));
		return TEXT("");
	}

	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
	if (!hOldBitmap)
	{
		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
		ReleaseDC(NULL, hdcScreen);
		UE_LOG(LogTemp, Warning, TEXT("Failed to select bitmap into DC"));
		return TEXT("");
	}

	BitBlt(hdcMem, 0, 0, captureWidth, captureHeight, hdcScreen, captureX, captureY, SRCCOPY);

	TArray<uint8> pixels;
	pixels.SetNumUninitialized(captureWidth * captureHeight * 4);

	BITMAPINFOHEADER bih = {0};
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = captureWidth;
	bih.biHeight = -captureHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	GetDIBits(hdcScreen, hBitmap, 0, captureHeight, pixels.GetData(), 
	           (BITMAPINFO*)&bih, DIB_RGB_COLORS);

	SelectObject(hdcMem, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	TArray<FColor> colorData;
	colorData.SetNumUninitialized(captureWidth * captureHeight);
	for (int i = 0; i < captureWidth * captureHeight; ++i)
	{
		colorData[i] = FColor(
			pixels[i * 4 + 2],
			pixels[i * 4 + 1],
			pixels[i * 4],
			pixels[i * 4 + 3]
		);
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create image wrapper"));
		return TEXT("");
	}

	if (!ImageWrapper->SetRaw(colorData.GetData(), colorData.Num() * sizeof(FColor), captureWidth, captureHeight, ERGBFormat::BGRA, 8))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to set raw image data"));
		return TEXT("");
	}

	const TArray64<uint8>& PNGData = ImageWrapper->GetCompressed();

	if (FFileHelper::SaveArrayToFile(PNGData, *FullPath))
	{
		UE_LOG(LogTemp, Log, TEXT("Screenshot saved successfully: %s"), *FullPath);
		return FullPath;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save screenshot: %s"), *FullPath);
		return TEXT("");
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("Screenshot is only supported on Windows"));
	return TEXT("");
#endif
}

FString UBLibraryBP::GetMyPicturesPath()
{
	WCHAR path[MAX_PATH];
	if (SHGetFolderPathW(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
	{
		return FString(path);
	}
	return TEXT("");
}

FString UBLibraryBP::GetMyDocumentsPath()
{
	WCHAR path[MAX_PATH];
	if (SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path) == S_OK)
	{
		return FString(path);
	}
	return TEXT("");
}

TArray<uint8> UBLibraryBP::LoadTextureFromFileAndConvertToBytes(const FString& FilePath)
{
	TArray<uint8> Result;
	
	if (!FPaths::FileExists(FilePath)) {
		UE_LOG(LogTemp, Error, TEXT("File does not exist: %s"), *FilePath);
		return Result;
	}

	if (FFileHelper::LoadFileToArray(Result, *FilePath)) {
		return Result;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to load image from file: %s"), *FilePath);
	return Result;
}

bool UBLibraryBP::GenerateMultipartFormData(const FString& FilePath, const FString& FieldName, FString& OutBoundary, TArray<uint8>& OutRequestBody)
{
	if (!FPaths::FileExists(FilePath)) {
		UE_LOG(LogTemp, Error, TEXT("File does not exist: %s"), *FilePath);
		return false;
	}

	FString Boundary = FString::Printf(TEXT("---------------------------%lld"), FDateTime::Now().GetTicks());
	OutBoundary = Boundary;

	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *FilePath)) {
		UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
		return false;
	}

	FString Filename = FPaths::GetCleanFilename(FilePath);

	FString HeaderPart = FString::Printf(
		TEXT("--%s\r\n" 
			 "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n" 
			 "Content-Type: image/png\r\n" 
			 "\r\n"), 
		*Boundary, *FieldName, *Filename);

	FString FooterPart = FString::Printf(TEXT("\r\n--%s--\r\n"), *Boundary);

	TArray<uint8> HeaderBytes;
	FTCHARToUTF8 HeaderConverter(*HeaderPart);
	if (HeaderConverter.Length() > 0)
	{
		HeaderBytes.Append((uint8*)HeaderConverter.Get(), HeaderConverter.Length());
	}
	
	TArray<uint8> FooterBytes;
	FTCHARToUTF8 FooterConverter(*FooterPart);
	if (FooterConverter.Length() > 0)
	{
		FooterBytes.Append((uint8*)FooterConverter.Get(), FooterConverter.Length());
	}

	OutRequestBody.Append(HeaderBytes);
	OutRequestBody.Append(FileData);
	OutRequestBody.Append(FooterBytes);

	return true;
}

template<typename T>
T UBLibraryBP::GetPreviousItem(const TArray<T>& Array, const T& Current)
{
	if (Array.Num() == 0)
	{
		return T();
	}

	int32 CurrentIndex = Array.Find(Current);
	if (CurrentIndex == INDEX_NONE)
	{
		return Array[0];
	}

	if (CurrentIndex == 0)
	{
		return Array.Last();
	}

	return Array[CurrentIndex - 1];
}

template<typename T>
T UBLibraryBP::GetNextItem(const TArray<T>& Array, const T& Current)
{
	if (Array.Num() == 0)
	{
		return T();
	}

	int32 CurrentIndex = Array.Find(Current);
	if (CurrentIndex == INDEX_NONE)
	{
		return Array[0];
	}

	if (CurrentIndex == Array.Num() - 1)
	{
		return Array[0];
	}

	return Array[CurrentIndex + 1];
}

template<typename T>
T UBLibraryBP::GetFirstItem(const TArray<T>& Array)
{
	if (Array.Num() == 0)
	{
		return T();
	}

	return Array[0];
}

UUserWidget* UBLibraryBP::GetPreviousWidget(const TArray<UUserWidget*>& Array, UUserWidget* Current)
{
	return GetPreviousItem(Array, Current);
}

UUserWidget* UBLibraryBP::GetNextWidget(const TArray<UUserWidget*>& Array, UUserWidget* Current)
{
	return GetNextItem(Array, Current);
}

UUserWidget* UBLibraryBP::GetFirstWidget(const TArray<UUserWidget*>& Array)
{
	return GetFirstItem(Array);
}

UObject* UBLibraryBP::GetPreviousObject(const TArray<UObject*>& Array, UObject* Current)
{
	return GetPreviousItem(Array, Current);
}

UObject* UBLibraryBP::GetNextObject(const TArray<UObject*>& Array, UObject* Current)
{
	return GetNextItem(Array, Current);
}

UObject* UBLibraryBP::GetFirstObject(const TArray<UObject*>& Array)
{
	return GetFirstItem(Array);
}

UTexture2D* UBLibraryBP::GetPreviousTexture(const TArray<UTexture2D*>& Array, UTexture2D* Current)
{
	return GetPreviousItem(Array, Current);
}

UTexture2D* UBLibraryBP::GetNextTexture(const TArray<UTexture2D*>& Array, UTexture2D* Current)
{
	return GetNextItem(Array, Current);
}

UTexture2D* UBLibraryBP::GetFirstTexture(const TArray<UTexture2D*>& Array)
{
	return GetFirstItem(Array);
}
