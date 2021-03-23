// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "Item_Manager.h"

// Sets default values
AItem_Manager::AItem_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AItem_Manager::FindItemPoints()
{
	UWorld* CurrentWorld = GetWorld();
	for (TActorIterator<AKitTargetPoint> It(CurrentWorld); It; ++It)
	{
		AKitTargetPoint* NewPoint = *It;
		KitPoints.Emplace(NewPoint);
	}

	for (TActorIterator<ACupcakeTargetPoint> It(CurrentWorld); It; ++It)
	{
		ACupcakeTargetPoint* NewPoint = *It;
		CupcakePoints.Emplace(NewPoint);
	}
}

void AItem_Manager::SpawnItems()
{
	UWorld* CurrentWorld = GetWorld();
	UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Class'/Game/Items/Kit.Kit_C'")));
	UClass* GenerateBp = Cast<UClass>(SpawnActor);

	FActorSpawnParameters Spawnparams;
	Spawnparams.Owner = this;
	Spawnparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FRotator NewRotator;
	NewRotator.Roll = -90.0f;
	NewRotator.Pitch = 0.0f;
	NewRotator.Yaw = 0.0f;

	int ItemNum = 0;
	for (auto i = KitPoints.begin(); i != KitPoints.end(); ++i)
	{
		//NewRotator.Yaw = FMath::RandRange(0.0f, 360.0f);
		ASConsumableActor* NewItem = CurrentWorld->SpawnActor<ASConsumableActor>(GenerateBp, (*i)->GetActorLocation(), NewRotator, Spawnparams);

		ItemArray.Emplace(NewItem);
		NewItem->ItemIndex = ItemNum;
		Item_info.IsUsed[ItemNum] = false;
		ItemNum++;
	}

	UObject* SpawnActor2 = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Class'/Game/Items/Cupcake.Cupcake_C'")));
	UClass* GenerateBp2 = Cast<UClass>(SpawnActor2);

	for (auto i = CupcakePoints.begin(); i != CupcakePoints.end(); ++i)
	{
		//NewRotator.Yaw = FMath::RandRange(0.0f, 360.0f);
		ASConsumableActor* NewItem = CurrentWorld->SpawnActor<ASConsumableActor>(GenerateBp2, (*i)->GetActorLocation(), FRotator::ZeroRotator, Spawnparams);

		ItemArray.Emplace(NewItem);
		NewItem->ItemIndex = ItemNum;
		Item_info.IsUsed[ItemNum] = false;
		ItemNum++;
	}

}

// Called when the game starts or when spawned
void AItem_Manager::BeginPlay()
{
	Super::BeginPlay();

	FindItemPoints();
	SpawnItems();

}

// Called every frame
void AItem_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (ItemArray.Num() > 0)
	{
		for (int32 i = 0; i < ItemArray.Num(); ++i)
		{
			if (Item_info.IsUsed[i] && (ItemArray[i] != nullptr))
			{
				ItemArray[i] = nullptr;
			}
		}
	}*/
}



TArray<ASConsumableActor*>* AItem_Manager::GetItemArray()
{
	return &ItemArray;
}