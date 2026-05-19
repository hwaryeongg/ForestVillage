// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ForestVillageGameModeBase.h" // 자원 추가 함수 호출을 위해 필요
#include "PlayerPawn.h" // 플레이어 클래스 확인을 위해 필요

// 생성자: 컴포넌트 생성 및 기본값 설정
AItemActor::AItemActor()
{
	// 아이템은 매 프레임 업데이트(Tick)가 필요 없으므로 꺼둡니다.
	PrimaryActorTick.bCanEverTick = false;

	// 1. 충돌체(SphereComp) 생성 및 루트로 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	
	// --- 수정 포인트 2: 획득 범위 및 감지 신뢰도 향상 ---
	// 범위를 기존 50에서 80~100 정도로 키워서 플레이어가 근처만 가도 먹어지게 합니다.
	SphereComp->SetSphereRadius(100.0f); 
	
	// 충돌 프로파일 설정: "OverlapAllDynamic"으로 설정하면 모든 움직이는 물체와 겹침을 감지합니다.
	// 혹은 "Trigger"를 유지하되 감지 범위를 넓히는 것이 안전합니다.
	SphereComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 2. 외형 메쉬(MeshComp) 생성 및 충돌체에 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	
	// 메쉬가 바닥을 뚫지 않게 하려면 물리 시뮬레이션을 켤 수 있습니다.
	// MeshComp->SetSimulatePhysics(true); // 필요 시 에디터에서 켜주세요.
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 3. 충돌 이벤트(Overlap) 발생 시 호출될 함수 연결
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnOverlapBegin);
}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	// --- 추가 수정: 메쉬가 충돌을 방해하지 않도록 확실히 설정 ---
	if (MeshComp)
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	}

	// 생성되자마자 플레이어 발밑에 있다면 즉시 감지
	SphereComp->UpdateOverlaps();
}

// 다른 액터와 겹쳤을 때 호출되는 함수
void AItemActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. 닿은 대상이 플레이어(APlayerPawn)인지 확인
	if (OtherActor && OtherActor->IsA(APlayerPawn::StaticClass()))
	{
		Collect();
	}
}

void AItemActor::Collect()
{
	// 2. 현재 월드의 게임 모드를 가져와서 자원 관리 클래스로 캐스팅
	AForestVillageGameModeBase* GM = Cast<AForestVillageGameModeBase>(GetWorld()->GetAuthGameMode());
	
	if (GM)
	{
		// 3. 설정된 아이템 타입과 수량을 게임 모드 데이터에 추가
		GM->AddResource(ItemType, Amount);
		
		UE_LOG(LogTemp, Warning, TEXT("[아이템 획득] Type: %d, Amount: %d"), (int32)ItemType, Amount);

		// 4. 습득 완료 후 아이템 액터 파괴 (월드에서 제거)
		Destroy();
	}
}
