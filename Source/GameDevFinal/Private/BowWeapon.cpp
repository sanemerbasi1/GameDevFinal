#include "BowWeapon.h"
#include "BowProjectile.h" 
#include "Engine/World.h"

void ABowWeapon::Attack(AActor* Attacker, FVector ForwardVector, float StrengthStat)
{
    if (!GetWorld() || !ProjectileBP || !Attacker) return;

    FRotator SpawnRotation = ForwardVector.Rotation();
    FVector SpawnLocation = Attacker->GetActorLocation() + (ForwardVector * 100.0f) + FVector(0.0f, 0.0f, 50.0f);

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Attacker;
    SpawnParams.Instigator = Cast<APawn>(Attacker);
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileBP, SpawnLocation, SpawnRotation, SpawnParams);
    
    if (SpawnedProjectile)
    {
        ABowProjectile* Arrow = Cast<ABowProjectile>(SpawnedProjectile);
        if (Arrow)
        {
            Arrow->ProjectileDamage = 15.0f * StrengthStat;
        }

        SpawnedProjectile->SetLifeSpan(5.0f); 
    }
}