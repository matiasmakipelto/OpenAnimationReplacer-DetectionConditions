#pragma once
#include "API/OpenAnimationReplacerAPI-Conditions.h"
#include "API/OpenAnimationReplacer-ConditionTypes.h"

namespace Conditions
{
	class IsDetectedByCondition : public CustomCondition
	{
	public:
		// Mandatory stuff
		constexpr static inline std::string_view CONDITION_NAME = "IsDetectedBy"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "True if an actor that would affect stealth meter and fulfills conditions sees the actor."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 1, 0, 0 }; }



		// Good stuff
		std::string message;
		IsDetectedByCondition();
		//Relationship
		IBoolConditionComponent* useBoolComponent1;
		IComparisonConditionComponent* comparisonComponent;
		INumericConditionComponent* numericComponent;
		//Gender
		IBoolConditionComponent* useBoolComponent2;
		IBoolConditionComponent* isFemale;
		//Faction
		IBoolConditionComponent* useBoolComponent3;
		IBoolConditionComponent* notComponent;
		IFormConditionComponent* factionComponent;
	protected:
		bool IsRelationship(RE::TESNPC* base, RE::TESNPC* target, RE::TESObjectREFR* a_refr) const;
		// this is the function with the actual condition logic
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator) const override;
	};
}
