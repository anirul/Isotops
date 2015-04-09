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

FNucleon UNucleusModel::Create(uint32 Protons, uint32 Neutrons, float Random) {
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
	auto life = half_life * -log2(Random);
	
	return FNucleon { element->name, half_life, life };
}

TArray<FDecayMode> BranchDecay(uint32 protons, uint32 neutrons, const Branch* branch, float random) {
	auto decays_begin = branch->decays_begin;
	auto decays_end = branch->decays_end;

	TArray<FDecayMode> result;
	result.Reserve(decays_end - decays_begin);

	for (auto decay_it = decays_begin; decay_it != decays_end; ++decay_it) {
		auto decay = *decay_it;
		protons -= decay.Protons;
		neutrons -= decay.Neutrons;
		if (decay.Type == EDecayType::Nucleon && decay.Protons == 0 && decay.Neutrons == 0) {
			// spontaneous fission => random nucleon
			decay.Protons = protons * random;
			decay.Neutrons = neutrons * random;
		}
		result.Add(decay);
	}

	return result;
}

TArray<FDecayMode> UnknownDecay(uint32 protons, uint32 neutrons, float random) {
	// TODO: compute unknown decay
	return TArray<FDecayMode>();
}

TArray<FDecayMode> UNucleusModel::Decay(uint32 Protons, uint32 Neutrons, float Random) {
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
		// stable isotope: should not happen
		// TODO: emit a warning
		return TArray<FDecayMode>();
	}

	float interval_high = 1.0;
	for (auto branch = isotope->branches_begin; branch != isotope->branches_end; ++branch) {
		auto probability = branch->probability;
		auto interval_low = interval_high - probability;
		if (interval_low <= Random) {
			// take this branch
			auto fraction = (Random - interval_low) / probability;
			return BranchDecay(Protons, Neutrons, branch, fraction);
		}
		interval_high = interval_low;
	}
	
	// inconsistent data
	assert(isotope->branches_begin < isotope->branches_end);
	auto branch = isotope->branches_end - 1;
	auto fraction = Random / interval_high;
	return BranchDecay(Protons, Neutrons, branch, fraction);
}

