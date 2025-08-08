# Pre-Challenge Development Plan (August 8th)

## Confirmed from GoQuant Email:
- Challenge unlocks: Saturday, August 9th
- 24-hour timer: Starts when YOU click "Start Challenge"
- Environment: Pre-configured VS Code with `challenge/` directory
- Starter code: C++ sample provided in workspace
- Deadline: Monday morning (lockout for all participants)

## Today's Development Goals (August 8th):

### Phase 1: Local Environment Setup (2 hours)
```bash
# Set up local C++ development environment
brew install cmake
brew install nlohmann-json  
brew install openssl
brew install curl

# Create project structure
mkdir -p curve-trading-agent/{src,include,tests,tools}
cd curve-trading-agent
```

### Phase 2: Core Logic Implementation (4-6 hours)
Build these components that will be 100% portable:

#### 1. Order Management System
- `Order.hpp/cpp` - Order data structure and validation
- `OrderPolicies.hpp/cpp` - GTC, IOC, FOK policy implementations
- `LimitOrderEngine.hpp/cpp` - Core order management logic

#### 2. Business Logic
- Price calculation and normalization utilities
- Slippage calculation and validation
- Order state machine transitions
- Configuration parsing framework

#### 3. Clean Interfaces
- `IEthereumClient.hpp` - Abstract blockchain interface
- `ICurveService.hpp` - Abstract Curve protocol interface  
- `IERC20Service.hpp` - Abstract token interface
- `IOrderStorage.hpp` - Abstract persistence interface

#### 4. Unit Tests
- Test order creation and validation
- Test policy execution logic
- Test price calculations and slippage
- Test state transitions

### Phase 3: Mock Implementations (2 hours)
- `MockEthereumClient` for testing
- `MockCurveService` with deterministic quotes
- Sample test data and scenarios

## What NOT to Build Today:
- HTTP client implementations (unknown library dependencies)
- Crypto signing (unknown library availability)
- File I/O specifics (unknown filesystem constraints)
- Main entry point (will adapt to provided starter)

## Time Investment vs. Savings:
- **Investment today**: 6-8 hours of development
- **Savings tomorrow**: 6-8 hours during hackathon
- **Net benefit**: Enter hackathon with 60-70% complete solution
- **Risk reduction**: Debug complex logic without time pressure

## Tomorrow's Strategy (August 9th):

### Before Starting Timer:
1. Log into GoCode and examine `challenge/` directory
2. Read all provided starter code and interfaces
3. Understand submission requirements
4. Plan integration approach (30-60 minutes of analysis)

### After Starting Timer:
1. **Hours 0-2**: Integrate local code with starter interfaces
2. **Hours 2-4**: Implement blockchain and Curve services  
3. **Hours 4-16**: Integration, testing, and debugging
4. **Hours 16-20**: End-to-end testing and edge cases
5. **Hours 20-24**: Recording, documentation, and submission

## Key Advantages of This Approach:
- **Confidence**: You'll know your core logic works before time pressure
- **Speed**: Focus hackathon time on integration, not algorithm development
- **Quality**: More time for testing and polish during hackathon
- **Backup plan**: Even if integration is challenging, you have working components

## Files to Create Today:
```
src/core/
├── Order.hpp
├── Order.cpp
├── OrderPolicies.hpp  
├── OrderPolicies.cpp
├── LimitOrderEngine.hpp
└── LimitOrderEngine.cpp

src/interfaces/
├── IEthereumClient.hpp
├── ICurveService.hpp
├── IERC20Service.hpp
└── IOrderStorage.hpp

src/utils/
├── Config.hpp
├── Config.cpp
├── PriceUtils.hpp
├── PriceUtils.cpp
├── Logger.hpp
└── Logger.cpp

tests/
├── test_order.cpp
├── test_policies.cpp
├── test_engine.cpp
└── test_utils.cpp

mocks/
├── MockEthereumClient.hpp
├── MockCurveService.hpp
└── test_data.hpp
```

## Success Metrics for Today:
- [ ] Order creation and validation working
- [ ] All three policies (GTC, IOC, FOK) implemented and tested
- [ ] Price calculation utilities tested with various decimal combinations
- [ ] Mock-based integration tests passing
- [ ] Clean interfaces defined for tomorrow's integration
- [ ] Unit test coverage >80% for core logic

## Tomorrow Morning Checklist:
- [ ] All local code committed and backed up
- [ ] Integration strategy documented
- [ ] GoCode environment tested and accessible
- [ ] Recording setup verified
- [ ] Ready to click "Start Challenge" with confidence
