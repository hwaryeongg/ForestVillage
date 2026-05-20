// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

/**
 * 마을 복원 퀘스트 목록과 상세 정보를 표시하는 위젯 클래스
 */
UCLASS()
class FORESTVILLAGE_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// UI 갱신 (탭 전환 시 호출)
	// bShowAvailable: true면 진행 가능, false면 완료된 퀘스트 표시
	void UpdateQuestView(bool bShowAvailable);

	/** 위젯을 닫고 입력 모드를 복구하는 공용 함수 */
	void CloseUI();

protected:
	// 위젯이 생성(초기화)될 때 호출되는 함수
	virtual void NativeConstruct() override;

	// --- meta=(BindWidget) ---
	// 블루프린트 위젯 내의 이름과 반드시 일치해야 합니다.
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_TabAvailable;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_TabCompleted;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* QuestListBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_QuestTitle;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_QuestDescription;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_CompleteQuest;

	// --- 내부 버튼 이벤트 함수 ---
	
	UFUNCTION()
	void OnAvailableTabClicked();

	UFUNCTION()
	void OnCompletedTabClicked();

	UFUNCTION()
	void OnCompleteButtonClicked();
};
