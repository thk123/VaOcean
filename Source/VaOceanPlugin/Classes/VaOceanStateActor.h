// Copyright 2014 Vladimir Alyamkin. All Rights Reserved.

#pragma once
#include "GameFramework/Actor.h"
#include "VaOceanTypes.h"
#include "VaOceanStateActor.generated.h"

/**
 * Calculates wave height based on SK_Ocean shader approach
 */
UCLASS(ClassGroup = VaOcean, Blueprintable, BlueprintType)
class VAOCEANPLUGIN_API AVaOceanStateActor : public AActor
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITORONLY_DATA
	/** A UBillboardComponent to hold Icon sprite */
	UPROPERTY()
	TSubobjectPtr<UBillboardComponent> SpriteComponent;

	/** Icon sprite */
	UTexture2D* SpriteTexture;
#endif // WITH_EDITORONLY_DATA

	/** Ocean simulation component (should be set in blueprint!) */
	class UVaOceanSimulatorComponent* OceanSimulator;


	//////////////////////////////////////////////////////////////////////////
	// Config accessors

	/** Set global zero ocean level (ocean mesh Z location) */
	UFUNCTION(BlueprintCallable, Category = "World|VaOcean")
	const FSpectrumData& GetSpectrumConfig() const;


	//////////////////////////////////////////////////////////////////////////
	// Ocean state API

	/** Set global zero ocean level (ocean mesh Z location) */
	UFUNCTION(BlueprintCallable, Category = "World|VaOcean")
	virtual void SetGlobalOceanLevel(float OceanLevel);

	/** Global zero ocean level (ocean mesh Z location) */
	UFUNCTION(BlueprintCallable, Category = "World|VaOcean")
	float GetGlobalOceanLevel() const;

	float GetOceanPatchSize();

	/** Get ocean level at desired position */
	virtual float GetOceanLevelAtLocation(const FVector& Location) const;

	virtual FVector GetCurrentAtLocation(const FVector& Location) const;

	/** Get ocean surface normal at desired location */
	virtual FLinearColor GetOceanSurfaceNormal(FVector& Location) const;

	/** Wave velocity is determined by UV change rate */
	virtual FVector GetOceanWaveVelocity(FVector& Location) const;

	/** How much waves are defined by normal map */
	virtual int32 GetOceanWavesNum() const;

	// Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;
	// End AActor interface

protected:
	/** Ocean spectrum data */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FSpectrumData SpectrumConfig;

protected:

	UPROPERTY(EditAnywhere, Category = OceanSetup)
	float GlobalOceanLevel;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bIsDrawingDebugSpheres;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bIsDrawingDebugCurrentLines;

	UPROPERTY(EditAnywhere, Category = Debug)
	int32 HalfNumberOfDebugSpheres;

	//UPROPERTY(EditAnywhere, Category = OceanSetup)
	//float WorldPositionDivider;

};
