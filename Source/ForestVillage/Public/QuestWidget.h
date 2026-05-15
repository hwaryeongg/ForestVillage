// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

/**
 * 퀘스트 창 UI를 관리하는 클래스
 */
UCLASS()
class FORESTVILLAGE_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- UI 바인딩 (GEMINI.md 규칙: meta=(BindWidget) 사용) ---
	// 블루프린트 위젯 내의 버튼 이름을 'CloseButton'으로 지어주세요.
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

protected:
	// 위젯이 생성(초기화)될 때 호출되는 함수
	virtual void NativeConstruct() override;

	// 닫기 버튼 클릭 시 호출될 함수
	UFUNCTION()
	void OnCloseClicked();
};
