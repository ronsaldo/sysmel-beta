#include "sysmel/BootstrapEnvironment/PatternMatchingMethod.hpp"
#include "sysmel/BootstrapEnvironment/BootstrapTypeRegistration.hpp"
#include "sysmel/BootstrapEnvironment/Error.hpp"
#include <limits>

namespace SysmelMoebius
{
namespace BootstrapEnvironment
{

static BootstrapTypeRegistration<PatternMatchingMethod> patternMatchingMethodTypeRegistration;

PatternMatchingMethod::PatternMatchingMethod(const AnyValuePtr &initialName)
{
    name = initialName;
}

bool PatternMatchingMethod::isPatternMatchingMethod() const
{
    return true;
}

void PatternMatchingMethod::addPattern(const MethodPtr &newPattern)
{
    // TODO: Check for a clash!!
    patterns.push_back(newPattern);
}

AnyValuePtr PatternMatchingMethod::runWithArgumentsIn(const AnyValuePtr &selector, const std::vector<AnyValuePtr> &arguments, const AnyValuePtr &receiver)
{
    std::vector<MethodPtr> matchingCandidates;
    PatternMatchingRank bestRank = std::numeric_limits<PatternMatchingRank>::max();

    for(const auto &pattern : patterns)
    {
        auto result = pattern->matchPatternForRunWithIn(selector, arguments, receiver);
        if(!result.matchingMethod)
            continue;

        if(result.matchingRank < bestRank)
        {
            matchingCandidates.clear();
            matchingCandidates.push_back(result.matchingMethod);
            bestRank = result.matchingRank;
        }
        else if(result.matchingRank == bestRank)
        {
            matchingCandidates.push_back(result.matchingMethod);
        }
    }

    if(matchingCandidates.empty())
    {
        throw NotMatchingPatternFound();
    }
    else if(matchingCandidates.size() > 1)
    {
        throw AmbiguousMatchingPatternsFound();
    }

    return matchingCandidates.front()->runWithArgumentsIn(selector, arguments, receiver);
}

} // End of namespace BootstrapEnvironment
} // End of namespace SysmelMoebius