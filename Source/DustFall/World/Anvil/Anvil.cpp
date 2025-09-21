// Fill out your copyright notice in the Description page of Project Settings.


#include "Anvil.h"
#include "Components/BoxComponent.h"
#include "DustFall/Core/Interface/GamemodeInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"


AAnvil::AAnvil()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAnvil::BeginPlay()
{
	Super::BeginPlay();

	if (UBoxComponent* Box = FindComponentByClass<UBoxComponent>())
		Box->OnComponentBeginOverlap.AddDynamic(this, &AAnvil::OnTriggerOverlap);
}

void AAnvil::OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasCrushedPlayer) return;

	if (OtherActor && OtherActor != this && Cast<ACharacter>(OtherActor))
	{
		bHasCrushedPlayer = true;

		FTimerHandle CrushDelayHandle;
		GetWorldTimerManager().SetTimer(
			CrushDelayHandle,
			this,
			&AAnvil::NotifyCrushAfterDelay,
			0.04f,
			false
		);
	}
}

void AAnvil::NotifyRemoveAnvilAfterDelay()
{
	Destroy();
}

void AAnvil::NotifyCrushAfterDelay()
{
	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
		IGamemodeInterface::Execute_AnvilOverlapPlayer(GM);

	FTimerHandle RemoveAnvilDelayHandle;
	GetWorldTimerManager().SetTimer(
		RemoveAnvilDelayHandle,
		this,
		&AAnvil::NotifyRemoveAnvilAfterDelay,
		2.f,
		false
	);
}
