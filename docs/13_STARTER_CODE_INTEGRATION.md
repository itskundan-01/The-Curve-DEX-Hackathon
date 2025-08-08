# Starter Code Integration Strategy

This document outlines how to integrate with the provided C++ starter code in the `challenge/` directory.

## First Steps Upon Starting Challenge
1. **Immediately examine** the `challenge/` directory structure
2. **Read all provided interfaces** and understand the expected extension points
3. **Identify integration patterns** between our planned architecture and starter code
4. **Adapt our modules** to work with provided base classes/interfaces

## Expected Integration Points
Based on typical hackathon starter patterns, expect to find:
- Base classes for blockchain interaction
- Configuration templates or interfaces
- Sample trading logic to extend
- Build system setup (CMake/Makefile)
- Testing framework integration

## Adaptation Strategy
1. **Preserve starter interfaces**: Don't break provided APIs
2. **Extend rather than replace**: Build our modules as extensions of starter code
3. **Maintain compatibility**: Ensure our architecture works with provided patterns
4. **Quick validation**: Build and run provided examples immediately

## Risk Mitigation
- If starter code structure differs significantly from our docs:
  - Adapt our module boundaries to fit
  - Keep core logic (limit orders, policies) modular for easy transplantation
  - Document deviations in submission notes

## Documentation Updates
- Update README to reflect actual starter integration
- Note any deviations from planned architecture in final submission
- Ensure demo shows both starter functionality and our extensions

## Success Criteria
- All provided starter tests still pass
- Our extensions integrate seamlessly
- Functional demonstration shows end-to-end trading capability
- Code submission follows provided structure and requirements
