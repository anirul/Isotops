#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NucleusModel.generated.h"

USTRUCT(BlueprintType)
struct FNucleon {
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Element;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double HalfLife;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Life;
};

UENUM(BlueprintType)
enum class EDecayType : uint8 {
	Bplus UMETA(DisplayName="Beta+"),
	Bminus UMETA(DisplayName="Beta-"),
	EC UMETA(DisplayName="Electron capture"),
	Eplus UMETA(DisplayName="e+???"),
	Nucleon UMETA(DisplayName="Nucleon ejection"),
};

USTRUCT(BlueprintType)
struct FDecayMode {
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDecayType Type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 Protons;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 Neutrons;
};

UCLASS()
class ISOTOPS_API UNucleusModel : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=NucleusModel)
	static FNucleon Create(uint32 Protons, uint32 Neutrons, float Random);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=NucleusModel)
	static TArray<FDecayMode> Decay(uint32 Protons, uint32 Neutrons, float Random);
	
};
