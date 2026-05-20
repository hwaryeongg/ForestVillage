#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

// 화자와 대사 내용을 하나로 묶어주는 구조체 정의
USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SpeakerName; // "모모" 또는 "라비"

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Text; // 실제 대사 내용
};

UCLASS()
class FORESTVILLAGE_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부(NPC)에서 대사 리스트를 넘겨주며 대화를 시작하는 함수
	void StartDialogue(const TArray<FDialogueLine>& Lines);

protected:
	virtual void NativeConstruct() override;

	// --- 🔑 핵심 추가: 키보드 'E' 입력을 가로채기 위한 가상 함수 오버라이드 ---
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// --- UI 바인딩 ---
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_SpeakerName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Dialogue;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Portrait;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Next; // 마우스 클릭용 버튼도 유지

	// --- 에디터에서 설정할 초상화 에셋 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portraits")
	class UTexture2D* MomoPortrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portraits")
	class UTexture2D* RaviPortrait;

	// 다음 대사로 넘어가는 공용 함수
	UFUNCTION()
	void OnNextClicked();

private:
	TArray<FDialogueLine> CurrentDialogueLines;
	int32 CurrentLineIndex = 0;

	// 현재 인덱스의 대사와 초상화를 화면에 세팅하는 함수
	void DisplayCurrentLine();
};