# Challenge Overview

Goal: Build a C++ trading agent that executes limit orders on Curve Finance via the Ethereum blockchain. Implement a limit order engine using the provided C++ starter code, with robust tests and a narrated demo.

## Constraints
- Language: C++ (C++17 or later)
- Duration: 24 hours from start
- Environment: GoCode browser-based VS Code, pre-configured; `challenge/` contains starter and submission rules
- Deliverables:
  - Functional agent executable (or CLI)
  - Source code with tests
  - Short screen recording with narration: design, walkthrough, live demo

## Primary Capabilities
- Read price/quote from Curve pool(s)
- Create limit orders with execution policies (IOC, FOK, GTC)
- Monitor market; when target met, execute swap on Curve
- Manage token approvals and gas strategy
- Handle failures (reverts, slippage, timeouts) and retries

## Non-Goals (for this hackathon)
- Full-blown order book or smart contract deployment
- Cross-chain operations
- Production-grade monitoring/alerting

## Success Criteria
- Clean C++ architecture with modular components
- Deterministic tests and a mockable Ethereum client
- Clear README and recording demonstrating end-to-end flow
- Adherence to provided starter interfaces and submission rules
