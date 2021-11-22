#include "sysmel/BootstrapEnvironment/TemporaryReferenceType.hpp"
#include "sysmel/BootstrapEnvironment/PointerType.hpp"
#include "sysmel/BootstrapEnvironment/RuntimeContext.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapMethod.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<TemporaryReferenceTypeValue> TemporaryReferenceTypeValueTypeRegistration;

TemporaryReferenceTypePtr TemporaryReferenceType::make(const TypePtr &baseType)
{
    return makeWithAddressSpace(baseType, internSymbol("generic"));
}

TemporaryReferenceTypePtr TemporaryReferenceType::makeWithAddressSpace(const TypePtr &baseType, const AnyValuePtr &addressSpace)
{
    auto &cache = RuntimeContext::getActive()->temporaryReferenceTypeCache;
    auto it = cache.find({baseType, addressSpace});
    if(it != cache.end())
        return it->second;

    auto result = std::make_shared<TemporaryReferenceType> ();
    result->setSupertypeAndImplicitMetaType(TemporaryReferenceTypeValue::__staticType__());
    result->baseType = baseType;
    result->addressSpace = addressSpace;
    cache.insert({{baseType, addressSpace}, result});
    result->addSpecializedInstanceMethods();
    return result;
}

bool TemporaryReferenceType::isReferenceLikeType() const
{
    return true;
}

bool TemporaryReferenceType::isTemporaryReferenceType() const
{
    return true;
}

ReferenceTypePtr TemporaryReferenceType::ref()
{
    return baseType->ref();
}

ReferenceTypePtr TemporaryReferenceType::refFor(const AnyValuePtr &newAddressSpace)
{
    return baseType->refFor(newAddressSpace);
}

PointerLikeTypePtr TemporaryReferenceType::tempRef()
{
    return shared_from_this();
}

PointerLikeTypePtr TemporaryReferenceType::tempRefFor(const AnyValuePtr &newAddressSpace)
{
    if(addressSpace == newAddressSpace)
        return shared_from_this();

    return baseType->tempRefFor(newAddressSpace);
}

std::string TemporaryReferenceType::printString() const
{
    if(hasGenericAddressSpace())
        return baseType->printString() + " tempRef";
    return "(" + baseType->printString() + " tempRefFor: " + addressSpace->printString();
}

SExpression TemporaryReferenceType::asSExpression() const
{
    return SExpressionList{{
        SExpressionIdentifier{{"tempRefType"}},
        baseType->asSExpression(),
        addressSpace->asSExpression()
    }};
}

PointerLikeTypeValuePtr TemporaryReferenceType::makeWithValue(const AnyValuePtr &value)
{
    auto temporaryReference = std::make_shared<TemporaryReferenceTypeValue> ();
    temporaryReference->type = shared_from_this();
    temporaryReference->baseValue = value;
    return temporaryReference;
}

void TemporaryReferenceType::addSpecializedInstanceMethods()
{
    auto pointerType = baseType->pointerFor(addressSpace);

    addMethodCategories(MethodCategories{
            {"accessing", {
                makeIntrinsicMethodBindingWithSignature<PointerLikeTypeValuePtr (TemporaryReferenceTypeValuePtr)> ("reference.to.pointer", "address", shared_from_this(), pointerType, {}, [=](const TemporaryReferenceTypeValuePtr &value) {
                    return pointerType->makeWithValue(value->baseValue);
                }, MethodFlags::Pure),
            }
        }
    });
}

bool TemporaryReferenceTypeValue::isTemporaryReferenceTypeValue() const
{
    return true;
}

bool TemporaryReferenceTypeValue::isReferenceLikeTypeValue() const
{
    return true;
}

TypePtr TemporaryReferenceTypeValue::getType() const
{
    return type;
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius