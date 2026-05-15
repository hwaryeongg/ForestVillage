// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h" // 동적 머티리얼 제어용
#include "PlayerPawn.h" // 플레이어인지 확인하기 위해 필요

AResourceActor::AResourceActor()
{
    // Tick 함수는 필요 없으므로 성능을 위해 끕니다.
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
        DropResource(); // 아이템 스폰
        Destroy();      // 자원(나 자신) 파괴
    }
}

// 아이템 스폰 함수
void AResourceActor::DropResource()
{
    // 스폰할 아이템이 에디터에서 지정되어 있는지 널 체크(Null Check)
    if (GetWorld() && ItemFactory)
    {
        // 내 위치, 내 회전값 그대로 아이템을 생성합니다 (수업 자료 2일차 SpawnActor 응용)
        GetWorld()->SpawnActor<AActor>(ItemFactory, GetActorLocation(), GetActorRotation());
        UE_LOG(LogTemp, Warning, TEXT("자원 아이템 드랍 성공!"));
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