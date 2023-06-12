#include "SunController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DirectionalLightComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Niagara/Public/NiagaraActor.h"


UNiagaraComponent* ASunController::FindFireflyNiagaraComponent()
{
    // Find all actors
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANiagaraActor::StaticClass(), FoundActors);

    // Check for a Niagara component
    for (AActor* FoundActor : FoundActors)
    {
        ANiagaraActor* NiagaraActor = Cast<ANiagaraActor>(FoundActor);
        if (FoundActor->GetName() == TEXT("NS_Firefly"))
        {
            UNiagaraComponent* NiagaraComponent = NiagaraActor->GetNiagaraComponent();
            if (NiagaraComponent)
            {
                return NiagaraComponent;
            }
        }
    }

    // if component was not found
    return nullptr;
}

void ASunController::EnableFog()
{
    UExponentialHeightFogComponent* FogComponent = GetWorld()->GetFirstPlayerController()->FindComponentByClass<UExponentialHeightFogComponent>();
    if (FogComponent)
    {
        FogComponent->SetVisibility(true);
    }
}

void ASunController::DisableFog()
{
    UExponentialHeightFogComponent* FogComponent = GetWorld()->GetFirstPlayerController()->FindComponentByClass<UExponentialHeightFogComponent>();
    if (FogComponent)
    {
        FogComponent->SetVisibility(false);
    }
}

void ASunController::EnableFireflies()
{
    UNiagaraComponent* FireflyComponent = FindFireflyNiagaraComponent();
    if (FireflyComponent)
    {
        FireflyComponent->SetVisibility(true);
        FireflyComponent->ActivateSystem();
    }
}

void ASunController::DisableFireflies()
{
    UNiagaraComponent* FireflyComponent = FindFireflyNiagaraComponent();
    if (FireflyComponent)
    {
        FireflyComponent->DeactivateSystem();
        FireflyComponent->SetVisibility(false);
    }
}


ASunController::ASunController()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASunController::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (World)
    {
        UGameplayStatics::GetPlayerController(World, 0)->InputComponent->BindAction("SunPosition1", IE_Pressed, this, &ASunController::OnSunPosition1);
        UGameplayStatics::GetPlayerController(World, 0)->InputComponent->BindAction("SunPosition2", IE_Pressed, this, &ASunController::OnSunPosition2);
        UGameplayStatics::GetPlayerController(World, 0)->InputComponent->BindAction("SunPosition3", IE_Pressed, this, &ASunController::OnSunPosition3);
    }
}

void ASunController::OnSunPosition1()
{
    UDirectionalLightComponent* DirectionalLight = UGameplayStatics::GetActorOfClass(GetWorld(), UDirectionalLightComponent::StaticClass());
    if (DirectionalLight)
    {
        // morning
        DirectionalLight->SetWorldRotation(FRotator(30.0f, 0.0f, 0.0f));
    }
    
    disableFog();
    disableFireflies();
}

void ASunController::OnSunPosition2()
{
    UDirectionalLightComponent* DirectionalLight = UGameplayStatics::GetActorOfClass(GetWorld(), UDirectionalLightComponent::StaticClass());
    if (DirectionalLight)
    {
        // afternoon
        DirectionalLight->SetWorldRotation(FRotator(60.0f, 0.0f, 0.0f));
    }
    
    enableFog();
    enableFireflies();
}


void ASunController::OnSunPosition3()
{
    UDirectionalLightComponent* DirectionalLight = UGameplayStatics::GetActorOfClass(GetWorld(), UDirectionalLightComponent::StaticClass());
    if (DirectionalLight)
    {
        // night
        DirectionalLight->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
    }
    
    disableFog();
    enableFireflies();
}
