// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DataAssets/BulletType.h"

#include "ProjectilesManager.generated.h"

/** Please add a struct description */
USTRUCT(BlueprintType)
struct FBullet
{
	GENERATED_BODY()
public:

	FBullet() {} 

	FBullet(FVector staritngPosition, FVector direction, UBulletType* bulletData)
		: StaritngPosition(staritngPosition), Direction(direction), BulletData(bulletData){ }
	

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Staritng Position", MakeStructureDefaultValue = "0.000000,0.000000,0.000000"))
		FVector StaritngPosition;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Direction", MakeStructureDefaultValue = "0.000000,0.000000,0.000000"))
		FVector Direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Bullet Data"))
		UBulletType* BulletData;
};

class UNiagaraComponent;

UCLASS(Abstract)
class BULLETSYSTEM_API AProjectilesManager : public AActor
{
	GENERATED_BODY()
	
private:

	inline static AProjectilesManager* SceneManager;

	void UpdateBullets();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Simulation")
		TArray<FBullet> BulletList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		bool AsyncSim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Simulation|Ricochet", meta = (ClampMin = 0.0f, ClampMax = 90.0f))
		float MaxRicochetAngle;

	// The percentage of energy the bullet should mantian when ricochet. X if ricochet angle is max, Y if is min.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Simulation|Ricochet", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
		FVector2f MinMaxEnergyReduction;
	
	// Check if the bullet should ricochet and set new bullet direction (outDirection) if true
	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
		bool ShouldRicochet(const FHitResult& hitData, UPARAM(ref) FVector& outDirection);

	// Check if the bullet should penetrate the surface and set new bullet starting point and direction (outBulllet) if true
	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
		bool ShouldPenetrate(const FHitResult& hitData, UPARAM(ref) FBullet& outBullet);

	// Calculate and add the drag and/or wind on to the bullet direction (outBulllet)
	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
		void ApplyDrag(UPARAM(ref) FBullet& outBullet);

	// Calculate and add the gravity on to the bullet direction (outBulllet)
	UFUNCTION(BlueprintNativeEvent, Category = "Simulation")
		void ApplyGravity(UPARAM(ref) FBullet& outBullet);

	// Get the atmospheric pressure at given location for simulation purposes
	// Must be overriden in blueprint based on how atmospheric conditions are implemented
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Simulation")
		float GetPressure(const FVector& location);

	// Get air humidity at given location for simulation purposes
	// Must be overriden in blueprint based on how atmospheric conditions are implemented
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Simulation")
		float GetHumidity(const FVector& location);

	// Get wind speed at given location for simulation purposes
	// Must be overriden in blueprint based on how atmospheric conditions are implemented
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Simulation")
		FVector GetWind(const FVector& location);

	UFUNCTION()
		void ClearRef(AActor* actor, EEndPlayReason::Type boh);

public:	
	// Sets default values for this actor's properties
	AProjectilesManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "VFX")
		void ApplyVFX(bool shouldSpawnDecal, const FHitResult& hitData);

	UFUNCTION(BlueprintCallable)
		static void CreateBullet(UPARAM(ref) AActor*& caller, UPARAM(ref) FBullet& newBullet);

};
