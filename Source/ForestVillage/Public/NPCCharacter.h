#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCCharacter.generated.h"

UCLASS()
class FORESTVILLAGE_API ANPCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANPCCharacter();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* InteractionArea;

	// --- 퀘스트 위젯 대신 대화 위젯 클래스를 받도록 변경 ---
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UDialogueWidget> DialogueWidgetClass;

	void OnInteract();

private:
	UPROPERTY()
	class UDialogueWidget* CurrentDialogueUI;
};