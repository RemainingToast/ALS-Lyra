#pragma once

#include "Character/LyraCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsMovementSettings.h"
#include "AlsCharacterMovementComponent.generated.h"

using FAlsPhysicsRotationDelegate = TMulticastDelegate<void(float DeltaTime)>;

class ALS_API FAlsCharacterNetworkMoveData : public FCharacterNetworkMoveData
{
private:
	using Super = FCharacterNetworkMoveData;

public:
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	FGameplayTag Stance{AlsStanceTags::Standing};

	FGameplayTag MaxAllowedGait{AlsGaitTags::Walking};

public:
	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& Movement, FArchive& Archive, UPackageMap* Map, ENetworkMoveType MoveType) override;
};

class ALS_API FAlsCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
public:
	FAlsCharacterNetworkMoveData MoveData[3];

public:
	FAlsCharacterNetworkMoveDataContainer();
};

class ALS_API FAlsSavedMove : public FSavedMove_Character
{
private:
	using Super = FSavedMove_Character;

public:
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	FGameplayTag Stance{AlsStanceTags::Standing};

	FGameplayTag MaxAllowedGait{AlsGaitTags::Walking};

public:
	virtual void Clear() override;

	virtual void SetMoveFor(ACharacter* Character, float NewDeltaTime, const FVector& NewAcceleration,
	                        FNetworkPredictionData_Client_Character& PredictionData) override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;

	virtual void CombineWith(const FSavedMove_Character* PreviousMove, ACharacter* Character,
	                         APlayerController* Player, const FVector& PreviousStartLocation) override;

	virtual void PrepMoveFor(ACharacter* Character) override;
};

class ALS_API FAlsNetworkPredictionData : public FNetworkPredictionData_Client_Character
{
private:
	using Super = FNetworkPredictionData_Client_Character;

public:
	explicit FAlsNetworkPredictionData(const UCharacterMovementComponent& Movement);

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class ALS_API UAlsCharacterMovementComponent : public ULyraCharacterMovementComponent
{
	GENERATED_BODY()

	friend FAlsSavedMove;

protected:
	UAlsCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	
	FAlsCharacterNetworkMoveDataContainer MoveDataContainer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	TObjectPtr<UAlsMovementSettings> MovementSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FAlsMovementGaitSettings GaitSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag RotationMode{AlsRotationModeTags::ViewDirection};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag Stance{AlsStanceTags::Standing};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FGameplayTag MaxAllowedGait{AlsGaitTags::Walking};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bMovementModeLocked;

	// Used to temporarily prohibit the player from moving the character. Also works for AI-controlled characters.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bInputBlocked;

	// Valid only on locally controlled characters.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FRotator PreviousControlRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector PendingPenetrationAdjustment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FVector PrePenetrationAdjustmentVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	bool bPrePenetrationAdjustmentVelocityValid;

public:
	FAlsPhysicsRotationDelegate OnPhysicsRotation;

public:
	UAlsCharacterMovementComponent();

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* Property) const override;
#endif

	virtual void BeginPlay() override;

	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual bool ShouldPerformAirControlForPathFollowing() const override;

	virtual void UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation) override;

	virtual bool ApplyRequestedMove(float DeltaTime, float CurrentMaxAcceleration, float MaxSpeed, float Friction,
	                                float BrakingDeceleration, FVector& RequestedAcceleration, float& RequestedSpeed) override;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	virtual float GetMaxAcceleration() const override;

	virtual float GetMaxBrakingDeceleration() const override;

protected:
	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaTime) override;

	/* @Game-Change Add CitySample Slide Detection - Begin */
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact) override;

	// Minimum angle for movement to slide against. This prevents sliding against very sharp angles that will result in very small movements. Only active if greater than 0.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Surface Slide")
	float MinSurfaceSlideAngle = 0.0f;

	// Minimum angle for movement to slide against when colliding against a character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Surface Slide")
	float MinSurfaceSlideAngleCharacter = 0.0f;
	
private:
	float TimeLastSlideAlongSurfaceBlock = -1000.0f;
	/* @Game-Change Add CitySample Slide Detection - End */

public:
	virtual void PhysicsRotation(float DeltaTime) override;

protected:
	virtual void PhysWalking(float DeltaTime, int32 Iterations) override;

	virtual void PhysNavWalking(float DeltaTime, int32 Iterations) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	virtual FVector ConsumeInputVector() override;

public:
	virtual void ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult,
	                              float SweepRadius, const FHitResult* DownwardSweepResult) const override;

protected:
	virtual void PerformMovement(float DeltaTime) override;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void SmoothClientPosition(float DeltaTime) override;

	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAcceleration) override;

private:
	void SavePenetrationAdjustment(const FHitResult& Hit);

	void ApplyPendingPenetrationAdjustment();

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Character Movement")
	void SetMovementSettings(UAlsMovementSettings* NewMovementSettings);

	const FAlsMovementGaitSettings& GetGaitSettings() const;

private:
	void RefreshGaitSettings();

public:
	void SetRotationMode(const FGameplayTag& NewRotationMode);

	void SetStance(const FGameplayTag& NewStance);

	void SetMaxAllowedGait(const FGameplayTag& NewMaxAllowedGait);

private:
	void RefreshMaxWalkSpeed();

public:
	float CalculateGaitAmount() const;

	void SetMovementModeLocked(bool bNewMovementModeLocked);

	void SetInputBlocked(bool bNewInputBlocked);

	bool TryConsumePrePenetrationAdjustmentVelocity(FVector& OutVelocity);
};

inline const FAlsMovementGaitSettings& UAlsCharacterMovementComponent::GetGaitSettings() const
{
	return GaitSettings;
}
