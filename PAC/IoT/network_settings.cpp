#include "network_settings.h"
#include "log.h"
#include <cstring>
#include <cstdio>

// port_forward_rule implementation
port_forward_rule::port_forward_rule(const char* dev_name, unsigned int int_port,
                                     unsigned int ext_port, const char* dev_ip)
    : internal_port(int_port)
    , external_port(ext_port == 0 ? int_port : ext_port)
    , port_opened(false)
{
    device_name = new char[strlen(dev_name) + 1];
    strcpy(device_name, dev_name);
    
    device_ip = new char[strlen(dev_ip) + 1];
    strcpy(device_ip, dev_ip);
}

port_forward_rule::~port_forward_rule()
{
    if (port_opened)
    {
        close_port();
    }
    
    delete[] device_name;
    device_name = nullptr;
    
    delete[] device_ip;
    device_ip = nullptr;
}

bool port_forward_rule::open_port()
{
    if (port_opened)
    {
        G_LOG->info("Port forward rule for device %s already opened (internal port %u -> external port %u)",
                   device_name, internal_port, external_port);
        return true;
    }
    
    // Здесь должна быть реальная логика открытия порта
    // В данной реализации мы просто логируем событие
    G_LOG->notice("Opening port forward: device %s, IP %s, internal port %u -> external port %u",
                 device_name, device_ip, internal_port, external_port);
    
    port_opened = true;
    return true;
}

void port_forward_rule::close_port()
{
    if (!port_opened)
    {
        return;
    }
    
    G_LOG->notice("Closing port forward: device %s, IP %s, internal port %u -> external port %u",
                 device_name, device_ip, internal_port, external_port);
    
    port_opened = false;
}

bool port_forward_rule::is_open() const
{
    return port_opened;
}

unsigned int port_forward_rule::get_internal_port() const
{
    return internal_port;
}

unsigned int port_forward_rule::get_external_port() const
{
    return external_port;
}

const char* port_forward_rule::get_device_name() const
{
    return device_name;
}

const char* port_forward_rule::get_device_ip() const
{
    return device_ip;
}

// network_settings_manager implementation
network_settings_manager::network_settings_manager()
{
}

network_settings_manager::~network_settings_manager()
{
    close_all_ports();
    
    if (rules.size() > 0)
    {
        port_forward_map::iterator it;
        for (it = rules.begin(); it != rules.end(); it++)
        {
            delete it->second;
        }
        rules.clear();
    }
}

network_settings_manager* network_settings_manager::get_instance()
{
    if (instance.is_null())
    {
        instance = new network_settings_manager();
    }
    return instance;
}

void network_settings_manager::add_port_forward_rule(unsigned int command_id,
                                                     const char* device_name,
                                                     unsigned int internal_port,
                                                     unsigned int external_port,
                                                     const char* device_ip)
{
    // Проверяем, не существует ли уже правило с таким command_id
    port_forward_map::iterator it = rules.find(command_id);
    if (it != rules.end())
    {
        G_LOG->warning("Port forward rule for command %u already exists, replacing", command_id);
        delete it->second;
        rules.erase(it);
    }
    
    port_forward_rule* new_rule = new port_forward_rule(device_name, internal_port,
                                                        external_port, device_ip);
    rules.emplace(command_id, new_rule);
    
    G_LOG->info("Added port forward rule: command %u -> device %s (%s:%u -> %u)",
               command_id, device_name, device_ip, internal_port, external_port);
}

bool network_settings_manager::process_command(unsigned int command_id, bool open)
{
    port_forward_map::iterator it = rules.find(command_id);
    if (it == rules.end())
    {
        G_LOG->warning("Port forward rule for command %u not found", command_id);
        return false;
    }
    
    port_forward_rule* rule = it->second;
    
    if (open)
    {
        return rule->open_port();
    }
    else
    {
        rule->close_port();
        return true;
    }
}

port_forward_rule* network_settings_manager::get_rule(unsigned int command_id)
{
    port_forward_map::iterator it = rules.find(command_id);
    if (it != rules.end())
    {
        return it->second;
    }
    return nullptr;
}

void network_settings_manager::close_all_ports()
{
    if (rules.size() > 0)
    {
        port_forward_map::iterator it;
        for (it = rules.begin(); it != rules.end(); it++)
        {
            if (it->second && it->second->is_open())
            {
                it->second->close_port();
            }
        }
    }
}

void network_settings_manager::evaluate()
{
    // В будущем здесь может быть логика для мониторинга состояния портов
    // или автоматического переоткрытия портов при необходимости
}

int network_settings_manager::save_device(char* buff)
{
    int size = 0;
    size += sprintf(buff + size, "NETWORK_SETTINGS = \n\t{\n");
    
    if (rules.size() > 0)
    {
        port_forward_map::iterator it;
        for (it = rules.begin(); it != rules.end(); it++)
        {
            port_forward_rule* rule = it->second;
            size += sprintf(buff + size, "\t[%u] = {device=\"%s\", ip=\"%s\", "
                          "internal_port=%u, external_port=%u, is_open=%d},\n",
                          it->first,
                          rule->get_device_name(),
                          rule->get_device_ip(),
                          rule->get_internal_port(),
                          rule->get_external_port(),
                          rule->is_open() ? 1 : 0);
        }
    }
    
    size += sprintf(buff + size, "\t}\n");
    return size;
}

const char* network_settings_manager::get_name_in_Lua() const
{
    return "NETWORK_SETTINGS";
}

int network_settings_manager::set_cmd(const char* prop, u_int idx, double val)
{
    if (strcmp(prop, "CMD") == 0)
    {
        switch ((COMMANDS)(int)val)
        {
            case CMD_OPEN_PORT:
                if (process_command(idx, true))
                {
                    G_LOG->notice("Network settings: opened port for command %u via Monitor", idx);
                    return 0;
                }
                else
                {
                    G_LOG->warning("Network settings: failed to open port for command %u", idx);
                    return 1;
                }
                
            case CMD_CLOSE_PORT:
                if (process_command(idx, false))
                {
                    G_LOG->notice("Network settings: closed port for command %u via Monitor", idx);
                    return 0;
                }
                else
                {
                    G_LOG->warning("Network settings: failed to close port for command %u", idx);
                    return 1;
                }
                
            case CMD_CLOSE_ALL_PORTS:
                close_all_ports();
                G_LOG->notice("Network settings: closed all ports via Monitor");
                return 0;
                
            default:
                G_LOG->warning("Network settings: unknown command %d", (int)val);
                return 1;
        }
    }
    
    G_LOG->warning("Network settings: unknown property '%s'", prop);
    return 1;
}

int network_settings_manager::set_cmd(const char* prop, u_int idx, const char* val)
{
    // Строковые команды не используются в данной реализации
    G_LOG->warning("Network settings: string commands not supported (prop='%s', val='%s')", prop, val);
    return 1;
}

auto_smart_ptr<network_settings_manager> network_settings_manager::instance;

network_settings_manager* G_NETWORK_SETTINGS_MANAGER()
{
    return network_settings_manager::get_instance();
}
