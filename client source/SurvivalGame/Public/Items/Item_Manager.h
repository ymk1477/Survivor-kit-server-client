// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KitTargetPoint.h"
#include "CupcakeTargetPoint.h"
#include "SConsumableActor.h"
#include "MyGameInstance.h"
#include "GameFramework/Actor.h"
#include "Item_Manager.generated.h"

UCLASS()
class SURVIVALGAME_API AItem_Manager : public AActor
{
	GENERATED_BODY()

	TArray<AKitTargetPoint*> KitPoints;
	TArray<ACupcakeTargetPoint*> CupcakePoints;
	TArray<ASConsumableActor*> ItemArray;

public:	
	// Sets default values for this actor's properties
	AItem_Manager();

	void FindItemPoints();
	void SpawnItems();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<ASConsumableActor*>* GetItemArray();

};
