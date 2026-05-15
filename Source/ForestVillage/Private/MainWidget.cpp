// Fill out your copyright notice in the Description page of Project Settings.

#include "MainWidget.h"
#include "Components/TextBlock.h" // UTextBlock 기능을 위해 필요

void UMainWidget::UpdateResourceUI(int32 Apple, int32 Wood, int32 Orange)
{
	// 텍스트 블록이 유효한지 확인 후 숫자 세팅
	if (AppleText)
	{
		AppleText->SetText(FText::AsNumber(Apple));
	}

	if (WoodText)
	{
		WoodText->SetText(FText::AsNumber(Wood));
	}

	if (OrangeText)
	{
		OrangeText->SetText(FText::AsNumber(Orange));
	}
}
