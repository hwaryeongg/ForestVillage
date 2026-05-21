#include "VillageFacility.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AVillageFacility::AVillageFacility()
{
	PrimaryActorTick.bCanEverTick = false;

	// 퀘스트 창에서 통합 제어하므로 상호작용 박스는 제거하고 메시를 바로 루트로 씁니다.
	FacilityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FacilityMesh"));
	SetRootComponent(FacilityMesh);
}

void AVillageFacility::BeginPlay()
{
	Super::BeginPlay();
    
	// 게임 시작 시 항상 '부서진 모습'으로 초기화
	if (BrokenMesh)
	{
		FacilityMesh->SetStaticMesh(BrokenMesh);
	}
}

void AVillageFacility::TriggerRestoration()
{
	// 이미 고쳐졌다면 중복 실행 방지
	if (bIsRestored) return;
	bIsRestored = true;

	// 1. 깨끗한 메시(새 건물/오브젝트)로 스왑
	if (RestoredMesh)
	{
		FacilityMesh->SetStaticMesh(RestoredMesh);
	}

	// 2. 마법 같은 복원 파티클 생성 (Niagara)
	if (SuccessFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SuccessFX, GetActorLocation());
	}

	// 3. 복원 효과음 재생
	if (SuccessSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SuccessSound, GetActorLocation());
	}

	UE_LOG(LogTemp, Warning, TEXT("[VillageFacility] %d번 시설이 성공적으로 복원되었습니다!"), LinkedQuestIndex);
}