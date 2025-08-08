# GoCode Environment Notes

Reference notes for working inside the GoQuant GoCode browser IDE.

## Workspace
- After starting, open `challenge/` directory; it contains starter C++ code and submission rules.
- Our docs live alongside; adapt paths as needed.
- **CRITICAL**: Review starter code interfaces and adapt our architecture to extend them properly

## Build Tooling
- Expect CMake and a recent GCC/Clang
- If package install is restricted, vendor lightweight headers (e.g., nlohmann/json)
- Verify starter code build requirements and dependency management

## Networking
- An RPC endpoint should be provided or configurable; verify early
- If outbound network is blocked, switch to full-mock mode and record demo accordingly
- Test network connectivity to Ethereum RPC endpoints immediately

## Filesystem & Recording
- Ensure recording tool is accessible (browser or OS)
- Keep artifacts in `challenge/` unless submission requires different layout
- **Recording requirement**: 5-8 minute screen recording with audio narration showing functional application

## Start/Stop
- The 24-hour timer is irreversible; prep configs and docs before pressing Start
- Ensure you can demonstrate live trading functionality within the time limit
