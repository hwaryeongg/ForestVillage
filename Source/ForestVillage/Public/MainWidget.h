// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 게임의 메인 UI를 담당하는 위젯 클래스
 */
UCLASS()
class FORESTVILLAGE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- UI 바인딩 (GEMINI.md 규칙: meta=(BindWidget) 사용) ---
	// 블루프린트 위젯 내의 텍스트 블록 이름과 정확히 일치해야 합니다.
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AppleText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WoodText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OrangeText;

	// 자원 수치를 UI에 업데이트하는 함수
	void UpdateResourceUI(int32 Apple, int32 Wood, int32 Orange);
};
