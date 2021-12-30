#include "Environment/AggregateTypeSequentialLayout.hpp"
#include "Environment/Type.hpp"
#include "Environment/BootstrapTypeRegistration.hpp"
#include "Environment/Utilities.hpp"
#include <algorithm>
#include <sstream>

namespace Sysmel
{
namespace Environment
{

static BootstrapTypeRegistration<AggregateTypeSequentialLayout> AggregateTypeSequentialLayoutRegistration;

uint64_t AggregateTypeSequentialLayout::getMemorySize()
{
    return memorySize;
}

uint64_t AggregateTypeSequentialLayout::getMemoryAlignment()
{
    return memoryAlignment;
}

bool AggregateTypeSequentialLayout::hasTrivialInitialization()
{
    return hasTrivialInitialization_;
}

bool AggregateTypeSequentialLayout::hasTrivialInitializationCopyingFrom()
{
    return hasTrivialInitializationCopyingFrom_;
}

bool AggregateTypeSequentialLayout::hasTrivialInitializationMovingFrom()
{
    return hasTrivialInitializationMovingFrom_;
}

bool AggregateTypeSequentialLayout::hasTrivialFinalization()
{
    return hasTrivialFinalization_;
}

bool AggregateTypeSequentialLayout::hasTrivialAssignCopyingFrom()
{
    return hasTrivialAssignCopyingFrom_;
}

bool AggregateTypeSequentialLayout::hasTrivialAssignMovingFrom()
{
    return hasTrivialAssignMovingFrom_;
}

void AggregateTypeSequentialLayout::addInheritance(const AggregateTypeSequentialLayoutPtr &parentLayout)
{
    if(!parentLayout)
        return;

    // Inherit the triviality.
    hasTrivialInitialization_ = hasTrivialInitialization_ && parentLayout->hasTrivialInitialization();
    hasTrivialInitializationCopyingFrom_ = hasTrivialInitializationCopyingFrom_ && parentLayout->hasTrivialInitializationCopyingFrom();
    hasTrivialInitializationMovingFrom_ = hasTrivialInitializationMovingFrom_ && parentLayout->hasTrivialInitializationMovingFrom();
    hasTrivialFinalization_ = hasTrivialFinalization_ && parentLayout->hasTrivialFinalization();
    hasTrivialAssignCopyingFrom_ = hasTrivialAssignCopyingFrom_ && parentLayout->hasTrivialAssignCopyingFrom();
    hasTrivialAssignMovingFrom_ = hasTrivialAssignMovingFrom_ && parentLayout->hasTrivialAssignMovingFrom();

    // Append the parent layout slot types and offsets.
    const auto &parentSlotTypes = parentLayout->getSlotTypes();
    const auto &parentOffsets = parentLayout->getOffsets();

    slotTypes.insert(slotTypes.end(), parentSlotTypes.begin(), parentSlotTypes.end());
    offsets.reserve(parentOffsets.size());
    auto parentAlignment = parentLayout->getMemoryAlignment();
    if(memoryAlignment != 0 && parentAlignment)
    {
        memorySize = alignedTo(memorySize, parentAlignment);
        memoryAlignment = std::max(memoryAlignment, parentAlignment);
        auto extraOffset = memorySize;
        for(auto &parentSlotOffset : parentOffsets)
            offsets.push_back(parentSlotOffset + extraOffset);

        memorySize += parentLayout->getMemorySize();
    }
    else
    {
        offsets.insert(offsets.end(), parentOffsets.begin(), parentOffsets.end());
    }
}

uint32_t AggregateTypeSequentialLayout::addSlotWithType(const TypePtr &slotType)
{
    hasTrivialInitialization_ = hasTrivialInitialization_ && slotType->hasTrivialInitialization();
    hasTrivialInitializationCopyingFrom_ = hasTrivialInitializationCopyingFrom_ && slotType->hasTrivialInitializationCopyingFrom();
    hasTrivialInitializationMovingFrom_ = hasTrivialInitializationMovingFrom_ && slotType->hasTrivialInitializationMovingFrom();
    hasTrivialFinalization_ = hasTrivialFinalization_ && slotType->hasTrivialFinalization();
    hasTrivialAssignCopyingFrom_ = hasTrivialAssignCopyingFrom_ && slotType->hasTrivialAssignCopyingFrom();
    hasTrivialAssignMovingFrom_ = hasTrivialAssignMovingFrom_ && slotType->hasTrivialAssignMovingFrom();

    auto offset = memorySize;
    if(memoryAlignment != 0)
    {
        auto slotSize = slotType->getValueSize();
        auto slotAlignment = slotType->getValueAlignment();
        if(slotAlignment == 0)
        {
            memoryAlignment = 0;
            memorySize = 0;
            offset = 0;
        }
        else
        {
            if(isPacked_)
            {
                offset = memorySize;
                memorySize = offset + slotSize;
            }
            else
            {
                offset = alignedTo(memorySize, slotAlignment);
                memorySize = offset + slotSize;
                memoryAlignment = std::max(memoryAlignment, slotAlignment);
            }
        }
    }

    auto slotIndex = uint32_t(slotTypes.size());
    slotTypes.push_back(slotType);
    offsets.push_back(offset);
    return slotIndex;
}

void AggregateTypeSequentialLayout::finishGroup()
{
    if(memoryAlignment != 0)
        memorySize = alignedTo(memorySize, memoryAlignment);
}

TypePtr AggregateTypeSequentialLayout::getTypeForSlotAndOffset(int64_t slotIndex, int64_t slotOffset)
{
    (void)slotOffset;
    if(slotIndex < 0 || size_t(slotIndex) >= slotTypes.size())
        return nullptr;

    return slotTypes[slotIndex];
}

} // End of namespace Environment
} // End of namespace Sysmel