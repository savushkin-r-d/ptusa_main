# Alarm Management

## Overview

The alarm management system in ptusa_main is implemented in accordance with modern SCADA practices and the ISA-18.2 standard for industrial alarm management.

## Key Features

### 1. Timestamp Tracking

The system automatically records timestamps for key events in the alarm lifecycle:

- **alarm_time** - time when the alarm occurred
- **ack_time** - time when the alarm was acknowledged by the operator
- **return_time** - time when conditions returned to normal

These timestamps enable:
- Analysis of operator response times
- Determination of alarm duration
- Statistical analysis of alarms

### 2. Occurrence Counter

The **occurrence_count** field tracks how many times a particular alarm has occurred since system startup.

Benefits:
- Identification of recurring issues
- Prioritization of maintenance
- Equipment reliability analysis

### 3. Alarm Shelving

The shelving feature allows temporary suppression of alarm notifications for a specified period.

#### Usage:

```cpp
// Shelve an alarm for 3600 seconds (1 hour).
G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_SHELVE, 
                            device_type, 
                            device_number, 
                            3600 );

// Unshelve before expiration.
G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_UNSHELVE, 
                            device_type, 
                            device_number, 
                            alarm_number );
```

#### Applications:
- Scheduled maintenance
- Known temporary issues
- System configuration and commissioning

**Important**: Shelving is automatically removed after the specified duration expires.

### 4. Alarm States

The system supports the following alarm states:

| State | Description |
|-------|-------------|
| AS_NORMAL | Normal state, no alarm |
| AS_ALARM | Active alarm requiring attention |
| AS_ACCEPT | Alarm acknowledged by operator but still active |
| AS_RETURN | Conditions returned to normal, awaiting acknowledgment |

### 5. Alarm Management Commands

| Command | Constant | Description |
|---------|----------|-------------|
| Acknowledge | C_CMD_ACCEPT | Operator acknowledgment of alarm |
| Suppress | C_CMD_SUPPRESS | Permanent alarm suppression |
| Unsuppress | C_CMD_UNSET_SUPPRESS | Cancel permanent suppression |
| Shelve | C_CMD_SHELVE | Temporary suppression for N seconds |
| Unshelve | C_CMD_UNSHELVE | Cancel temporary suppression |

## Lua Integration

All alarm information, including new fields, is automatically transmitted to the server in Lua format:

```lua
{
    description = "V1 - Feedback error",
    priority = 250,
    state = 1,  -- AS_ALARM
    type = 4,   -- AT_SPECIAL
    group = "alarm",
    id_n = 1,
    id_object_alarm_number = -1,
    id_type = 1,
    suppress = false,
    alarm_time = 1706706000,
    occurrence_count = 3,
    shelved = false
}
```

## Usage Examples

### Example 1: Acknowledging an Alarm

```cpp
// Get alarm list and acknowledge the first one.
G_ERRORS_MANAGER->evaluate();

if ( G_ERRORS_MANAGER->get_errors_id() > 0 )
    {
    // Acknowledge alarm for device type 7, number 0, alarm number 1.
    G_ERRORS_MANAGER->set_cmd( base_error::C_CMD_ACCEPT, 7, 0, 1 );
    }
```

### Example 2: Shelving Before Maintenance

```cpp
// Valve V1 maintenance scheduled for 2 hours.
const int TWO_HOURS_IN_SECONDS = 7200;

G_ERRORS_MANAGER->set_cmd( 
    base_error::C_CMD_SHELVE, 
    device::DT_V,    // Device type - valve.
    1,               // Device number.
    TWO_HOURS_IN_SECONDS 
);

// ... perform maintenance ...

// Can unshelve early after completion.
G_ERRORS_MANAGER->set_cmd( 
    base_error::C_CMD_UNSHELVE, 
    device::DT_V, 
    1, 
    1 
);
```

### Example 3: Analyzing Alarm History

Timestamps enable calculation of important metrics:

```cpp
// Alarm duration (if already resolved).
time_t alarm_duration = return_time - alarm_time;

// Operator response time.
time_t response_time = ack_time - alarm_time;

// This data can be used for:
// - Operator KPIs
// - Efficiency analysis
// - Management reporting
```

## Best Practices

1. **Timely Acknowledgment**: Operators should acknowledge alarms promptly for accurate statistics.

2. **Limited Shelving**: Use this feature only when truly necessary and document the reasons.

3. **Monitor Occurrence Counter**: Regularly check devices with high occurrence counts for preventive maintenance.

4. **Analyze Timing Metrics**: Use timestamps to improve processes and operator training.

## Technical Details

### base_error Structure

```cpp
class base_error
    {
    protected:
        unsigned char error_state;    // Error state.
        time_t alarm_time;            // Alarm occurrence time.
        time_t ack_time;              // Alarm acknowledgment time.
        time_t return_time;           // Return to normal time.
        u_int_4 occurrence_count;     // Alarm occurrence count.
        time_t shelve_time;           // Shelving start time.
        u_int_4 shelve_duration;      // Shelving duration, seconds.
    };
```

### Data Access API

```cpp
// Get timestamps.
time_t get_alarm_time() const;
time_t get_ack_time() const;
time_t get_return_time() const;

// Get occurrence counter.
u_int_4 get_occurrence_count() const;

// Check shelving status.
bool is_shelved() const;

// Manage shelving.
void shelve_alarm( u_int_4 duration );
void unshelve_alarm();
```

## Compatibility

All new features are fully compatible with existing code. Legacy projects will continue to work without modifications, and new capabilities can be adopted as needed.

## See Also

- [Testing](../test/readme.md) - Information about alarm management feature testing
- [ISA-18.2 Standard](https://www.isa.org/standards-and-publications/isa-standards/isa-standards-committees/isa18) - Alarm management standard
- g_errors.h - Header file with definitions
- g_errors.cpp - Alarm management implementation
