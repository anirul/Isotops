#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NucleusModel.generated.h"

USTRUCT(BlueprintType)
struct FNucleon {
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Element;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HalfLife;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Life;
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
	int32 Protons;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Neutrons;
};

UCLASS()
class ISOTOPS_API UNucleusModel : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=NucleusModel)
	static FNucleon Create(int32 Protons, int32 Neutrons, float Random);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=NucleusModel)
	static TArray<FDecayMode> Decay(int32 Protons, int32 Neutrons, float Random);
	
};
