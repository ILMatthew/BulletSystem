// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StaticAtmosphereConditions.generated.h"

/**
 * 
 */
UCLASS()
class BULLETSYSTEM_API UStaticAtmosphereConditions : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Conditions")
		float Pressure;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Conditions")
		float Humidity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Atmosphere Conditions")
		FVector Wind;
	
};
