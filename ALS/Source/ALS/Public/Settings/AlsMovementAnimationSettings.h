﻿#pragma once

#include "AlsMovementAnimationSettings.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType)
struct FAlsMovementAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float AnimatedWalkSpeed{150};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float AnimatedRunSpeed{350};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float AnimatedSprintSpeed{600};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float AnimatedCrouchSpeed{150};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float VelocityBlendInterpolationSpeed{12};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	float PivotActivationSpeedThreshold{200};

	// Movement speed to stride blend amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* StrideBlendAmountWalkCurve;

	// Movement speed to stride blend amount curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* StrideBlendAmountRunCurve;
};