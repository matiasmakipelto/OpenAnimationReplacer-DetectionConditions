#pragma once
#include "API/OpenAnimationReplacerAPI-Conditions.h"
#include "API/OpenAnimationReplacer-ConditionTypes.h"

namespace Conditions
{
	class DetectionCondition : public CustomCondition
	{
	public:
		//Multi condition
		IMultiConditionComponent* multiComponent = static_cast<IMultiConditionComponent*>(AddBaseComponent(
			ConditionComponentType::kMulti,
			"Conditions"));

	protected:
		void PostInitialize() override {
			CustomCondition::PostInitialize();
			multiComponent->SetShouldDrawEvaluateResultForChildConditions(false);
		};
		RE::BSString GetCurrent(RE::TESObjectREFR*) const override { return resultText; };
		bool Evaluate(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod, bool isDetectedByCondition) const;
		bool ValidateTarget(RE::Actor* actor, RE::Actor* target) const;

		bool CheckMultiCondition(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod) const;
		mutable RE::BSString resultText = "";
	};

	class DetectedByCondition : public DetectionCondition
	{
	public:
		// Mandatory stuff
		constexpr static inline std::string_view CONDITION_NAME = "DETECTED_BY"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "Checks if an actor that fulfills all of the child conditions sees the actor."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 2, 0, 0 }; }

	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod) const override { return Evaluate(a_refr, a_clipGenerator, a_parentSubMod, true); };
	};

	class DetectsCondition : public DetectionCondition
	{
	public:
		// Mandatory stuff
		constexpr static inline std::string_view CONDITION_NAME = "DETECTS"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "True if an actor that fulfills the conditions is visible to the actor."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 2, 0, 0 }; }

	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod) const override { return Evaluate(a_refr, a_clipGenerator, a_parentSubMod, false); };
	};





	
	class DetectionDistanceCondition : public CustomCondition
	{
	public:
		IComparisonConditionComponent* comparisonComponent;
		INumericConditionComponent* numericComponent;
		DetectionDistanceCondition()
		{
			comparisonComponent = static_cast<IComparisonConditionComponent*>(AddBaseComponent(
				ConditionComponentType::kComparison,
				"Comparison"));
			numericComponent = static_cast<INumericConditionComponent*>(AddBaseComponent(
				ConditionComponentType::kNumeric,
				"Distance"));
		}


		constexpr static inline std::string_view CONDITION_NAME = "DetectionDistance"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "True if distance between detector and target meets the condition. Only usable with detection conditions."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 2, 0, 0 }; }

		bool CustomEvaluate(RE::TESObjectREFR* actor, const RE::Actor* target);

	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod) const override;
	};






	class DetectionRelationshipCondition : public CustomCondition
	{
	public:
		IComparisonConditionComponent* comparisonComponent;
		INumericConditionComponent* numericComponent;
		DetectionRelationshipCondition()
		{
			comparisonComponent = static_cast<IComparisonConditionComponent*>(AddBaseComponent(
				ConditionComponentType::kComparison,
				"Comparison"));
			numericComponent = static_cast<INumericConditionComponent*>(AddBaseComponent(
				ConditionComponentType::kNumeric,
				"Relationship [-4 = Archnemesis, .., 4 = Lover]"));
		}

		constexpr static inline std::string_view CONDITION_NAME = "DetectionRelationship"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "True if relationship between detector and target meets the condition. Only usable with detection conditions."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 2, 0, 0 }; }

		bool CustomEvaluate(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr);
	
	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod) const override;
	};
	




	class DetectionAngleCondition : public CustomCondition
	{
	public:
		IBoolConditionComponent* booleanComponent;
		IComparisonConditionComponent* comparisonComponent;
		INumericConditionComponent* numericComponent;
		DetectionAngleCondition()
		{
			booleanComponent = static_cast<IBoolConditionComponent*>(AddBaseComponent(
				ConditionComponentType::kBool,
				"Swap actor",
				"When false, the angle is from the actor being evaluated. When true, the angle is from the actor being detected in the case of DETECTS, or from the actor detecting in the case of DETECTED_BY."));
			comparisonComponent = static_cast<IComparisonConditionComponent*>(AddBaseComponent(
				ConditionComponentType::kComparison,
				"Comparison"));
			numericComponent = static_cast<INumericConditionComponent*>(AddBaseComponent(
				ConditionComponentType::kNumeric,
				"Angle in degrees",
				"Absolute angle from the direction an actor is facing."));
		}

		constexpr static inline std::string_view CONDITION_NAME = "DetectionAngle"sv;
		RE::BSString GetName() const override { return CONDITION_NAME.data(); }
		RE::BSString GetDescription() const override { return "True if angle from actor to target meets the condition. Only usable with detection conditions."sv.data(); }
		constexpr REL::Version GetRequiredVersion() const override { return { 2, 0, 1 }; }
	
		bool CustomEvaluate(RE::Actor* actor, RE::Actor* target, RE::TESObjectREFR* a_refr);

	protected:
		bool EvaluateImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_parentSubMod) const override;
	};
}
