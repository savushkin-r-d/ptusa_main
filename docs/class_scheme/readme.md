# General class scheme #

```mermaid
flowchart BT
    device[device] --> i_DO_AO_device[i_DO_AO_device]
    device --> par_device[par_device]
    i_DO_device[i_DO_device] --> i_DI_device[i_DI_device]
    i_AO_device[i_AO_device] --> i_AI_device[i_AI_device]
    i_DI_device --> i_cmd_device[i_cmd_device]
    i_AI_device --> i_cmd_device
    i_DO_AO_device --> i_AO_device
    i_DO_AO_device --> i_DO_device
    digital_io_device[digital_io_device] --> device
    digital_io_device --> io_device[io_device]
    analog_io_device[analog_io_device] --> device
    analog_io_device --> io_device
    i_motor[i_motor] --> device
    signal_column[signal_column] --> device
    signal_column --> io_device
    virtual_wages[virtual_wages] --> device
    virtual_device[virtual_device] --> device
    threshold_regulator[threshold_regulator] --> device
    watchdog[watchdog] --> device
```

## Class DO1 scheme ##

```mermaid
flowchart BT
    DO1 --> digital_io_device
    DO_signal --> DO1
    lamp --> DO1
    siren --> DO1
```

## Class DI1 scheme ##

```mermaid
flowchart BT
    DI1 --> digital_io_device
    level_s[level_s] --> DI1
    flow_s[flow_s] --> DI1
    state_s[state_s] --> DI1
    state_s_inverse[state_s_inverse] --> DI1
    diff_pressure[diff_pressure] --> DI1
    temperature_signal[temperature_signal] --> DI1
    DI_signal[DI_signal] --> DI1
    button[button] --> DI1
```

## Class i_wages scheme ##

```mermaid
flowchart BT
    virtual_wages[virtual_wages] --> i_wages[i_wages]
    wages_RS232[wages_RS232] --> i_wages
    wages_eth[wages_eth] --> i_wages
    wages_pxc_axl[wages_pxc_axl] --> i_wages
    wages[wages] --> i_wages
    dev_stub[dev_stub] --> i_wages
```

## Class analog_io_device scheme ##

```mermaid
flowchart BT
    AO1[AO1] --> analog_io_device[analog_io_device]
    AI1[AI1] --> analog_io_device
    pressure_e_iolink[pressure_e_iolink] --> analog_io_device
    circuit_breaker[circuit_breaker] --> analog_io_device
    concentration_e_iolink[concentration_e_iolink] --> analog_io_device
    wages_RS232[wages_RS232] --> analog_io_device
    wages_eth[wages_eth] --> analog_io_device
    wages_pxc_axl[wages_pxc_axl] --> analog_io_device
    wages[wages] --> analog_io_device
    level_s_iolink[level_s_iolink] --> analog_io_device
    power_unit[power_unit] --> analog_io_device
```

## Class valve scheme ##

```mermaid
flowchart BT
    valve_DO2_DI2_bistable[valve_DO2_DI2_bistable] --> valve[valve]
    virtual_valve[virtual_valve] --> valve
    valve_DO1[valve_DO1] --> valve
    valve_DO2[valve_DO2] --> valve
    valve_DO1_DI1_off[valve_DO1_DI1_off] --> valve
    valve_DO1_DI1_on[valve_DO1_DI1_on] --> valve
    valve_DO1_DI2[valve_DO1_DI2] --> valve
    valve_DO2_DI2[valve_DO2_DI2] --> valve
    valve_mix_proof[valve_mix_proof] --> valve
    valve_AS[valve_AS] --> valve
    valve_AS_mix_proof[valve_AS_mix_proof] --> valve_AS
    valve_AS_DO1_DI2[valve_AS_DO1_DI2] --> valve_AS
    valve_bottom_mix_proof[valve_bottom_mix_proof] --> valve
    valve_mini_flushing[valve_mini_flushing] --> valve
    valve_iolink_mix_proof[valve_iolink_mix_proof] --> valve
    valve_iolink_shut_off_thinktop[valve_iolink_shut_off_thinktop] --> valve
    valve_iolink_shut_off_sorio[valve_iolink_shut_off_sorio] --> valve
    valve_iolink_gea_tvis_a15[valve_iolink_gea_tvis_a15] --> valve
    valve_iolink_gea_tvis_a15_ss[valve_iolink_gea_tvis_a15_ss] --> valve_iolink_gea_tvis_a15
    valve_iolink_gea_tvis_a15_ds[valve_iolink_gea_tvis_a15_ds] --> valve_iolink_gea_tvis_a15
    valve_iol_terminal[valve_iol_terminal] --> valve
    valve_iol_terminal_mixproof_DO3[valve_iol_terminal_mixproof_DO3] --> valve
    valve_iol_terminal_mixproof_DO3_DI2[valve_iol_terminal_mixproof_DO3_DI2] --> valve
    dev_stub[dev_stub] --> valve
```

## Class i_mix_proof scheme ##

```mermaid
flowchart BT
    valve_mix_proof[valve_mix_proof] --> i_mix_proof[i_mix_proof]
    valve_AS_mix_proof[valve_AS_mix_proof] --> i_mix_proof
    valve_bottom_mix_proof[valve_bottom_mix_proof] --> i_mix_proof
    valve_mini_flushing[valve_mini_flushing] --> i_mix_proof
    valve_iolink_mix_proof[valve_iolink_mix_proof] --> i_mix_proof
    valve_iolink_gea_tvis_a15_ds[valve_iolink_gea_tvis_a15_ds] --> i_mix_proof
    valve_iol_terminal_mixproof_DO3[valve_iol_terminal_mixproof_DO3] --> i_mix_proof
    valve_iol_terminal_mixproof_DO3_DI2[valve_iol_terminal_mixproof_DO3_DI2] --> i_mix_proof
```

## Class AI1 scheme ##

```mermaid
flowchart BT
    temperature_e[temperature_e] --> AI1[AI1]
    temperature_e_analog[temperature_e_analog] --> AI1
    temperature_e_iolink[temperature_e_iolink] --> AI1
    temperature_e_iolink_tm311[temperature_e_iolink_tm311] --> AI1
    pressure_e[pressure_e] --> AI1
    concentration_e[concentration_e] --> AI1
    concentration_e_ok[concentration_e_ok] --> concentration_e
    analog_input[analog_input] --> AI1
```

## Class i_motor scheme ##

```mermaid
flowchart BT
    virtual_motor[virtual_motor] --> i_motor[i_motor]
    motor[motor] --> i_motor
    motor_altivar[motor_altivar] --> i_motor
    motor_altivar_linear[motor_altivar_linear] --> motor_altivar
    motor --> io_device[io_device]
    motor_altivar --> io_device
    motor_altivar_linear --> io_device
```

## Class level scheme ##

```mermaid
flowchart BT
    level_e[level_e] --> level[level]
    level_e_cyl[level_e_cyl] --> level
    level_e_cone[level_e_cone] --> level
    level_e_iolink[level_e_iolink] --> level
```

## Class action scheme ##

```mermaid
flowchart BT
    on_action[on_action] --> action[action]
    delay_on_action[delay_on_action] --> action
    on_reverse_action[on_reverse_action] --> action
    off_action[off_action] --> action
    delay_off_action[delay_off_action] --> action
    open_seat_action[open_seat_action] --> action
    DI_DO_action[DI_DO_action] --> action
    inverted_DI_DO_action[inverted_DI_DO_action] --> DI_DO_action
    AI_AO_action[AI_AO_action] --> action
    required_DI_action[required_DI_action] --> action
    checked_devices_action[checked_devices_action] --> action
    wash_action[wash_action] --> action
    jump_if_action[jump_if_action] --> action
    enable_step_by_signal[enable_step_by_signal] --> action
```
