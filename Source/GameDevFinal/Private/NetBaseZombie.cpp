// Fill out your copyright notice in the Description page of Project Settings.
#include "NetBaseZombie.h"
#include "NetBaseCharacter.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "NetBaseAvatar.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/WidgetComponent.h"

ANetBaseZombie::ANetBaseZombie() 
{
    PrimaryActorTick.bCanEverTick = true;

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(RootComponent);

    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen); 
    HealthBarWidget->SetDrawSize(FVector2D(200.0f, 50.0f));

    HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
}


void ANetBaseZombie::TakingDamage(float Damage)
{
    if (!HasAuthority()) 
    {
        return;
    }
    CurrentHealth -= Damage;
    if (CurrentHealth <= 0)
    {
        OnDeath();
    }
    OnHealthChanged(CurrentHealth);
}

void ANetBaseZombie::UpdateBodyParts()
{


}

void ANetBaseZombie::FollowPlayer(ANetBaseCharacter* Player)
{
    if (!HasAuthority() || !Player) return;

    AAIController* AIController = Cast<AAIController>(GetController());

    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

    if (AIController)
    {
        if (Distance <= 200.0f)
    {
        AIController->StopMovement();
        Attack();
    }
    else if (Distance > 200.0f)
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(Player);
        MoveRequest.SetAcceptanceRadius(75.0f);
        AIController->MoveTo(MoveRequest);
    }
    }
}  

void ANetBaseZombie::Attack()
{
    FHitResult HitResult;
    FVector Start = GetActorLocation() + GetActorForwardVector() * 20.0f;
    FVector End = Start + (GetActorForwardVector() * 100.0f);
    FCollisionShape Capsule = FCollisionShape::MakeCapsule(20.0f, 60.0f); 

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult, Start, End, FQuat::Identity, 
        ECC_WorldDynamic, Capsule, QueryParams
    );

if (bHit && HitResult.GetActor())
{
    ANetBaseAvatar* HitEnemy = Cast<ANetBaseAvatar>(HitResult.GetActor());
    if (HitEnemy)
    {
        HitEnemy->TakingDamage(AttackDamage); 
    }
}
}

