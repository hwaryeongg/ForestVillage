#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

UCLASS()
class FORESTVILLAGE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 인벤토리가 열릴 때 자원 개수를 갱신하는 함수
	void UpdateInventory();

	// 창 닫기 함수
	UFUNCTION()
	void CloseUI();

protected:
	virtual void NativeConstruct() override;

	// --- UI 바인딩 (이름이 블루프린트와 완벽히 같아야 합니다) ---
	
	// 닫기 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Close;

	// 7대 자원 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Apple;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Orange;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Wood;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Stone;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Ruby;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Diamond;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Lapis;
};