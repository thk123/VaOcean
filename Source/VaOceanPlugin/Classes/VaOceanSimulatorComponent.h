// Copyright 2014 Vladimir Alyamkin. All Rights Reserved.

#pragma once

#include "VaOceanPluginPrivatePCH.h"
#include "VaOceanSimulatorComponent.generated.h"

#define PAD16(n) (((n)+15)/16*16)

/**
 * Renders normals and heightmap from Phillips spectrum
 */
UCLASS(ClassGroup=Environment, editinlinenew, meta=(BlueprintSpawnableComponent))
class UVaOceanSimulatorComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	/** Reference to actor that keeps spectrum configuration */
	UPROPERTY(BlueprintReadOnly, Category = OceanState)
	class AVaOceanStateActor *StateActor;

	/** Render target for normal map that can be used by the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OceanSpectrum)
	class UTextureRenderTarget2D* DisplacementTarget;

	/** Render target for normal map that can be used by the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OceanSpectrum)
	class UTextureRenderTarget2D* ResultantTexture;

	/** Render target for height map that can be used by the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OceanSpectrum)
	class UTextureRenderTarget2D* GradientTarget;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bIsUpdatingDisplacementMap;

	UPROPERTY(EditDefaultsOnly, Category = OceanSetup)
	float PatchSize;

	UPROPERTY(EditDefaultsOnly, Category = OceanSetup)
	float DisplacementMapTimeScale;

	// Temporary hack: need to work out how we are doing currents, combining both
	// level layout currents + wave currents + performance work
	// For now, we just absolute position (like wave height) so that can use waves currents
	// in storms
	UPROPERTY(EditDefaultsOnly, Category = OceanSetup)
	bool bIsCurrentRelative;

	UPROPERTY(EditDefaultsOnly, Category = OceanSetup)
	bool bIsDisplacementRelative;

	UPROPERTY(EditDefaultsOnly, Category = OceanSetup)
	float CurrentPatchSize;

	//////////////////////////////////////////////////////////////////////////
	// Buffered data access API

	/** Displacement value from XY pixel */
	FLinearColor GetDisplacementColor(int32 X, int32 Y) const;

	/** Normals value from XY pixel */
	FLinearColor GetGradientColor(int32 X, int32 Y) const;

public:
	// Begin UActorComponent Interface
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// End UActorComponent Interface

	// Begin UObject Interface
	virtual void BeginDestroy() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	// End UObject Interface

	/** Update normals and heightmap from spectrum */
	void UpdateContent();
	void UpdateDisplacementMap(float WorldTime);
	void UpdateDisplacementArray();

	float GetOceanLevelAtLocation(const FVector& Location) const;
	FVector GetCurrentAtLocation(const FVector& Location) const;
protected:

	//////////////////////////////////////////////////////////////////////////
	// Spectrum initialization

	/** Initialize the vector field */
	void InitHeightMap(const FSpectrumData& Params, TResourceArray<FVector2D>& out_h0, TResourceArray<float>& out_omega);

	void CreateBufferAndUAV(FResourceArrayInterface* Data, uint32 byte_width, uint32 byte_stride,
		FStructuredBufferRHIRef* ppBuffer, FUnorderedAccessViewRHIRef* ppUAV, FShaderResourceViewRHIRef* ppSRV);

	//////////////////////////////////////////////////////////////////////////
	// Parameters that will be send to rendering thread

	FUpdateSpectrumCSImmutable UpdateSpectrumCSImmutableParams;


	//////////////////////////////////////////////////////////////////////////
	// Spectrum simulation data

	// Initial height field H(0) generated by Phillips spectrum & Gauss distribution.
	FStructuredBufferRHIRef m_pBuffer_Float2_H0;
	FUnorderedAccessViewRHIRef m_pUAV_H0;
	FShaderResourceViewRHIRef m_pSRV_H0;

	// Angular frequency
	FStructuredBufferRHIRef m_pBuffer_Float_Omega;
	FUnorderedAccessViewRHIRef m_pUAV_Omega;
	FShaderResourceViewRHIRef m_pSRV_Omega;

	// Height field H(t), choppy field Dx(t) and Dy(t) in frequency domain, updated each frame.
	FStructuredBufferRHIRef m_pBuffer_Float2_Ht;
	FUnorderedAccessViewRHIRef m_pUAV_Ht;
	FShaderResourceViewRHIRef m_pSRV_Ht;

	// Height & choppy buffer in the space domain, corresponding to H(t), Dx(t) and Dy(t)
	FStructuredBufferRHIRef m_pBuffer_Float_Dxyz;
	FUnorderedAccessViewRHIRef m_pUAV_Dxyz;
	FShaderResourceViewRHIRef m_pSRV_Dxyz;

	FVector4 m_pQuadVB[4];

	// FFT wrap-up
	FRadixPlan512 FFTPlan;


	// Data for querying the ocean
	TArray<FFloat16Color> ColorBuffer;
	virtual FFloat16Color GetHeightMapPixelColor(float U, float V) const;
};
