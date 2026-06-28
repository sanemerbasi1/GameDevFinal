#include "NetGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UNetGameInstance::Host(FString MapName)
{
    GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, TEXT("Hosting Game..."));
    UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName), true, "?listen");
}

void UNetGameInstance::Join(FString Address)
{
    GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Green, 
    FString::Printf(TEXT("Joining Game at %s..."),*Address));
    GWorld->GetFirstPlayerController()->ClientTravel(Address,
    ETravelType::TRAVEL_Absolute);
}