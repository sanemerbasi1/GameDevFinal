// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameMode.h"
#include "NetBaseZombie.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

TArray<AActor*> ANetGameMode::GetSpawnersByTag(FName TagToFind)
{
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TagToFind, FoundSpawners);
    return FoundSpawners;
}

ANetBaseZombie* ANetGameMode::SpawnEnemy(TSubclassOf<ANetBaseZombie> ZombieClass, const FVector& Location, const FRotator& Rotation)
{
    
    if (!ZombieClass) 
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    return GetWorld()->SpawnActor<ANetBaseZombie>(ZombieClass, Location, Rotation, SpawnParams);
}
