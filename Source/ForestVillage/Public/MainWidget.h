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
	// HUD 다이어트: 상시 자원 표시는 더 이상 하지 않습니다.
	// 필요 시 퀘스트 알림용 텍스트 등을 추가할 수 있습니다.
	
	// 자원 수치를 UI에 업데이트하는 함수
	void UpdateResourceUI(int32 Apple, int32 Wood, int32 Orange, int32 Stone, int32 Diamond, int32 Ruby, int32 Lapis) {}
};
