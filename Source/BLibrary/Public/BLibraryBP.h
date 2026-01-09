// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "BLibraryBP.generated.h"

UCLASS()
class BLIBRARY_API UBLibraryBP : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "BLibrary|Screenshot", meta = (DisplayName = "Take Screenshot", Tooltip = "截取屏幕截图并保存为PNG文件", Keywords = "screenshot capture screen"))
	static FString TakeScreenshot(const FString& ScreenshotName, const FString& SaveFolder, float TopPadding = 0.0f, float BottomPadding = 0.0f, float LeftPadding = 0.0f, float RightPadding = 0.0f);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Path", meta = (DisplayName = "Get My Pictures Path", Tooltip = "获取Windows图片文件夹路径", Keywords = "path pictures folder"))
	static FString GetMyPicturesPath();

	UFUNCTION(BlueprintPure, Category = "BLibrary|Path", meta = (DisplayName = "Get My Documents Path", Tooltip = "获取Windows文档文件夹路径", Keywords = "path documents folder"))
	static FString GetMyDocumentsPath();

	UFUNCTION(BlueprintCallable, Category = "BLibrary|File", meta = (DisplayName = "Load Texture From File", Tooltip = "从文件加载纹理并转换为字节数组", Keywords = "load texture file bytes"))
	static TArray<uint8> LoadTextureFromFileAndConvertToBytes(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "BLibrary|File", meta = (DisplayName = "Generate Multipart Form Data", Tooltip = "生成用于HTTP上传的多部分表单数据", Keywords = "multipart form upload http"))
	static bool GenerateMultipartFormData(const FString& FilePath, const FString& FieldName, FString& OutBoundary, TArray<uint8>& OutRequestBody);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get Previous Widget", Tooltip = "获取数组中的上一个Widget", Keywords = "array previous widget"))
	static UUserWidget* GetPreviousWidget(const TArray<UUserWidget*>& Array, UUserWidget* Current);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get Next Widget", Tooltip = "获取数组中的下一个Widget", Keywords = "array next widget"))
	static UUserWidget* GetNextWidget(const TArray<UUserWidget*>& Array, UUserWidget* Current);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get First Widget", Tooltip = "获取数组中的第一个Widget", Keywords = "array first widget"))
	static UUserWidget* GetFirstWidget(const TArray<UUserWidget*>& Array);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get Previous Object", Tooltip = "获取数组中的上一个Object", Keywords = "array previous object"))
	static UObject* GetPreviousObject(const TArray<UObject*>& Array, UObject* Current);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get Next Object", Tooltip = "获取数组中的下一个Object", Keywords = "array next object"))
	static UObject* GetNextObject(const TArray<UObject*>& Array, UObject* Current);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get First Object", Tooltip = "获取数组中的第一个Object", Keywords = "array first object"))
	static UObject* GetFirstObject(const TArray<UObject*>& Array);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get Previous Texture", Tooltip = "获取数组中的上一个Texture", Keywords = "array previous texture"))
	static UTexture2D* GetPreviousTexture(const TArray<UTexture2D*>& Array, UTexture2D* Current);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get Next Texture", Tooltip = "获取数组中的下一个Texture", Keywords = "array next texture"))
	static UTexture2D* GetNextTexture(const TArray<UTexture2D*>& Array, UTexture2D* Current);

	UFUNCTION(BlueprintPure, Category = "BLibrary|Array", meta = (DisplayName = "Get First Texture", Tooltip = "获取数组中的第一个Texture", Keywords = "array first texture"))
	static UTexture2D* GetFirstTexture(const TArray<UTexture2D*>& Array);

private:

	template<typename T>
	static T GetPreviousItem(const TArray<T>& Array, const T& Current);

	template<typename T>
	static T GetNextItem(const TArray<T>& Array, const T& Current);

	template<typename T>
	static T GetFirstItem(const TArray<T>& Array);
};
