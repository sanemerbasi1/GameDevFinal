#include "BowProjectile.h"
#include "NetBaseZombie.h"

ABowProjectile::ABowProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true); 

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(15.0f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    RootComponent = CollisionComp;

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    
    ProjectileMesh->SetMobility(EComponentMobility::Movable);
    ProjectileMesh->SetupAttachment(CollisionComp);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ProjectileMesh->SetSimulatePhysics(false);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    ProjectileDamage = 15.0f;
}

void ABowProjectile::BeginPlay()
{
    Super::BeginPlay();
    
    if (CollisionComp)
    {
        CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABowProjectile::OnProjectileOverlap);

        if (GetOwner())
        {
            CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
        }
    }
}

void ABowProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                         bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;

    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        ANetBaseZombie* HitEnemy = Cast<ANetBaseZombie>(OtherActor);
        if (HitEnemy)
        {
            HitEnemy->TakingDamage(ProjectileDamage);
            Destroy();
        }
    }
}