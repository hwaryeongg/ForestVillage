// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h" // 동적 머티리얼 제어용
#include "PlayerPawn.h" // 플레이어인지 확인하기 위해 필요

AResourceActor::AResourceActor()
{
    // Tick 함수는 필요 없으므로 성능을 위해 니다.
    PrimaryActorTick.bCanEverTick = false;

    // 1. 충돌체(BoxComp) 생성 및 루트 설정
    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    SetRootComponent(BoxComp);
    
    // 오버랩(겹침) 이벤트만 발생하도록 설정 (수업 자료 2일차 응용)
    BoxComp->SetCollisionResponseToAllChannels(ECR_Overlap);
    BoxComp->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f)); // 적당히 큰 크기로 설정

    // 2. 메시(MeshComp) 생성 및 부착
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);

    // 3. 오버랩 이벤트 연결 (델리게이트 바인딩)
    BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AResourceActor::OnOverlapBegin);
    BoxComp->OnComponentEndOverlap.AddDynamic(this, &AResourceActor::OnOverlapEnd);
}

void AResourceActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 4. 머티리얼 제어를 위한 세팅
    // 메시에 적용된 첫 번째(0번) 머티리얼을 동적으로 바꿀 수 있게 복사본을 만듭니다.
    if (MeshComp->GetMaterial(0))
    {
        DynamicMat = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
    }
}

// 플레이어가 E 키를 눌러 상호작용할 때 실행되는 함수
void AResourceActor::Gather(float Power)
{
    Durability -= Power;
    
    // 로그를 띄워 채집이 잘 되는지 확인합니다.
    UE_LOG(LogTemp, Warning, TEXT("채집 진행 중! 남은 내구도: %f"), Durability);

    // 내구도가 0 이하가 되면
    if (Durability <= 0.0f)
    {
        // --- 추가 수정: 스폰 전 '폭발' 방지 ---
        // 나무의 충돌을 미리 꺼서 아이템들이 튕겨나가지 않게 합니다.
        BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        MeshComp->SetVisibility(false); // 시각적으로 숨김

        DropResource(); // 아이템 스폰
        Destroy();      // 자원(나 자신) 파괴
    }
}

void AResourceActor::DropResource()
{
    if (!GetWorld()) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 1. 바위 노드인 경우
    if (bIsRockNode)
    {
        // 돌(Stone) 스폰: 100% 확률로 1~2개
        int32 StoneCount = FMath::RandRange(1, 2);
        for (int32 i = 0; i < StoneCount; i++)
        {
            FVector StoneOffset = FVector(FMath::RandRange(-50.f, 50.f), FMath::RandRange(-50.f, 50.f), 50.f);
            GetWorld()->SpawnActor<AActor>(StoneFactory, GetActorLocation() + StoneOffset, GetActorRotation(), SpawnParams);
        }

        // 보석(Gemstone) 스폰: 50% 확률로 추가 보너스
        if (FMath::RandRange(0.0f, 100.0f) <= 50.0f)
        {
            TSubclassOf<AActor> SelectedGem = nullptr;
            int32 GemType = FMath::RandRange(0, 2);
            
            switch (GemType)
            {
                case 0: SelectedGem = RubyFactory; break;
                case 1: SelectedGem = DiamondFactory; break;
                case 2: SelectedGem = LapisFactory; break;
            }

            if (SelectedGem)
            {
                FVector GemOffset = FVector(FMath::RandRange(-70.f, 70.f), FMath::RandRange(-70.f, 70.f), 80.f);
                GetWorld()->SpawnActor<AActor>(SelectedGem, GetActorLocation() + GemOffset, GetActorRotation(), SpawnParams);
                UE_LOG(LogTemp, Warning, TEXT("희귀 보석 드랍 성공!"));
            }
        }
    }
    // 2. 일반 나무 노드인 경우
    else
    {
        // 과일 아이템 스폰
        if (ItemFactory)
        {
            FVector RandomOffset = FVector(FMath::RandRange(-30.f, 30.f), FMath::RandRange(-30.f, 30.f), 50.f);
            GetWorld()->SpawnActor<AActor>(ItemFactory, GetActorLocation() + RandomOffset, GetActorRotation(), SpawnParams);
        }

        // 나무 아이템 스폰 (Wood)
        if (WoodFactory)
        {
            FVector WoodOffset = FVector(FMath::RandRange(-60.f, 60.f), FMath::RandRange(-60.f, 60.f), 50.f);
            GetWorld()->SpawnActor<AActor>(WoodFactory, GetActorLocation() + WoodOffset, GetActorRotation(), SpawnParams);
        }
    }
}

// 플레이어가 접근했을 때 (빛나게 하기)
void AResourceActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 다가온 액터가 플레이어(APlayerPawn)인지 확인합니다 (Cast 대신 IsA 사용이 가볍고 안전함)
    if (OtherActor && OtherActor->IsA<APlayerPawn>())
    {
        if (DynamicMat)
        {
            // 머티리얼에 있는 "HighlightIntensity" 라는 파라미터의 값을 5.0으로 변경하여 빛을 냅니다.
            DynamicMat->SetScalarParameterValue(TEXT("HighlightIntensity"), 5.0f);
        }
    }
}

// 플레이어가 멀어졌을 때 (빛 끄기)
void AResourceActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA<APlayerPawn>())
    {
        if (DynamicMat)
        {
            // 값을 다시 0으로 돌려서 빛을 끕니다.
            DynamicMat->SetScalarParameterValue(TEXT("HighlightIntensity"), 0.0f);
        }
    }
}