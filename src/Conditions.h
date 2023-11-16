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
		RE::BSString GetDescription() const override { return "True if an actor that fulfills the conditions sees the actor."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 1, 1, 0 }; }



		// Good stuff
		IsDetectedByCondition();
		//Human
		IBoolConditionComponent* useBoolComponent;
		IBoolConditionComponent* isHuman;
		//Gender
		IBoolConditionComponent* useBoolComponent1;
		IBoolConditionComponent* isFemale;
		//Relationship
		IBoolConditionComponent* useBoolComponent2;
		IComparisonConditionComponent* comparisonComponent;
		INumericConditionComponent* numericComponent;
		//Faction
		IBoolConditionComponent* useBoolComponent3;
		IBoolConditionComponent* notComponent;
		IFormConditionComponent* factionComponent;
		//ActorBase
		IBoolConditionComponent* useBoolComponent4;
		IBoolConditionComponent* notComponent2;
		IFormConditionComponent* actorBaseComponent;

	protected:
		bool IsRelationship(RE::TESNPC* base, RE::TESNPC* target, RE::TESObjectREFR* a_refr) const;
		bool FitsConditions(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr) const;
		bool ValidateTarget(RE::Actor* actor, RE::Actor* target) const;
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator) const override;
	};


	class DetectsCondition : public CustomCondition
	{
	public:
		// Mandatory stuff
		constexpr static inline std::string_view CONDITION_NAME = "Detects"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "True if an actor that fulfills the conditions is visible to the actor."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 1, 1, 0 }; }
	
	
	
		// Good stuff
		DetectsCondition();
		//Human
		IBoolConditionComponent* useBoolComponent;
		IBoolConditionComponent* isHuman;
		//Gender
		IBoolConditionComponent* useBoolComponent1;
		IBoolConditionComponent* isFemale;
		//Relationship
		IBoolConditionComponent* useBoolComponent2;
		IComparisonConditionComponent* comparisonComponent;
		INumericConditionComponent* numericComponent;
		//Faction
		IBoolConditionComponent* useBoolComponent3;
		IBoolConditionComponent* notComponent;
		IFormConditionComponent* factionComponent;
		//ActorBase
		IBoolConditionComponent* useBoolComponent4;
		IBoolConditionComponent* notComponent2;
		IFormConditionComponent* actorBaseComponent;
	protected:
		bool IsRelationship(RE::TESNPC* base, RE::TESNPC* target, RE::TESObjectREFR* a_refr) const;
		bool FitsConditions(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr) const;
		bool ValidateTarget(RE::Actor* actor, RE::Actor* target) const;
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator) const override;
	};
}
