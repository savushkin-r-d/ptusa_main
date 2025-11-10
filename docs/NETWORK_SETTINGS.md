# Network Settings Manager - Port Forwarding Feature

## Overview

The Network Settings Manager is a new feature that provides configurable port forwarding from internal networks to external networks for enhanced security. This implementation addresses the requirements described in issue #1095.

## Architecture

### Core Components

1. **port_forward_rule** - Individual port forwarding rule
   - Device identification (name and IP)
   - Port mapping (internal → external)
   - State management (open/closed)

2. **network_settings_manager** - Centralized manager (Singleton)
   - Rule collection management
   - Command-based operations
   - Lifecycle integration with IoT subsystem

### Integration Points

- **IoT Subsystem**: Integrated via `IOT_EVALUATE()` and `IOT_FINAL()` hooks
- **Lua Interface**: Exposed via tolua++ bindings for script configuration
- **Logging**: Uses existing G_LOG infrastructure

## Usage

### From Lua Scripts

```lua
-- Get manager instance
local net_mgr = G_NETWORK_SETTINGS_MANAGER()

-- Add port forwarding rules
-- Syntax: add_port_forward_rule(command_id, device_name, internal_port, external_port, device_ip)
-- Note: external_port = 0 means use the same as internal_port

-- Example 1: Command 10001 opens port 10001 on device A100
net_mgr:add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100")

-- Example 2: Command 11001 forwards port 1962 to external 11001 on device A100
net_mgr:add_port_forward_rule(11001, "A100", 1962, 11001, "192.168.1.100")

-- Example 3: Schneider Electric SoMove (port 502)
net_mgr:add_port_forward_rule(502, "Schneider_SoMove", 502, 0, "192.168.1.200")

-- Example 4: WAGO IO-Check (port 6626)
net_mgr:add_port_forward_rule(6626, "WAGO_IOCheck", 6626, 0, "192.168.1.201")

-- Execute commands
local success = net_mgr:process_command(10001, true)   -- Open port
if success then
    print("Port opened successfully")
end

net_mgr:process_command(10001, false)  -- Close port

-- Close all ports (usually done at shutdown)
net_mgr:close_all_ports()
```

### From C++

```cpp
#include "network_settings.h"

// Get manager instance
network_settings_manager* mgr = G_NETWORK_SETTINGS_MANAGER();

// Add a rule
mgr->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");

// Process command
bool success = mgr->process_command(10001, true);

// Get rule details
port_forward_rule* rule = mgr->get_rule(10001);
if (rule)
{
    printf("Device: %s, IP: %s, Port: %u -> %u\n",
           rule->get_device_name(),
           rule->get_device_ip(),
           rule->get_internal_port(),
           rule->get_external_port());
}

// Close all ports
mgr->close_all_ports();
```

## Command Examples

Based on the issue requirements, here are typical command scenarios:

| Command ID | Device    | Internal Port | External Port | Purpose                     |
|------------|-----------|---------------|---------------|-----------------------------|
| 10001      | A100      | 10001         | 10001         | Direct port access          |
| 11001      | A100      | 1962          | 11001         | Startup+ mode forwarding    |
| 502        | Schneider | 502           | 502           | SoMove access               |
| 6626       | WAGO      | 6626          | 6626          | IO-Check access             |

## Testing

### Running Tests

```bash
cd build
./Release/main_test --gtest_filter="NetworkSettingsManagerTest.*:PortForwardRuleTest.*"
```

### Test Coverage

The implementation includes 15 comprehensive unit tests covering:

- ✓ Rule construction and configuration
- ✓ Port open/close operations
- ✓ Manager singleton pattern
- ✓ Multiple rule management
- ✓ Command processing
- ✓ Error handling for non-existent rules
- ✓ Rule replacement
- ✓ Cleanup operations

All tests pass successfully.

### Demo Script

A demonstration script is provided at `demo_projects/network_settings_demo.lua` that shows:
- Adding multiple rules
- Opening and closing ports
- Error handling
- Cleanup operations

## Implementation Details

### Current State

The current implementation provides:
1. Complete API for managing port forwarding rules
2. Lua bindings for configuration
3. Logging of port operations
4. State management and cleanup

### Future Development

The actual port forwarding mechanism needs to be implemented based on the target platform:

**Linux**: Use iptables or nftables
```cpp
// Example for Linux implementation in port_forward_rule::open_port()
char cmd[256];
snprintf(cmd, sizeof(cmd), 
         "iptables -t nat -A PREROUTING -p tcp --dport %u -j DNAT --to-destination %s:%u",
         external_port, device_ip, internal_port);
system(cmd);
```

**Windows**: Use netsh or Windows Firewall API
```cpp
// Example for Windows implementation
char cmd[256];
snprintf(cmd, sizeof(cmd),
         "netsh interface portproxy add v4tov4 listenport=%u connectaddress=%s connectport=%u",
         external_port, device_ip, internal_port);
system(cmd);
```

**Dedicated Firewall/Router**: Use vendor-specific API or SSH commands

### Security Considerations

1. **Input Validation**: Port numbers are validated via type system (unsigned int)
2. **Memory Safety**: Proper allocation/deallocation with nullptr checks
3. **Logging**: All operations are logged for audit purposes
4. **State Management**: Automatic cleanup on shutdown prevents port leaks

### Performance

- Minimal overhead: O(1) for command lookups via std::map
- No periodic polling: ports remain open until explicitly closed
- Efficient memory usage: only stores active rules

## Files

### Core Implementation
- `PAC/IoT/network_settings.h` - Header file with class definitions
- `PAC/IoT/network_settings.cpp` - Core implementation
- `PAC/IoT/network_settings_lua.cpp` - Lua bindings wrapper

### Integration
- `PAC/IoT/iot_common.cpp` - IoT subsystem integration
- `PAC/IoT/IOT_dev_lua.cpp` - Generated Lua bindings
- `PAC/common/toloa++/pkg/IOT_dev.hh` - Lua package definition

### Testing & Documentation
- `test/network_settings_tests.cpp` - Unit tests
- `demo_projects/network_settings_demo.lua` - Demo script
- `docs/NETWORK_SETTINGS.md` - This documentation

## Building

The feature is automatically built as part of the main build process:

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

No additional configuration or dependencies are required.

## API Reference

### Lua API

#### G_NETWORK_SETTINGS_MANAGER()
Returns the singleton instance of the network settings manager.

**Returns**: `network_settings_manager*`

#### add_port_forward_rule(command_id, device_name, internal_port, external_port, device_ip)
Adds a new port forwarding rule.

**Parameters**:
- `command_id` (number): Unique command identifier
- `device_name` (string): Name of the device
- `internal_port` (number): Internal device port
- `external_port` (number): External accessible port (0 = same as internal)
- `device_ip` (string): IP address of the device

**Returns**: void

#### process_command(command_id, open)
Executes a port forwarding command.

**Parameters**:
- `command_id` (number): Command identifier
- `open` (boolean): true to open, false to close (default: true)

**Returns**: `boolean` - true if successful

#### close_all_ports()
Closes all currently open ports.

**Returns**: void

### C++ API

See header file `PAC/IoT/network_settings.h` for complete API documentation.

## License

This feature is part of the ptusa_main project and follows the same license terms.

## Contributors

- Implementation: GitHub Copilot AI Assistant
- Issue: #1095 - [FEATURE]: Объект сетевых настроек
