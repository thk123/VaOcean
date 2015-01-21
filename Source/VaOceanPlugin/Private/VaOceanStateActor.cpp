// Copyright 2014 Vladimir Alyamkin. All Rights Reserved.

#include "VaOceanPluginPrivatePCH.h"
#include "VaOceanStateActor.h"

AVaOceanStateActor::AVaOceanStateActor(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
#if WITH_EDITORONLY_DATA
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		// A helper class object we use to find target UTexture2D object in resource package
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> NoteTextureObject;

		// Icon sprite category name
		FName ID_Notes;

		// Icon sprite display name
		FText NAME_Notes;

		FConstructorStatics()
			// Use helper class object to find the texture
			// "/Engine/EditorResources/S_Note" is resource path
			: NoteTextureObject(TEXT("/Engine/EditorResources/S_Note"))
			, ID_Notes(TEXT("Notes"))
			, NAME_Notes(NSLOCTEXT("SpriteCategory", "Notes", "Notes"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;
#endif // WITH_EDITORONLY_DATA

	// We need a scene component to attach Icon sprite
	USceneComponent* SceneComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComp"));
	RootComponent = SceneComponent;
	RootComponent->Mobility = EComponentMobility::Movable;

#if WITH_EDITORONLY_DATA
	SpriteComponent = PCIP.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("Sprite"));
	if (SpriteComponent)
	{
		SpriteComponent->Sprite = ConstructorStatics.NoteTextureObject.Get();		// Get the sprite texture from helper class object
		SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Notes;			// Assign sprite category name
		SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Notes;	// Assign sprite display name
		SpriteComponent->AttachParent = RootComponent;								// Attach sprite to scene component
	}
#endif // WITH_EDITORONLY_DATA

	OceanSimulator = NULL;

	bIsDrawingDebugSpheres = false;
	bIsDrawingDebugCurrentLines = false;
	HalfNumberOfDebugSpheres = 25;


	// Enable to see debug spheres
	PrimaryActorTick.bCanEverTick = true;
}

void AVaOceanStateActor::PreInitializeComponents()
{
	OceanSimulator = FindComponentByClass<UVaOceanSimulatorComponent>();

	Super::PreInitializeComponents();
}

void AVaOceanStateActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


//////////////////////////////////////////////////////////////////////////
// Ocean state API

const FSpectrumData& AVaOceanStateActor::GetSpectrumConfig() const
{
	return SpectrumConfig;
}

float AVaOceanStateActor::GetOceanLevelAtLocation(const FVector& Location) const
{
	// @TODO Get altitute from ocean simulation component

	if (OceanSimulator)
	{
		return OceanSimulator->GetOceanLevelAtLocation(Location);
	}

	return GetGlobalOceanLevel();
}

FVector AVaOceanStateActor::GetCurrentAtLocation(const FVector& Location) const
{
	if (OceanSimulator)
	{
		return OceanSimulator->GetCurrentAtLocation(Location);
	}

	return FVector::ZeroVector;
}

FLinearColor AVaOceanStateActor::GetOceanSurfaceNormal(FVector& Location) const
{
	// @TODO Get surface normals from ocean simulation component

	return FLinearColor::Blue;
}

FVector AVaOceanStateActor::GetOceanWaveVelocity(FVector& Location) const
{
	// @TODO Velocity should be calculated in ocean simulation component

	return FVector::UpVector;
}

int32 AVaOceanStateActor::GetOceanWavesNum() const
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// Parameters access (get/set)

void AVaOceanStateActor::SetGlobalOceanLevel(float OceanLevel)
{
	GlobalOceanLevel = OceanLevel;
}

float AVaOceanStateActor::GetGlobalOceanLevel() const
{
	return GlobalOceanLevel;
}

void AVaOceanStateActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// draw debug speheres
	if (bIsDrawingDebugSpheres || bIsDrawingDebugCurrentLines)
	{
		const float GapBetweenSpheres = (GetOceanPatchSize() / 2.0f) / HalfNumberOfDebugSpheres;
		for (int i = -HalfNumberOfDebugSpheres; i < HalfNumberOfDebugSpheres; ++i)
		{
			for (int j = -HalfNumberOfDebugSpheres; j < HalfNumberOfDebugSpheres; ++j)
			{
				FVector SpherePoint = GetActorLocation() + FVector(GapBetweenSpheres * i, GapBetweenSpheres * j, 0.0f);
				float OceanHeight = GetOceanLevelAtLocation(SpherePoint);

				if (bIsDrawingDebugSpheres)
				{
					DrawDebugSphere(GetWorld(), SpherePoint + (FVector::UpVector * OceanHeight), 10.0f, 4, FColor::White, false, -1.0f, 0);
				}

				if (bIsDrawingDebugCurrentLines)
				{
					FVector OceanCurrent = GetCurrentAtLocation(SpherePoint);
					if (OceanCurrent.SizeSquared2D() > 0.0f)
					{
						DrawDebugLine(GetWorld(), SpherePoint + (FVector::UpVector * OceanHeight), SpherePoint + (100.0f * OceanCurrent) + (FVector::UpVector * OceanHeight), FColor::White, false, 0.0f, 0, 10.0f);
					}
				}
			}
		}
	}
}

float AVaOceanStateActor::GetOceanPatchSize()
{
	if (OceanSimulator)
	{
		return OceanSimulator->PatchSize;
	}
	else
	{
		return 0.0f;
	}
}
