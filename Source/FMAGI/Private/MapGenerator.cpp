#include "MapGenerator.h"

AMapGenerator::AMapGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	_thisScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = _thisScene;

	_thisMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	_thisMesh->SetupAttachment(RootComponent);
}

void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void AMapGenerator::StartPerlinSpawn()
{
	for (int j = 0; j < _objectSpawnCount; j++)
	{
		for (int i = 0; i < _objectSpawnCount; i++)
		{
			GetNoiseValue((float) i / _noiseSize, (float) j / _noiseSize, _noiseVal);

			FVector spawnPosition = GetActorLocation() + GetActorRightVector() * (i * _spawnWidth);
			spawnPosition = spawnPosition + (GetActorForwardVector() * (j* _spawnWidth));
			spawnPosition = spawnPosition + (GetActorUpVector() * (spawnPosition.Z + (_noiseVal * _heightMultiplier)));

			FActorSpawnParameters params;
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FRotator rot = FRotator::ZeroRotator;

			AActor* spawnedActor = GetWorld()->SpawnActor<AActor>(_objectToSpawn, spawnPosition, rot, params);
		}
	}
}

void AMapGenerator::PostActorCreated()
{
	Super::PostActorCreated();
	GenerateMesh();
}

void AMapGenerator::PostLoad()
{
	Super::PostLoad();
}

void AMapGenerator::GenerateMesh()
{
	_meshProperties = new MeshProperties();
	_meshProperties->Reset();

	int32 triangleIndexCount = 0;
	FVector definedShape[4];
	FProcMeshTangent tangentSetup;

	definedShape[0] = FVector(_cubeRadius.X, _cubeRadius.Y, _cubeRadius.Z);
	definedShape[1] = FVector(_cubeRadius.X, _cubeRadius.Y, -_cubeRadius.Z);
	definedShape[2] = FVector(_cubeRadius.X, -_cubeRadius.Y, _cubeRadius.Z);
	definedShape[3] = FVector(_cubeRadius.X, -_cubeRadius.Y, -_cubeRadius.Z);

	tangentSetup = FProcMeshTangent(0, 1, 0);
	AddQuadMesh(definedShape[0], definedShape[1], definedShape[2], definedShape[3], triangleIndexCount, tangentSetup);

	_meshProperties->CreateMesh(0, _thisMesh, true);
}

void AMapGenerator::AddTriangleMesh(FVector topRight_, FVector bottomRight_, FVector bottomLeft_, int32 & triIndex_, FProcMeshTangent tangent_)
{
	int32 point1 = triIndex_++;
	int32 point2 = triIndex_++;
	int32 point3 = triIndex_++;

	_meshProperties->AddVertex(topRight_, bottomRight_, bottomLeft_);
	_meshProperties->AddTriangle(point1, point2, point3);

	FVector _thisNormal = FVector::CrossProduct(topRight_, bottomRight_).GetSafeNormal();
	for (int i = 0; i < 3; i++)
	{
		_meshProperties->SetVertexProperties(_thisNormal, tangent_, FLinearColor::Green);
	}

	_meshProperties->AddUVs(FVector2D(0, 1), FVector2D(0, 0), FVector2D(1, 0));
}

void AMapGenerator::AddQuadMesh(FVector topRight_, FVector bottomRight_, FVector topLeft_, FVector bottomLeft_, int32 & triIndex_, FProcMeshTangent tangent_)
{
	int32 point1 = triIndex_++;
	int32 point2 = triIndex_++;
	int32 point3 = triIndex_++;
	int32 point4 = triIndex_++;

	_meshProperties->AddVertex(topRight_, bottomRight_, topLeft_, bottomLeft_);
	_meshProperties->AddTriangle(point1, point2, point3);
	_meshProperties->AddTriangle(point4, point3, point2);

	FVector _thisNormal = FVector::CrossProduct(topLeft_ - bottomRight_, topLeft_ - topRight_).GetSafeNormal();
	for (int i = 0; i < 4; i++)
	{
		_meshProperties->SetVertexProperties(_thisNormal, tangent_, FLinearColor::Green);
	}

	_meshProperties->AddUVs(FVector2D(0, 1), FVector2D(0, 0), FVector2D(1, 0));
}

