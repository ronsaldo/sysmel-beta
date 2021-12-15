#ifndef SYSMEL_ENVIRONMENT_SSA_LLVM_VALUE_VISITOR_HPP
#define SYSMEL_ENVIRONMENT_SSA_LLVM_VALUE_VISITOR_HPP
#pragma once

#include "../../Environment/SSAValueVisitor.hpp"
#include "SSALLVMCodeGenerationBackend.hpp"

namespace Sysmel
{
namespace LLVM
{

using namespace Environment;

SYSMEL_DECLARE_BOOTSTRAP_CLASS(SSALLVMValueVisitor)
SYSMEL_DECLARE_BOOTSTRAP_CLASS(SSALLVMCodeGenerationBackend)

/**
 * I wrap a llvm type in an AnyValue
 */
class SSALLVMValue : public SubtypeOf<CompilerObject, SSALLVMValue>
{
public:
    static constexpr char const __typeName__[] = "SSALLVMValue";

    llvm::Value *value = nullptr;
};

AnyValuePtr wrapLLVMValue(llvm::Value *value);

struct IntrinsicGenerationContext
{
    SSALLVMValueVisitor *self = nullptr;
    llvm::IRBuilder<> *builder = nullptr;
    llvm::Type *resultType = nullptr;
    std::vector<llvm::Value*> arguments;
    SSAValuePtrList originalArguments;
    std::string intrinsicName;
};

/**
 * I visit SSA values for converting them into llvm SSA values.
 */
class SSALLVMValueVisitor : public SubtypeOf<SSAValueVisitor, SSALLVMValueVisitor>
{
public:
    static constexpr char const __typeName__[] = "SSALLVMValueVisitor";

    llvm::Value *visitValueForLLVM(const SSAValuePtr &value);

    llvm::Value *translateValue(const SSAValuePtr &value);
    virtual AnyValuePtr visitFunction(const SSAFunctionPtr &value) override;
    virtual AnyValuePtr visitGlobalVariable(const SSAGlobalVariablePtr &value) override;
    virtual AnyValuePtr visitConstantLiteralValue(const SSAConstantLiteralValuePtr &value) override;
    virtual AnyValuePtr visitTypeProgramEntity(const SSATypeProgramEntityPtr &value) override;

    virtual AnyValuePtr visitCallInstruction(const SSACallInstructionPtr &instruction) override;
    virtual AnyValuePtr visitDoWithCleanupInstruction(const SSADoWithCleanupInstructionPtr &instruction) override;
    virtual AnyValuePtr visitDoWhileInstruction(const SSADoWhileInstructionPtr &instruction) override;
    virtual AnyValuePtr visitIfInstruction(const SSAIfInstructionPtr &instruction) override;
    virtual AnyValuePtr visitLocalVariableInstruction(const SSALocalVariableInstructionPtr &instruction) override;
    virtual AnyValuePtr visitReturnFromFunctionInstruction(const SSAReturnFromFunctionInstructionPtr &instruction) override;
    virtual AnyValuePtr visitReturnFromRegionInstruction(const SSAReturnFromRegionInstructionPtr &instruction) override;
    virtual AnyValuePtr visitStoreInstruction(const SSAStoreInstructionPtr &instruction) override;
    virtual AnyValuePtr visitUnreachableInstruction(const SSAUnreachableInstructionPtr &instruction) override;
    virtual AnyValuePtr visitWhileInstruction(const SSAWhileInstructionPtr &instruction) override;

    virtual AnyValuePtr visitUpcastInstruction(const SSAUpcastInstructionPtr &instruction) override;
    virtual AnyValuePtr visitDowncastInstruction(const SSADowncastInstructionPtr &instruction) override;
    virtual AnyValuePtr visitBitcastInstruction(const SSABitcastInstructionPtr &instruction) override;

    SSALLVMCodeGenerationBackend *backend = nullptr;

protected:
    static std::unordered_map<std::string, std::function<llvm::Value* (const IntrinsicGenerationContext&)>> intrinsicGenerators;

    void translateMainCodeRegion(const SSACodeRegionPtr &codeRegion);
    llvm::Value *translateCodeRegionWithArguments(const SSACodeRegionPtr &codeRegion, const std::vector<llvm::Value*> &arguments);

    void translateBasicBlockInto(const SSABasicBlockPtr &sourceBasicBlock, llvm::BasicBlock *targetBasicBlock);
    void translateInstruction(const SSAInstructionPtr &instruction);
    llvm::Value *translateIntrinsicCall(const std::string &intrinsicName, const SSACallInstructionPtr &instruction);
    llvm::Value *translateCall(const SSACallInstructionPtr &instruction);
    void returnFromFunction(const SSAValuePtr &resultValue);
    void emitCleanUpsForReturning();
    llvm::Value *makeVoidValue();
    llvm::Value *coerceBooleanIntoI1(llvm::Value *value);
    llvm::Value *coerceI1IntoBoolean8(llvm::Value *value);
    llvm::Value *simplifyDegeneratePhi(llvm::PHINode *phi);
    bool isLastTerminator() const;

    llvm::Function *currentFunction = nullptr;
    SSACodeRegionPtr currentCodeRegion;
    llvm::BasicBlock *currentCodeRegionReturnBlock = nullptr;
    llvm::PHINode *currentCodeRegionResultPHI = nullptr;
    
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::IRBuilder<>> allocaBuilder;
    std::unordered_map<SSAValuePtr, llvm::Value*> localTranslatedValueMap;
};

} // End of namespace LLVM
} // End of namespace Sysmel

#endif //SYSMEL_ENVIRONMENT_SSA_LLVM_VALUE_VISITOR_HPP