# Testing

## MockTransport

`MockTransport` is a mock implementation of the `ITransport` interface that simulates a SCPI transport without requiring actual hardware (USB or LAN).

### Features

- **No hardware dependency**: Tests can run without USB devices or network devices
- **Configurable responses**: Set predefined responses for query commands
- **Command logging**: Inspect all sent commands for assertion-based testing
- **Flexible test setup**: Control success/failure of send operations

### Usage in tests

```cpp
#include "mocktransport.h"

void testMyController() {
    MockTransport transport;
    transport.open();
    
    // Configure response for a query
    transport.setResponse("*IDN?", "Atten,GA1483,SN12345,1.0");
    
    // Pass &transport to a controller that accepts ITransport*
    MyController controller(&transport);
    
    // Test the controller behavior
    controller.queryDeviceId();
    
    // Verify commands were sent correctly
    QStringList commands = transport.sentCommands();
    QVERIFY(commands.contains("*IDN?"));
}
```

### Test helpers API

```cpp
MockTransport t;

// Connection management
t.open(0);              // Always succeeds; index is ignored
t.close();
t.isOpen();             // Returns current open state

// Command execution
t.sendScpiCommand(cmd);           // Logs cmd, returns m_sendResult
t.queryScpiCommand(cmd);          // Logs cmd, returns configured response

// Test setup
t.setResponse(query, response);   // Map query -> response
t.setSendResult(bool);            // Set return value for sendScpiCommand()

// Command inspection
t.sentCommands();                 // QStringList of all sent commands in order
t.clearSentCommands();            // Reset the log
```

## Running tests

### Build tests

```bash
cd build
cmake ..
cmake --build . --config Debug
```

### Run tests

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./test_mock_transport
```

### Run specific tests

```bash
ctest -R MockTransportTests --output-on-failure
```

## Integration: GrattenGa1483Controller

Currently `GrattenGa1483Controller` directly uses `LanInterface`, hardcoding the transport layer. To fully test it with `MockTransport`, refactor the controller to accept an `ITransport*` in its constructor (dependency injection):

```cpp
// Before (not testable with MockTransport)
GrattenGa1483Controller::GrattenGa1483Controller(QObject *parent)
    : AbstractScpiController(parent)
    , m_lanInterface(new LanInterface(this))
{
}

// After (testable)
GrattenGa1483Controller::GrattenGa1483Controller(ITransport *transport, QObject *parent)
    : AbstractScpiController(parent)
    , m_transport(transport)
{
}
```

This refactoring is planned for Phase 5 of the development roadmap.
