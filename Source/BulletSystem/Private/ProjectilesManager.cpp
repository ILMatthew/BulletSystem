// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectilesManager.h"

#include "Kismet/GameplayStatics.h"
#include "Interfaces/BulletDamageable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Async/Async.h"

// Sets default values
AProjectilesManager::AProjectilesManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AProjectilesManager::UpdateBullets()
{
	FCollisionQueryParams queryParams;
	queryParams.bDebugQuery = true;
	queryParams.bReturnPhysicalMaterial = true;
	
	if (AsyncSim)
	{
		int* indicesToDestroy = new int[BulletList.Num()];

		ParallelFor(BulletList.Num(), [&](int32 i)
		{
			FHitResult hit;
			FVector traceStart = BulletList[i].StaritngPosition;
			FVector traceEnd = traceStart + (BulletList[i].Direction * FApp::GetDeltaTime());

			if (GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECollisionChannel::ECC_Visibility, queryParams))
			{
				// Check if the hit object should execute custom logic and/or block the bullet
				if (UKismetSystemLibrary::DoesImplementInterface(hit.GetActor(), UBulletDamageable::StaticClass()))
				{
					IBulletDamageable::Execute_ReceiveBullet(hit.GetActor(), BulletList[i], hit.BoneName);
					if (IBulletDamageable::Execute_ShouldBlockBullet(hit.GetActor()))
					{
						//BulletList.RemoveAt(i);
						indicesToDestroy[i] = 1;
						BulletList[i].Direction = FVector::Zero();
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Simulated bullets: %i"), BulletList.Num()));

						return;
					}
				}

				// Add force to simulate bullet hit
				if (hit.Component->IsSimulatingPhysics())
					hit.GetComponent()->AddImpulseAtLocation(BulletList[i].Direction * BulletList[i].BulletData->Weight, hit.Location); // / hit.GetComponent()->CalculateMass(), hit.Location);

				// Physic passes
				if (ShouldRicochet(hit, BulletList[i].Direction))
				{
					traceEnd = hit.Location + hit.Normal;
					ApplyVFX(false, hit);
				}
				else if (ShouldPenetrate(hit, BulletList[i]))
				{
					ApplyVFX(true, hit);
				}
				else
				{
					ApplyVFX(true, hit);
					//BulletList.RemoveAt(i);
					indicesToDestroy[i] = 1;
					BulletList[i].Direction = FVector::Zero();
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Simulated bullets: %i"), BulletList.Num()));

					return;
				}
			}

			//DrawDebugLine(GetWorld(), traceStart, traceEnd, hit.bBlockingHit ? FColor::Blue : FColor::Red, false, hit.bBlockingHit ? 5.0f : 1.5f, 0, 2.0f);

			// Update the bullet in the array
			BulletList[i].StaritngPosition = traceEnd;
			ApplyDrag(BulletList[i]);
			ApplyGravity(BulletList[i]);

		});

		for (int i = BulletList.Num() - 1; i >= 0; i--)
		{
			if(indicesToDestroy[i] == 1)
				BulletList.RemoveAt(i);
		}

		delete[] indicesToDestroy;

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Simulated bullets: %i"), BulletList.Num()));
	}
	else
	{
		for (int i = 0; i < BulletList.Num(); i++)
		{
			FHitResult hit;
			FVector traceStart = BulletList[i].StaritngPosition;
			FVector traceEnd = traceStart + (BulletList[i].Direction * FApp::GetDeltaTime()); 

			if (GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECollisionChannel::ECC_Visibility, queryParams))
			{
				// Check if the hit object should execute custom logic and/or block the bullet
				if (UKismetSystemLibrary::DoesImplementInterface(hit.GetActor(), UBulletDamageable::StaticClass()))
				{
					IBulletDamageable::Execute_ReceiveBullet(hit.GetActor(), BulletList[i], hit.BoneName);
					if (IBulletDamageable::Execute_ShouldBlockBullet(hit.GetActor()))
					{
						BulletList.RemoveAt(i);
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Simulated bullets: %i"), BulletList.Num()));

						continue;
					}
				}

				// Add force to simulate bullet hit
				if(hit.Component->IsSimulatingPhysics())
					hit.GetComponent()->AddImpulseAtLocation(BulletList[i].Direction * BulletList[i].BulletData->Weight, hit.Location); // / hit.GetComponent()->CalculateMass(), hit.Location);
			
				// Physic passes
				if (ShouldRicochet(hit, BulletList[i].Direction))
				{
					traceEnd = hit.Location + hit.Normal;
					ApplyVFX(false, hit);
				}
				else if (ShouldPenetrate(hit, BulletList[i]))
				{
					ApplyVFX(true, hit);
				}
				else
				{
					ApplyVFX(true, hit);
					BulletList.RemoveAt(i);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Simulated bullets: %i"), BulletList.Num()));

					continue;
				}
			}
		
			//DrawDebugLine(GetWorld(), traceStart, traceEnd, hit.bBlockingHit ? FColor::Blue : FColor::Red, false, hit.bBlockingHit ? 5.0f : 1.5f, 0, 2.0f);

			// Update the bullet in the array
			BulletList[i].StaritngPosition = traceEnd;
			ApplyDrag(BulletList[i]);
			ApplyGravity(BulletList[i]);

			//BulletList[i].Direction = BulletList[i].Direction - drag + (FVector(0.0f, 0.0f, -980.0f) * FApp::GetDeltaTime());
		}
	}
}

// Called when the game starts or when spawned
void AProjectilesManager::BeginPlay()
{
	Super::BeginPlay();
	
	OnEndPlay.AddDynamic(this, &AProjectilesManager::ClearRef);
}

bool AProjectilesManager::ShouldRicochet_Implementation(const FHitResult& hitData, UPARAM(ref) FVector& outDirection)
{
	FVector newDirection = outDirection;
	newDirection.Normalize();

	float angleAlpha = FVector::DotProduct(hitData.Normal, newDirection);
	float maxAlpha = MaxRicochetAngle / -90.0f;
	if (angleAlpha <= 0.0f && angleAlpha > maxAlpha)
	{
		float speedMultiplier = (angleAlpha / maxAlpha * -1.0f + 1.0f) * (MinMaxEnergyReduction.Y - MinMaxEnergyReduction.X) + MinMaxEnergyReduction.X;

		// Bullet speed reduction should be based on material and angle
		outDirection = FMath::GetReflectionVector(outDirection, hitData.Normal) * 0.85f * speedMultiplier;

		return true;
	}

	return false;
}

bool AProjectilesManager::ShouldPenetrate_Implementation(const FHitResult& hitData, UPARAM(ref) FBullet& outBullet)
{
	TArray<FHitResult> penetrationData;
	FVector traceEnd = hitData.Location + hitData.Normal * -1.0f;
	FVector traceStart = traceEnd + outBullet.Direction / outBullet.Direction.Length() * hitData.Component->Bounds.SphereRadius;

	//DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Yellow, false, 5.0f, 0, 2.0f);

	FCollisionObjectQueryParams objectParams;
	objectParams.AddObjectTypesToQuery(hitData.GetComponent()->GetCollisionObjectType());

	FCollisionQueryParams queryParams;
	queryParams.bReturnPhysicalMaterial = true;

	if(GetWorld()->LineTraceMultiByObjectType(penetrationData, traceStart, traceEnd, objectParams, queryParams))
	//if (GetWorld()->LineTraceMultiByChannel(penetrationData, traceStart, traceEnd, ECollisionChannel::ECC_Visibility))
	{
		for (int i = 0; i < penetrationData.Num(); i++)
		{
			if (penetrationData[i].Component == hitData.Component)
			{
				float distanceToTravel = (penetrationData[i].Location - traceEnd).Length();

				// Bullet speed reduction to replace base on material and bullet stats
				// penetration depth L2 = L×D1/D2, where D1 is the density of the bullet and D2 is the density of the wall and L is the length of the bullet.
				//http://panoptesv.com/RPGs/Equipment/Weapons/Projectile_physics.php
				
				FVector speedReduction = -outBullet.Direction;
				speedReduction.Normalize();
				speedReduction *= FMath::Min(5000 * (hitData.PhysMaterial->Density / outBullet.BulletData->Penetration) * distanceToTravel / 100, outBullet.Direction.Length());
				auto t = hitData.PhysMaterial->SurfaceType;
				outBullet.Direction += speedReduction;
				if (outBullet.Direction.Length() < 100.0f)
					return false;

				outBullet.StaritngPosition = penetrationData[i].Location + penetrationData[i].Normal;
				ApplyVFX(true, penetrationData[i]);
				return true;
			}
		}
	}

	return false;
}

// Called every frame
void AProjectilesManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateBullets();
}

void AProjectilesManager::ApplyDrag_Implementation(UPARAM(ref)FBullet& outBullet)
{
	FVector drag = outBullet.Direction;
	drag.Normalize();
	drag *= 500 * FApp::GetDeltaTime();

	outBullet.Direction -= drag;
}

void AProjectilesManager::ApplyGravity_Implementation(UPARAM(ref)FBullet& outBullet)
{
	outBullet.Direction += FVector(0.0f, 0.0f, -980.0f) * FApp::GetDeltaTime();
}

float AProjectilesManager::GetPressure_Implementation(const FVector& location)
{
	return 1.0f;
}

float AProjectilesManager::GetHumidity_Implementation(const FVector& location)
{
	return 1.0f;
}

FVector AProjectilesManager::GetWind_Implementation(const FVector& location)
{
	return FVector();
}

void AProjectilesManager::ClearRef(AActor* actor, EEndPlayReason::Type boh)
{
	SceneManager = nullptr;
}

void AProjectilesManager::CreateBullet(UPARAM(ref) AActor*& caller,UPARAM(ref) FBullet& newBullet)
{
	/*AActor* maybeManager = UGameplayStatics::GetActorOfClass(caller->GetWorld(), AProjectilesManager::StaticClass());
	AProjectilesManager* manager;

	if (maybeManager)
	{
		manager = Cast<AProjectilesManager>(maybeManager);
	}
	else
	{
		FActorSpawnParameters Params;
		manager = caller->GetWorld()->SpawnActor<AProjectilesManager>(Params);
	}*/

	if (!SceneManager)
	{
		//FActorSpawnParameters Params;
		//SceneManager = caller->GetWorld()->SpawnActor<AProjectilesManager>(Params);

		AActor* maybeManager = UGameplayStatics::GetActorOfClass(caller->GetWorld(), AProjectilesManager::StaticClass());

		if (maybeManager)
		{
			SceneManager = Cast<AProjectilesManager>(maybeManager);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No Bullet Manager present in the scene"));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("No Bullet Manager present in the scene")));
			return;
		}
	}

	SceneManager->BulletList.Add(newBullet);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Simulated bullets: %i"), SceneManager->BulletList.Num()));
}

void AProjectilesManager::ApplyVFX_Implementation(bool shouldSpawnDecal, const FHitResult& hitData) { }

//AProjectilesManager* AProjectilesManager::SceneManager = nullptr;