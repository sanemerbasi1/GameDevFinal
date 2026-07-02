// Fill out your copyright notice in the Description page of Project Settings.
#include "SwordWeapon.h"
#include "NetBaseZombie.h" 
#include "Engine/World.h"

void ASwordWeapon::Attack(AActor* Attacker, FVector ForwardVector, float StrengthStat)
{
    FHitResult HitResult;
    FVector Start = GetActorLocation();
    FVector End = Start + (ForwardVector * 200.0f);
    FCollisionShape Capsule = FCollisionShape::MakeCapsule(20.0f, 60.0f); 
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Attacker);

    bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_WorldDynamic, Capsule, QueryParams);
    {
       if (bHit && HitResult.GetActor())
        {
        ANetBaseZombie* HitEnemy = Cast<ANetBaseZombie>(HitResult.GetActor());
        if (HitEnemy)
            {
            HitEnemy->TakingDamage(20.0f * StrengthStat); 
            }
        }
    }
}