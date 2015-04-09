#include "Isotops.h"
#include "NucleusModel.h"

#include <cassert>
#include <cmath>

struct Branch {
	float probability;
	const FDecayMode* decays_begin;
	const FDecayMode* decays_end;
};

struct Isotope {
	double half_life;
	const Branch* branches_begin;
	const Branch* branches_end;
};

struct Element {
	FString name;
	size_t min_neutrons;
	const Isotope* isotopes_begin;
	const Isotope* isotopes_end;
};

#include "NucleusModelConstants.h"

const Element* getElement(size_t protons) {
	auto element = elements_begin + protons;
	if (element >= elements_end) {
		// unknown element
		return nullptr;
	}
	
	return element;
}

const Isotope* getIsotope(const Element* element, size_t neutrons) {
//	if (element == nullptr) {
//		// unknown element
//		return nullptr;
//	}
	assert(element != nullptr);
	
	auto isotope_index = neutrons - element->min_neutrons;
	if (isotope_index < 0) {
		// not enough neutrons
		return nullptr;
	}
	
	auto isotope = element->isotopes_begin + isotope_index;
	if (isotope >= element->isotopes_end) {
		// too many neutrons
		return nullptr;
	}
	
	return isotope;
}

FNucleon UNucleusModel::Create(float Random, uint32 Protons, uint32 Neutrons) {
	auto element = getElement(Protons);
	if (element == nullptr) {
		// unknown element
		return FNucleon { FString::FormatAsNumber(Protons), 0.0 };
	}
	
	auto isotope = getIsotope(element, Neutrons);
	if (isotope == nullptr) {
		// unknown isotope
		return FNucleon { element->name, 0.0 };
	}
	
	auto half_life = isotope->half_life;

	// TODO: compute life
	auto life = half_life;
	
	return FNucleon { element->name, half_life, life };
}

TArray<FDecayMode> BranchDecay(const Branch* branch) {
	TArray<FDecayMode> result;
	result.Append(branch->decays_begin, branch->decays_end - branch->decays_begin);
	// TODO: spontaneous fission
	return result;
}

TArray<FDecayMode> UnknownDecay(float Random, uint32 Protons, uint32 Neutrons) {
	// TODO: compute unknown decay
	return TArray<FDecayMode>();
}

TArray<FDecayMode> UNucleusModel::Decay(float Random, uint32 Protons, uint32 Neutrons) {
	auto element = getElement(Protons);
	if (element == nullptr) {
		// unknown element
		return UnknownDecay(Random, Protons, Neutrons);
	}
	
	auto isotope = getIsotope(element, Neutrons);
	if (isotope == nullptr) {
		// unknown isotope
		return UnknownDecay(Random, Protons, Neutrons);
	}
	
	if (std::isinf(isotope->half_life)) {
		// stable
		// TODO: emit a warning
		return TArray<FDecayMode>();
	}

	for (auto branch = isotope->branches_begin; branch != isotope->branches_end; ++branch) {
		Random -= branch->probability;
		if (Random <= 0) {
			// take this branch
			return BranchDecay(branch);
		}
	}
	
	// inconsistent data
	assert(isotope->branches_begin < isotope->branches_end);
	return BranchDecay(isotope->branches_end - 1);
}

