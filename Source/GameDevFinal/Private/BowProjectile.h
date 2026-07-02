#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BowProjectile.generated.h"

UCLASS()
class GAMEDEVFINAL_API ABowProjectile : public AActor
{
    GENERATED_BODY()
    
public: 
    ABowProjectile();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ProjectileDamage;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                             bool bFromSweep, const FHitResult& SweepResult);
};