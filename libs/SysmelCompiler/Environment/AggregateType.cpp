#include "Environment/AggregateType.hpp"
#include "Environment/AggregateTypeLayout.hpp"
#include "Environment/ReferenceType.hpp"
#include "Environment/AggregateElementReference.hpp"
#include "Environment/PointerLikeType.hpp"
#include "Environment/SubclassResponsibility.hpp"
#include "Environment/BootstrapTypeRegistration.hpp"
#include "Environment/StringUtilities.hpp"
#include <sstream>

namespace Sysmel
{
namespace Environment
{

static BootstrapTypeRegistration<AggregateTypeValue> AggregateTypeValueTypeRegistration;

bool AggregateType::isAggregateType() const
{
    return true;
}

uint64_t AggregateType::getMemorySize()
{
    getLayout();
    return layout ? layout->getMemorySize() : 0;
}

uint64_t AggregateType::getMemoryAlignment()
{
    getLayout();
    return layout ? layout->getMemoryAlignment() : 0;
}

bool AggregateType::isNullableType() const
{
    return false;
}

bool AggregateType::isImmutableType()
{
    return false;
}

bool AggregateType::isPassedByReference()
{
    return true;
}

bool AggregateType::isReturnedByReference()
{
    return true;
}

TypePtr AggregateType::asConstReceiverType()
{
    return selfFromThis()->withConst()->ref();
}

TypePtr AggregateType::asReceiverType()
{
    return selfFromThis()->ref();
}

bool AggregateType::hasTrivialInitialization()
{
    getLayout();
    return hasTrivialInitialization_;
}

bool AggregateType::hasTrivialInitializationCopyingFrom()
{
    getLayout();
    return hasTrivialInitializationCopyingFrom_;
}

bool AggregateType::hasTrivialInitializationMovingFrom()
{
    getLayout();
    return hasTrivialInitializationMovingFrom_;
}

bool AggregateType::hasTrivialFinalization()
{
    getLayout();
    return hasTrivialFinalization_;
}

bool AggregateType::hasTrivialAssignCopyingFrom()
{
    getLayout();
    return hasTrivialAssignCopyingFrom_;
}

bool AggregateType::hasTrivialAssignMovingFrom()
{
    getLayout();
    return hasTrivialAssignMovingFrom_;
}

const AggregateTypeLayoutPtr &AggregateType::getLayout()
{
    if(!layout)
    {
        buildLayout();
        computeObjectLifetimeTriviality();
    }

    return layout;
}

void AggregateType::computeObjectLifetimeTriviality()
{
    evaluateAllPendingBodyBlockCodeFragments();
    hasTrivialInitialization_ = hasTrivialInitialization_ && SuperType::hasTrivialInitialization() && supertype->hasTrivialInitialization() && layout->hasTrivialInitialization();
    hasTrivialInitializationCopyingFrom_ = hasTrivialInitializationCopyingFrom_ && SuperType::hasTrivialInitializationCopyingFrom() && supertype->hasTrivialInitializationCopyingFrom() && layout->hasTrivialInitializationCopyingFrom();
    hasTrivialInitializationMovingFrom_ = hasTrivialInitializationMovingFrom_ && SuperType::hasTrivialInitializationMovingFrom() && supertype->hasTrivialInitializationMovingFrom() && layout->hasTrivialInitializationMovingFrom();
    hasTrivialFinalization_ = hasTrivialFinalization_&& SuperType::hasTrivialFinalization() && supertype->hasTrivialFinalization() && layout->hasTrivialFinalization();
    hasTrivialAssignCopyingFrom_ = hasTrivialAssignCopyingFrom_ && SuperType::hasTrivialAssignCopyingFrom() && supertype->hasTrivialAssignCopyingFrom() && layout->hasTrivialAssignCopyingFrom();
    hasTrivialAssignMovingFrom_ = hasTrivialAssignMovingFrom_ && SuperType::hasTrivialAssignMovingFrom() && supertype->hasTrivialAssignMovingFrom() && layout->hasTrivialAssignMovingFrom();
}

void AggregateType::ensureImplicitLifeTimeMethodsAreCreated()
{
}

void AggregateType::buildLayout()
{
    SysmelSelfSubclassResponsibility();
}

bool AggregateTypeValue::isAggregateTypeValue() const
{
    return true;
}

AnyValuePtr AggregateTypeValue::asMutableStoreValue()
{
    return shallowClone();
}

AnyValuePtr AggregateTypeValue::getReferenceToSlotWithType(int64_t slotIndex, int64_t slotOffset, const TypePtr &referenceType)
{
    sysmelAssert(referenceType->isPointerLikeType());
    auto layout = type->getLayout();
    auto elementType = layout->getTypeForSlotAndOffset(slotIndex, slotOffset);
    if(!elementType)
        signalNewWithMessage<Error> (formatString("Invalid slot index {0} and/or offset: {1} for accessing aggregate of type {1}.", {castToString(slotIndex), castToString(slotOffset), type->printString()}));

    auto referenceValue = AggregateElementReference::make(selfFromThis(), elementType, slotIndex, slotOffset);
    return referenceType.staticAs<PointerLikeType>()->makeWithValue(referenceValue);
}

AnyValuePtr AggregateTypeValue::loadAggregateElement(int64_t slotIndex, int64_t slotOffset, const TypePtr &elementType)
{
    (void)slotOffset;
    if(slotIndex < 0 || size_t(slotIndex) >= slots.size())
        signalNewWithMessage<Error> ("Invalid aggregate load element.");

    return validAnyValue(slots[slotIndex])->accessVariableAsValueWithType(elementType);
}

AnyValuePtr AggregateTypeValue::copyAssignAggregateElement(int64_t slotIndex, int64_t slotOffset, const TypePtr &elementType, const AnyValuePtr &newValue)
{
    (void)slotOffset;
    (void)elementType;
    if(slotIndex < 0 || size_t(slotIndex) >= slots.size())
        signalNewWithMessage<Error> ("Invalid aggregate move asign element.");

    slots[slotIndex] = newValue;
    return newValue;
}

AnyValuePtr AggregateTypeValue::moveAssignAggregateElement(int64_t slotIndex, int64_t slotOffset, const TypePtr &elementType, const AnyValuePtr &newValue)
{
    (void)slotOffset;
    (void)elementType;
    if(slotIndex < 0 || size_t(slotIndex) >= slots.size())
        signalNewWithMessage<Error> ("Invalid aggregate move asign element.");

    slots[slotIndex] = newValue;
    return newValue;
}

} // End of namespace Environment
} // End of namespace Sysmel
