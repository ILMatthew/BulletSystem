// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "E_MunitionType.h"

#include "BulletType.generated.h"


/**
 * 
 */
UCLASS(BlueprintType)
class BULLETSYSTEM_API UBulletType : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Stats")
		MunitionType MunitionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Stats")
		float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Stats")
		float Density;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Stats")
		float BulletLenght;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Stats")
		float Penetration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Visuals")
		UStaticMesh* BulletMesh;
};
