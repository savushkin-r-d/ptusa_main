-- Пример использования менеджера сетевых настроек
-- Example of using the network settings manager

-- Получаем экземпляр менеджера сетевых настроек
-- Get the network settings manager instance
local net_mgr = G_NETWORK_SETTINGS_MANAGER()

print("=== Демонстрация работы с сетевыми настройками ===")
print("=== Network Settings Demonstration ===\n")

-- Добавляем правила перенаправления портов согласно требованиям из issue
-- Add port forwarding rules according to the issue requirements

-- 1. Команда 10001 открывает порт 10001 устройства A100
--    Command 10001 opens port 10001 on device A100
print("1. Добавление правила для команды 10001 (порт 10001, устройство A100)")
net_mgr:add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100")

-- 2. Команда 11001 осуществляет проброс порта 1962 устройства A100 (режим Startup+)
--    Command 11001 forwards port 1962 of device A100 (Startup+ mode)
print("2. Добавление правила для команды 11001 (порт 1962->11001, устройство A100, Startup+)")
net_mgr:add_port_forward_rule(11001, "A100", 1962, 11001, "192.168.1.100")

-- 3. Для устройств Schneider Electric (SoMove) необходимо открыть порт 502
--    For Schneider Electric devices (SoMove), port 502 needs to be opened
print("3. Добавление правила для Schneider Electric SoMove (порт 502)")
net_mgr:add_port_forward_rule(502, "Schneider_SoMove", 502, 0, "192.168.1.200")

-- 4. Для WAGO IO-Check требуется открыть порт 6626
--    For WAGO IO-Check, port 6626 needs to be opened
print("4. Добавление правила для WAGO IO-Check (порт 6626)")
net_mgr:add_port_forward_rule(6626, "WAGO_IOCheck", 6626, 0, "192.168.1.201")

print("\nВсе правила добавлены успешно!\n")
print("All rules added successfully!\n")

-- Демонстрация управления портами
-- Demonstration of port management
print("=== Демонстрация управления портами ===")
print("=== Port Management Demonstration ===\n")

-- Открываем порт 10001 устройства A100
print("Открытие порта 10001 устройства A100...")
local result = net_mgr:process_command(10001, true)
if result then
    print("✓ Порт 10001 успешно открыт")
else
    print("✗ Ошибка при открытии порта 10001")
end

-- Открываем порт 1962 устройства A100 (режим Startup+)
print("\nОткрытие порта 1962->11001 устройства A100 (Startup+)...")
result = net_mgr:process_command(11001, true)
if result then
    print("✓ Порт 1962->11001 успешно открыт")
else
    print("✗ Ошибка при открытии порта 1962->11001")
end

-- Закрываем порт 10001
print("\nЗакрытие порта 10001...")
result = net_mgr:process_command(10001, false)
if result then
    print("✓ Порт 10001 успешно закрыт")
else
    print("✗ Ошибка при закрытии порта 10001")
end

-- Пример обработки несуществующей команды
print("\nПопытка обработки несуществующей команды 99999...")
result = net_mgr:process_command(99999, true)
if not result then
    print("✓ Корректно обработана несуществующая команда")
end

print("\n=== Демонстрация завершена ===")
print("=== Demonstration completed ===\n")

print("Для завершения работы все открытые порты будут автоматически закрыты")
print("при вызове close_all_ports() или при завершении программы.")
print("\nAll open ports will be automatically closed on close_all_ports()")
print("call or program termination.")
