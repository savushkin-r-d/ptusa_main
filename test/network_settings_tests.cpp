#include "gtest/gtest.h"
#include "network_settings.h"

// Тесты для port_forward_rule
TEST(PortForwardRuleTest, Constructor)
{
    port_forward_rule rule("A100", 1962, 10001, "192.168.1.100");
    
    EXPECT_STREQ("A100", rule.get_device_name());
    EXPECT_STREQ("192.168.1.100", rule.get_device_ip());
    EXPECT_EQ(1962u, rule.get_internal_port());
    EXPECT_EQ(10001u, rule.get_external_port());
    EXPECT_FALSE(rule.is_open());
}

TEST(PortForwardRuleTest, ConstructorWithZeroExternalPort)
{
    // Если external_port = 0, должен использоваться internal_port
    port_forward_rule rule("A100", 502, 0, "192.168.1.100");
    
    EXPECT_EQ(502u, rule.get_internal_port());
    EXPECT_EQ(502u, rule.get_external_port());
}

TEST(PortForwardRuleTest, OpenPort)
{
    port_forward_rule rule("A100", 1962, 10001, "192.168.1.100");
    
    EXPECT_FALSE(rule.is_open());
    EXPECT_TRUE(rule.open_port());
    EXPECT_TRUE(rule.is_open());
}

TEST(PortForwardRuleTest, ClosePort)
{
    port_forward_rule rule("A100", 1962, 10001, "192.168.1.100");
    
    rule.open_port();
    EXPECT_TRUE(rule.is_open());
    
    rule.close_port();
    EXPECT_FALSE(rule.is_open());
}

TEST(PortForwardRuleTest, OpenPortTwice)
{
    port_forward_rule rule("A100", 1962, 10001, "192.168.1.100");
    
    EXPECT_TRUE(rule.open_port());
    EXPECT_TRUE(rule.is_open());
    
    // Повторное открытие должно быть успешным
    EXPECT_TRUE(rule.open_port());
    EXPECT_TRUE(rule.is_open());
}

// Тесты для network_settings_manager
TEST(NetworkSettingsManagerTest, Singleton)
{
    network_settings_manager* manager1 = G_NETWORK_SETTINGS_MANAGER();
    network_settings_manager* manager2 = G_NETWORK_SETTINGS_MANAGER();
    
    EXPECT_EQ(manager1, manager2);
    EXPECT_NE(nullptr, manager1);
}

TEST(NetworkSettingsManagerTest, AddPortForwardRule)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    
    port_forward_rule* rule = manager->get_rule(10001);
    ASSERT_NE(nullptr, rule);
    EXPECT_STREQ("A100", rule->get_device_name());
    EXPECT_EQ(10001u, rule->get_internal_port());
}

TEST(NetworkSettingsManagerTest, AddMultipleRules)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    // Добавляем несколько правил согласно примерам из issue
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    manager->add_port_forward_rule(11001, "A100", 1962, 11001, "192.168.1.100");
    manager->add_port_forward_rule(502, "Schneider", 502, 0, "192.168.1.200");
    manager->add_port_forward_rule(6626, "WAGO", 6626, 0, "192.168.1.201");
    
    EXPECT_NE(nullptr, manager->get_rule(10001));
    EXPECT_NE(nullptr, manager->get_rule(11001));
    EXPECT_NE(nullptr, manager->get_rule(502));
    EXPECT_NE(nullptr, manager->get_rule(6626));
}

TEST(NetworkSettingsManagerTest, ProcessCommandOpen)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    
    EXPECT_TRUE(manager->process_command(10001, true));
    
    port_forward_rule* rule = manager->get_rule(10001);
    ASSERT_NE(nullptr, rule);
    EXPECT_TRUE(rule->is_open());
}

TEST(NetworkSettingsManagerTest, ProcessCommandClose)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    
    manager->process_command(10001, true);
    EXPECT_TRUE(manager->get_rule(10001)->is_open());
    
    EXPECT_TRUE(manager->process_command(10001, false));
    EXPECT_FALSE(manager->get_rule(10001)->is_open());
}

TEST(NetworkSettingsManagerTest, ProcessCommandNonExistent)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    // Команда для несуществующего правила должна вернуть false
    EXPECT_FALSE(manager->process_command(99999, true));
}

TEST(NetworkSettingsManagerTest, GetRuleNonExistent)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    port_forward_rule* rule = manager->get_rule(99999);
    EXPECT_EQ(nullptr, rule);
}

TEST(NetworkSettingsManagerTest, CloseAllPorts)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    manager->add_port_forward_rule(11001, "A100", 1962, 11001, "192.168.1.100");
    
    manager->process_command(10001, true);
    manager->process_command(11001, true);
    
    EXPECT_TRUE(manager->get_rule(10001)->is_open());
    EXPECT_TRUE(manager->get_rule(11001)->is_open());
    
    manager->close_all_ports();
    
    EXPECT_FALSE(manager->get_rule(10001)->is_open());
    EXPECT_FALSE(manager->get_rule(11001)->is_open());
}

TEST(NetworkSettingsManagerTest, ReplaceExistingRule)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    
    port_forward_rule* rule1 = manager->get_rule(10001);
    ASSERT_NE(nullptr, rule1);
    EXPECT_STREQ("A100", rule1->get_device_name());
    
    // Заменяем правило
    manager->add_port_forward_rule(10001, "A200", 10002, 0, "192.168.1.101");
    
    port_forward_rule* rule2 = manager->get_rule(10001);
    ASSERT_NE(nullptr, rule2);
    EXPECT_STREQ("A200", rule2->get_device_name());
    EXPECT_EQ(10002u, rule2->get_internal_port());
    EXPECT_STREQ("192.168.1.101", rule2->get_device_ip());
}

TEST(NetworkSettingsManagerTest, Evaluate)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    // evaluate() не должен падать даже без правил
    EXPECT_NO_THROW(manager->evaluate());
    
    // С правилами тоже не должен падать
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    EXPECT_NO_THROW(manager->evaluate());
}

// Тесты для set_cmd - управление через Монитор
TEST(NetworkSettingsManagerTest, SetCmdOpenPort)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    
    // Открываем порт через set_cmd
    int result = manager->set_cmd("CMD", 10001, 
                                   (double)network_settings_manager::CMD_OPEN_PORT);
    EXPECT_EQ(0, result);
    
    port_forward_rule* rule = manager->get_rule(10001);
    ASSERT_NE(nullptr, rule);
    EXPECT_TRUE(rule->is_open());
}

TEST(NetworkSettingsManagerTest, SetCmdClosePort)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    manager->process_command(10001, true); // Сначала открываем
    
    // Закрываем порт через set_cmd
    int result = manager->set_cmd("CMD", 10001, 
                                   (double)network_settings_manager::CMD_CLOSE_PORT);
    EXPECT_EQ(0, result);
    
    port_forward_rule* rule = manager->get_rule(10001);
    ASSERT_NE(nullptr, rule);
    EXPECT_FALSE(rule->is_open());
}

TEST(NetworkSettingsManagerTest, SetCmdCloseAllPorts)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    manager->add_port_forward_rule(11001, "A100", 1962, 11001, "192.168.1.100");
    
    manager->process_command(10001, true);
    manager->process_command(11001, true);
    
    // Закрываем все порты через set_cmd
    int result = manager->set_cmd("CMD", 0, 
                                   (double)network_settings_manager::CMD_CLOSE_ALL_PORTS);
    EXPECT_EQ(0, result);
    
    EXPECT_FALSE(manager->get_rule(10001)->is_open());
    EXPECT_FALSE(manager->get_rule(11001)->is_open());
}

TEST(NetworkSettingsManagerTest, SetCmdNonExistentPort)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    // Попытка открыть несуществующий порт
    int result = manager->set_cmd("CMD", 99999, 
                                   (double)network_settings_manager::CMD_OPEN_PORT);
    EXPECT_EQ(1, result); // Должна вернуть ошибку
}

TEST(NetworkSettingsManagerTest, SetCmdUnknownCommand)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    
    // Неизвестная команда
    int result = manager->set_cmd("CMD", 10001, 999.0);
    EXPECT_EQ(1, result); // Должна вернуть ошибку
}

TEST(NetworkSettingsManagerTest, SetCmdUnknownProperty)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    // Неизвестное свойство
    int result = manager->set_cmd("UNKNOWN_PROP", 0, 1.0);
    EXPECT_EQ(1, result); // Должна вернуть ошибку
}

TEST(NetworkSettingsManagerTest, SetCmdString)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    // Строковые команды не поддерживаются
    int result = manager->set_cmd("CMD", 0, "test");
    EXPECT_EQ(1, result); // Должна вернуть ошибку
}

TEST(NetworkSettingsManagerTest, SaveDevice)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    manager->add_port_forward_rule(10001, "A100", 10001, 0, "192.168.1.100");
    manager->add_port_forward_rule(11001, "A100", 1962, 11001, "192.168.1.100");
    manager->process_command(10001, true); // Открываем один порт
    
    char buffer[4096];
    int size = manager->save_device(buffer);
    
    EXPECT_GT(size, 0);
    EXPECT_NE(nullptr, strstr(buffer, "NETWORK_SETTINGS"));
    EXPECT_NE(nullptr, strstr(buffer, "A100"));
    EXPECT_NE(nullptr, strstr(buffer, "192.168.1.100"));
}

TEST(NetworkSettingsManagerTest, GetNameInLua)
{
    network_settings_manager* manager = G_NETWORK_SETTINGS_MANAGER();
    
    EXPECT_STREQ("NETWORK_SETTINGS", manager->get_name_in_Lua());
}

