#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VillageFacility.generated.h"

UCLASS()
class FORESTVILLAGE_API AVillageFacility : public AActor
{
	GENERATED_BODY()
    
public:    
	AVillageFacility();

protected:
	virtual void BeginPlay() override;

public:
	// 시각적 외형을 담당할 스태틱 메시 컴포넌트
	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* FacilityMesh;

	// 🔑 이 시설이 반응할 퀘스트 번호 (0: 울타리, 1: 가마솥, 2: 우물)
	UPROPERTY(EditAnywhere, Category = "Restoration")
	int32 LinkedQuestIndex = 0;

	// 부서진 상태의 모습 (가시덤불, 깨진 솥 등)
	UPROPERTY(EditAnywhere, Category = "Restoration")
	class UStaticMesh* BrokenMesh; 

	// 복원 완료 상태의 모습 (깨끗한 울타리, 불타는 솥 등)
	UPROPERTY(EditAnywhere, Category = "Restoration")
	class UStaticMesh* RestoredMesh; 

	// 복원 시 펑! 하고 터질 마법 파티클 효과 (Niagara)
	UPROPERTY(EditAnywhere, Category = "Restoration|Effects")
	class UNiagaraSystem* SuccessFX;

	// 복원 시 출력될 사운드 효과
	UPROPERTY(EditAnywhere, Category = "Restoration|Effects")
	class USoundBase* SuccessSound;

	// 게임모드에서 퀘스트 완료 시 호출할 복원 트리거 함수
	void TriggerRestoration();

private:
	bool bIsRestored = false;
};