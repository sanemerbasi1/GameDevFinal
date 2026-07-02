// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ANetGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

UPROPERTY(EditAnywhere, BlueprintReadWrite)
TSubclassOf<class ANetBaseZombie> ZombieClass;

UFUNCTION(BlueprintCallable)
TArray<AActor*> GetSpawnersByTag(FName TagToFind);

UFUNCTION(BlueprintCallable)
ANetBaseZombie* SpawnEnemy(TSubclassOf<class ANetBaseZombie> EnemyClass, const FVector& Location, const FRotator& Rotation);
};
