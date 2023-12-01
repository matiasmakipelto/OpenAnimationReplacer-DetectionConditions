#pragma once
#include "API/OpenAnimationReplacerAPI-Conditions.h"
#include "API/OpenAnimationReplacer-ConditionTypes.h"

namespace Conditions
{
	class DetectionCondition : public CustomCondition
	{
		public:
			DetectionCondition();
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
			//Distance
			IBoolConditionComponent* useBoolComponent5;
			IComparisonConditionComponent* comparisonComponent2;
			INumericConditionComponent* numericComponent2;
			//Keyword
			IBoolConditionComponent* useBoolComponent6;
			IBoolConditionComponent* notComponent3;
			IKeywordConditionComponent* keywordComponent;
			//Compare value
			IBoolConditionComponent* useBoolComponent7;
			INumericConditionComponent* numericComponent3;
			IComparisonConditionComponent* comparisonComponent3;
			INumericConditionComponent* numericComponent4;
			
		protected:
			bool IsRelationship(RE::TESNPC* base, RE::TESNPC* target, RE::TESObjectREFR* a_refr) const;
			bool FitsConditions(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr) const;
	};


	class IsDetectedByCondition : public DetectionCondition
	{
	public:
		// Mandatory stuff
		constexpr static inline std::string_view CONDITION_NAME = "IsDetectedBy"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "Checks if an actor that fulfills all of the child conditions sees the actor."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 1, 2, 1 }; }

		using DetectionCondition::DetectionCondition;

	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator) const override;
	};


	class DetectsCondition : public DetectionCondition
	{
	public:
		// Mandatory stuff
		constexpr static inline std::string_view CONDITION_NAME = "Detects"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "True if an actor that fulfills the conditions is visible to the actor."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 1, 2, 1 }; }
	
		using DetectionCondition::DetectionCondition;

	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator) const override;
	};
}
