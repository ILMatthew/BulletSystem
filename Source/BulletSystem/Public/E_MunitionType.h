// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class MunitionType : uint8
{
	GaugeShot12 = 0 UMETA(DisplayName = "12 Gauge Shot"),
	GaugeSlugs12 = 1 UMETA(DisplayName = "12 Gauge Sluugs"),
	Gauge20 = 2 UMETA(DisplayName = "20 Gauge"),
	mm9x18 = 3 UMETA(DisplayName = "9x18mm"),
	mm762x25 = 4 UMETA(DisplayName = "7.62x25mm"),
	mm9x19 = 5 UMETA(DisplayName = "9x19mm"),
	mm545x39 = 6 UMETA(DisplayName = "5.45x39mm"),
	mm556x45 = 7 UMETA(DisplayName = "5.56x45mm"),
	mm762x39 = 8 UMETA(DisplayName = "7.62x39mm"),
	mm762x51 = 9 UMETA(DisplayName = "7.62x51mm"),
	R762x54 = 10 UMETA(DisplayName = "7.62x54R"),
	mm57x28 = 11 UMETA(DisplayName = "5.7x28mm"),
	mm9x39 = 12 UMETA(DisplayName = "9x39mm"),
	Zero366 = 13 UMETA(DislpayName = "0.366"),
	mm9x21 = 14 UMETA(DisplayName = "9x21mm"),
	mm127x55 = 15 UMETA(DisplayName = "12.7x55mm"),
};