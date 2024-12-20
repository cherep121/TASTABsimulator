// Fill out your copyright notice in the Description page of Project Settings.


#include "MainField.h"
#include <memory.h>

// Sets default values
AMainField::AMainField()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	gridBase = CreateDefaultSubobject<USceneComponent>(TEXT("Grid Base Component"));
	SetRootComponent(gridBase);

	mainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Main Camera"));
	mainCamera->SetupAttachment(gridBase);
	mainCamera->SetRelativeLocation(FVector(0.0, 0.0, 1000.0));
	mainCamera->SetWorldRotation(FQuat(0.5, -0.5, -0.5, -0.5));
	//mainCamera->SetWorldRotation(FQuat());

	tileMeshes.SetNum(19);
	tileMaterials.SetNum(19);
	{
		{
			static ConstructorHelpers::FObjectFinder<UStaticMesh> meshToLoad(TEXT("/Game/Models/floor"));
			for (size_t i = 0; i < 16; i++)
				tileMeshes[i] = meshToLoad.Object.Get();
		}

		{
			static ConstructorHelpers::FObjectFinder<UStaticMesh> meshToLoad(TEXT("/Game/Models/wall"));
			for (size_t i = 16; i < 20; i++)
				tileMeshes[i] = meshToLoad.Object.Get();
		}

		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/floor1Mat"));
			tileMaterials[0] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/floor2Mat"));
			tileMaterials[1] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/bushMat"));
			tileMaterials[2] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/crateMat"));
			tileMaterials[3] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/player1Mat"));
			tileMaterials[4] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/player2Mat"));
			tileMaterials[5] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/player3Mat"));
			tileMaterials[6] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/player4Mat"));
			tileMaterials[7] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/pineMat"));
			tileMaterials[8] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/rockMat"));
			tileMaterials[9] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/treeMat"));
			tileMaterials[10] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/waterMat"));
			tileMaterials[11] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/cracked1Mat"));
			tileMaterials[12] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/cracked2Mat"));
			tileMaterials[13] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/cracked3Mat"));
			tileMaterials[14] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/cracked4Mat"));
			tileMaterials[15] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/wall1Mat"));
			tileMaterials[16] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/wall2Mat"));
			tileMaterials[17] = materialToLoad.Object.Get();
		}
		{
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialToLoad(TEXT("/Game/Materials/wall3Mat"));
			tileMaterials[18] = materialToLoad.Object.Get();
		}
	}

	theState = 1;

	for (size_t i = 0; i < 6; i++)
	{
		inputData[i] = 0;
	}

	timeTreshold = 0.0f;
}

THIRD_PARTY_INCLUDES_START
#include "ThirdParty/WorldFStream.h"
THIRD_PARTY_INCLUDES_END

AMainField::~AMainField()
{
	(((WorldFStream*)worldData)[0]).~WorldFStream();
}

// Called when the game starts or when spawned
void AMainField::BeginPlay()
{
	Super::BeginPlay();

	new (worldData) WorldFStream();

	gridTiles.SetNum(256 * 256);
	wchar_t nameThing[5] = { L'0', L'0', L'0', L'0', L'\0' };
	for (size_t Y = 0; Y < 4; Y++)
	{
		for (size_t X = 0; X < 4; X++)
		{
			for (size_t y = 0; y < 16; y++)
			{
				for (size_t x = 0; x < 16; x++)
				{
					nameThing[0] = (wchar_t)((((x + y * 16 + X * 256 + Y * 4096) / 1000) % 10) + L'0');
					nameThing[1] = (wchar_t)((((x + y * 16 + X * 256 + Y * 4096) / 100) % 10) + L'0');
					nameThing[2] = (wchar_t)((((x + y * 16 + X * 256 + Y * 4096) / 10) % 10) + L'0');
					nameThing[3] = (wchar_t)((((x + y * 16 + X * 256 + Y * 4096) / 1) % 10) + L'0');
					gridTiles[x + y * 16 + X * 256 + Y * 4096] = CreateDefaultSubobject<UStaticMeshComponent>(nameThing);
					gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetupAttachment(gridBase);
					gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetRelativeLocation(FVector((double)(x * 100), (double)(y * 100), 0.0));
					gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
					gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetSimulatePhysics(false);
				}
			}
		}
	}

	isLoading = 0;
}

 //Called every frame
#define px playerPos[0]
#define pX playerPos[1]
#define py playerPos[2]
#define pY playerPos[3]
void AMainField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (theState)
	{
	case 1:
		((char**)tempGarbache)[0] = (char*)FMemory::Malloc(4096);
		if (!(((WorldFStream*)worldData)[0]).Init(((char**)tempGarbache)[0], 4096))
		{
			theState = 0;
			FMemory::Free(((char**)tempGarbache)[0]);
			break;
		}
		theState = 2;
		break;
	case 2:
		if (!(((WorldFStream*)worldData)[0]).InitProcess(((char**)tempGarbache)[0]))
		{
			theState = 3;
			FMemory::Free(((char**)tempGarbache)[0]);
		}
		break;
	case 3:
		for (size_t Y = 0; Y < 4; Y++)
		{
			for (size_t X = 0; X < 4; X++)
			{
				for (size_t y = 0; y < 16; y++)
				{
					for (size_t x = 0; x < 16; x++)
					{
						unsigned int tileId = (((WorldFStream*)worldData)[0]).segments[X][Y][x + y * 16];
						gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetStaticMesh(tileMeshes[tileId]);
						gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetMaterial(0, tileMaterials[tileId]);
						if ((tileId >= 2) && (tileId >= 2))
						{
							playerPos[0] = x;
							playerPos[1] = X;
							playerPos[2] = y;
							playerPos[3] = Y;
						}
					}
				}

				return;
			}
		}
		mainCamera->SetRelativeLocation((double)((playerPos[0] + playerPos[1] * 16) * 100 + 50), (double)((playerPos[2] + playerPos[3] * 16) * 100 + 50), 1000.0);
		theState = 4;
		break;
	case 3:
		if (timeTreshold > 0.2f)
		{
			timeTreshold = 0.0f;

			//W
			if ((inputData[0] == 1) || (inputData[0] == 5))
			{
				unsigned int nextTileId;
				if (plaerPos[2] == 15)
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX][playerPos[3] + 1][px];
				else
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX][playerPos[3]][px + (py + 1) * 16];

				if (nextTileId < 4)
				{
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 0;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[0]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[0]);
					if (py == 15)
					{
						py = 0;
						pY++;
					}
					else
					{
						py++;
					}
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 4;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[2]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[2]);
					mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
				}
			}

			//A
			if ((inputData[1] == 1) || (inputData[1] == 5))
			{
				unsigned int nextTileId;
				if (plaerPos[0] == 15)
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX + 1][pY][py * 16];
				else
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX][pY][(px + 1) + py * 16];

				if (nextTileId < 4)
				{
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 0;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[0]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[0]);
					if (px == 15)
					{
						px = 0;
						pX++;
					}
					else
					{
						px++;
					}
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 7;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[5]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[5]);
					mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
				}
			}

			//S
			if ((inputData[2] == 1) || (inputData[2] == 5))
			{
				unsigned int nextTileId;
				if (plaerPos[1] == 0)
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX][pY - 1][px + 15 * 16];
				else
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX][pY][px + (py - 1) * 16];

				if (nextTileId < 4)
				{
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 0;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[0]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[0]);
					if (px == 15)
					{
						py = 15;
						pY--;
					}
					else
					{
						py--;
					}
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 6;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[4]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[4]);
					mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
				}
			}

			//D
			if ((inputData[3] == 1) || (inputData[3] == 5))
			{
				unsigned int nextTileId;
				if (plaerPos[0] == 0)
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX - 1][pY][15 + py * 16];
				else
					nextTileId = (((WorldFStream*)worldData)[0]).segments[pX][pY][(px - 1) + py * 16];

				if (nextTileId < 4)
				{
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 0;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[0]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[0]);
					if (px == 15)
					{
						px = 15;
						pX--;
					}
					else
					{
						px--;
					}
					(((WorldFStream*)worldData)[0]).segments[pX][pY][px + py * 16] = 5;
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetStaticMesh(tileMeshes[3]);
					gridTiles[px + py * 16 + pX * 256 + pY * 4096]->SetMaterial(0, tileMaterials[3]);
					mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
				}
			}

			for (size_t i = 0; i < 6; i++)
			{
				if ((inputData[i] > 0) && (inputData[i] < 5))
				{
					inputData[i]++;
				}
			}

			if (pY > 2)
			{
				(((WorldFStream*)worldData)[0]).SetPos((((WorldFStream*)worldData)[0]).posXLoad, (((WorldFStream*)worldData)[0]).posYLoad + 1, (char**)gridTiles.GetData());
				isLoading = 1;
				pY--;
				mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
			}
			else if (pX > 2)
			{
				(((WorldFStream*)worldData)[0]).SetPos((((WorldFStream*)worldData)[0]).posXLoad + 1, (((WorldFStream*)worldData)[0]).posYLoad, (char**)gridTiles.GetData());
				isLoading = 1;
				pX--;
				mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
			}
			else if (pY < 1)
			{
				(((WorldFStream*)worldData)[0]).SetPos((((WorldFStream*)worldData)[0]).posXLoad, (((WorldFStream*)worldData)[0]).posYLoad - 1, (char**)gridTiles.GetData());
				isLoading = 1;
				pY++;
				mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
			}
			else if (pX < 1)
			{
				(((WorldFStream*)worldData)[0]).SetPos((((WorldFStream*)worldData)[0]).posXLoad - 1, (((WorldFStream*)worldData)[0]).posYLoad, (char**)gridTiles.GetData());
				isLoading = 1;
				pX++;
				mainCamera->SetRelativeLocation(FVector((double)((px + pX * 16) * 100 + 50), (double)((py + pY * 16) * 100 + 50), 1000.0));
			}

			if (isLoading)
			{
				isLoading = (unsigned int)(((WorldFStream*)worldData)[0]).LoadProcess((bool*)tilesToUpdate);

				for (size_t Y = 0; Y < 4; Y++)
				{
					for (size_t X = 0; X < 4; X++)
					{
						if (((bool*)tilesToUpdate)[X + Y * 4])
						{
							for (size_t y = 0; y < 16; y++)
							{
								for (size_t x = 0; x < 16; x++)
								{
									unsigned int tileId = (((WorldFStream*)worldData)[0]).segments[X][Y][x + y * 16];
									gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetStaticMesh(tileMeshes[tileId]);
									gridTiles[x + y * 16 + X * 256 + Y * 4096]->SetMaterial(0, tileMaterials[tileId]);
								}
							}

							timeTreshold += DeltaTime;
							return;
						}
					}
				}
			}
		}

		timeTreshold += DeltaTime;
		break;
	}
}

// Called to bind functionality to input
void AMainField::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("BasicInput", EInputEvent::IE_Pressed, this, &AMainField::KeyPressed);
	PlayerInputComponent->BindAction("BasicInput", EInputEvent::IE_Released, this, &AMainField::KeyReleased);
}

void AMainField::KeyPressed(FKey key)
{
	FKey in = key.GetFName();

	FKey in = key.GetFName();

	if (in.ToString() == "W")
		inputData[0] = 1;
	else if (in.ToString() == "A")
		inputData[1] = 1;
	else if (in.ToString() == "S")
		inputData[2] = 1;
	else if (in.ToString() == "D")
		inputData[3] = 1;
}

void AMainField::KeyReleased(FKey key)
{
	FKey in = key.GetFName();

	if (in.ToString() == "W")
		inputData[0] = 0;
	else if (in.ToString() == "A")
		inputData[1] = 0;
	else if (in.ToString() == "S")
		inputData[2] = 0;
	else if (in.ToString() == "D")
		inputData[3] = 0;
}