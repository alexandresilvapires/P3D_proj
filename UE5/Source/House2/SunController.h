// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SunController.generated.h"

UCLASS()
class HOUSE2_API ASunController : public AActor
{
    GENERATED_BODY()

public:
    ASunController();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnSunPosition1();

    UFUNCTION()
    void OnSunPosition2();

    UFUNCTION()
    void OnSunPosition3();
};


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
