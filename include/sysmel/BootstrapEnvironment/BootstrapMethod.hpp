#ifndef SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_BOOTSTRAP_METHOD_HPP
#define SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_BOOTSTRAP_METHOD_HPP
#pragma once

#include "SpecificMethod.hpp"
#include "Wrappers.hpp"
#include "Error.hpp"
#include <type_traits>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

/**
 * I am the base interface for a type that is specifically defined by the bootstrap environment.
 */
class BootstrapMethodBase : public SubtypeOf<SpecificMethod, BootstrapMethodBase>
{
public:
    static constexpr char const __typeName__[] = "BootstrapMethod";
    
    virtual bool isBootstrapMethod() const override;

protected:
    AnyValuePtr selector;
};

template<typename MethodSignature, typename FT = void>
class BootstrapMethod;

template<typename ResultType, typename ReceiverType, typename... Args, typename FT>
class BootstrapMethod<ResultType (ReceiverType, Args...), FT> : public BootstrapMethodBase
{
public:
    typedef BootstrapMethod<ResultType (Args...), FT> SelfType;

    BootstrapMethod(const AnyValuePtr &initialSelector, FT initialFunctor)
        : functor(initialFunctor)
    {
        selector = initialSelector;
        signature = MethodSignature{
            wrapperTypeForReturning<ResultType> (),
            wrapperTypeFor<ReceiverType> (),
            {
                wrapperTypeFor<Args> ()...
            }
        };
    }

    AnyValuePtr runWithArgumentsIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver) override
    {
        (void)selector;
        
        if(signature.argumentTypes.size() != arguments.size())
            throw ArgumentsCountMismatch(signature.argumentTypes.size(), arguments.size());

        return doRunWithArgumentsIn(std::make_index_sequence<sizeof...(Args)> {}, arguments, receiver);
    }
private:

    template<typename T, T... ArgIndices>
    AnyValuePtr doRunWithArgumentsIn(std::integer_sequence<T, ArgIndices...> indicesSeq, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver)
    {
        (void)indicesSeq;

        if constexpr(std::is_same<ResultType, void>::value)
        {
            functor(unwrapValue<ReceiverType> (receiver), unwrapValue<Args> (arguments[ArgIndices])...);
            return getVoidConstant();
        }
        else
        {
            return wrapValue(functor(unwrapValue<ReceiverType> (receiver), unwrapValue<Args> (arguments[ArgIndices])...));
        }
    }

    FT functor;
};


template<typename ResultType, typename ReceiverType, typename... Args>
class BootstrapMethod<ResultType (ReceiverType::*) (Args...)> : public BootstrapMethodBase
{
public:
    typedef BootstrapMethod<ResultType (ReceiverType::*) (Args...)> SelfType;
    typedef ResultType (ReceiverType::*MemberFunctionPointerType) (Args...);

    BootstrapMethod(const AnyValuePtr &initialSelector, MemberFunctionPointerType initialMemberFunctionPointer)
        : memberFunctionPointer(initialMemberFunctionPointer)
    {
        selector = initialSelector;
        signature = MethodSignature{
            wrapperTypeForReturning<ResultType> (),
            wrapperTypeFor<ReceiverType*> (),
            {
                wrapperTypeFor<Args> ()...
            }
        };
    }

    AnyValuePtr runWithArgumentsIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver) override
    {
        (void)selector;
        
        if(signature.argumentTypes.size() != arguments.size())
            throw ArgumentsCountMismatch(signature.argumentTypes.size(), arguments.size());

        return doRunWithArgumentsIn(std::make_index_sequence<sizeof...(Args)> {}, arguments, receiver);
    }
private:
    template<typename T, T... ArgIndices>
    AnyValuePtr doRunWithArgumentsIn(std::integer_sequence<T, ArgIndices...> indicesSeq, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver)
    {
        (void)indicesSeq;

        auto unwrappedReceiver = unwrapValue<ReceiverType*> (receiver);
        if constexpr(std::is_same<ResultType, void>::value)
        {
            (unwrappedReceiver->*memberFunctionPointer)(unwrapValue<Args> (arguments[ArgIndices])...);
            return getVoidConstant();
        }
        else
        {
            return wrapValue((unwrappedReceiver->*memberFunctionPointer)(unwrapValue<Args> (arguments[ArgIndices])...));
        }
    }

    MemberFunctionPointerType memberFunctionPointer;
};

template<typename ResultType, typename ReceiverType, typename... Args>
class BootstrapMethod<ResultType (ReceiverType::*) (Args...) const> : public BootstrapMethodBase
{
public:
    typedef BootstrapMethod<ResultType (ReceiverType::*) (Args...)> SelfType;
    typedef ResultType (ReceiverType::*MemberFunctionPointerType) (Args...) const;

    BootstrapMethod(const AnyValuePtr &initialSelector, MemberFunctionPointerType initialMemberFunctionPointer)
        : memberFunctionPointer(initialMemberFunctionPointer)
    {
        selector = initialSelector;
        signature = MethodSignature{
            wrapperTypeForReturning<ResultType> (),
            wrapperTypeFor<const ReceiverType*> (),
            {
                wrapperTypeFor<Args> ()...
            }
        };
    }

    AnyValuePtr runWithArgumentsIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver) override
    {
        (void)selector;
        
        if(signature.argumentTypes.size() != arguments.size())
            throw ArgumentsCountMismatch(signature.argumentTypes.size(), arguments.size());

        return doRunWithArgumentsIn(std::make_index_sequence<sizeof...(Args)> {}, arguments, receiver);
    }

private:
    template<typename T, T... ArgIndices>
    AnyValuePtr doRunWithArgumentsIn(std::integer_sequence<T, ArgIndices...> indicesSeq, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver)
    {
        (void)indicesSeq;

        auto unwrappedReceiver = unwrapValue<ReceiverType*> (receiver);
        if constexpr(std::is_same<ResultType, void>::value)
        {
            (unwrappedReceiver->*memberFunctionPointer)(unwrapValue<Args> (arguments[ArgIndices])...);
            return getVoidConstant();
        }
        else
        {
            return wrapValue((unwrappedReceiver->*memberFunctionPointer)(unwrapValue<Args> (arguments[ArgIndices])...));
        }
    }

    MemberFunctionPointerType memberFunctionPointer;
};


template<typename MethodSignature, typename FT>
MethodBinding makeMethodBinding(const std::string &selector, FT &&functor)
{
    auto selectorSymbol = internSymbol(selector);
    auto bootstrapMethod = std::make_shared<BootstrapMethod<MethodSignature, FT> > (selectorSymbol, std::forward<FT> (functor));
    return MethodBinding{selectorSymbol, bootstrapMethod};
}

template<typename FT>
MethodBinding makeMethodBinding(const std::string &selector, FT &&functor)
{
    auto selectorSymbol = internSymbol(selector);
    auto bootstrapMethod = std::make_shared<BootstrapMethod<FT> > (selectorSymbol, std::forward<FT> (functor));
    return MethodBinding{selectorSymbol, bootstrapMethod};
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius

#endif //SYSMEL_COMPILER_BOOTSTRAP_ENVIRONMENT_BOOTSTRAP_TYPE_HPP