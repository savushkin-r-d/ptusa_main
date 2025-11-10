#pragma once

#include "smart_ptr.h"
#include "g_device.h"
#include <map>
#include <string>

/// @brief Класс для хранения правила перенаправления порта
class port_forward_rule
{
public:
    /// @brief Конструктор правила перенаправления порта
    /// @param device_name Имя устройства (например, "A100")
    /// @param internal_port Внутренний порт устройства
    /// @param external_port Внешний порт для доступа
    /// @param device_ip IP адрес устройства
    port_forward_rule(const char* device_name, unsigned int internal_port, 
                     unsigned int external_port, const char* device_ip);
    
    ~port_forward_rule();

    /// @brief Открыть порт
    /// @return true если порт успешно открыт
    bool open_port();
    
    /// @brief Закрыть порт
    void close_port();
    
    /// @brief Проверить, открыт ли порт
    /// @return true если порт открыт
    bool is_open() const;
    
    /// @brief Получить внутренний порт
    unsigned int get_internal_port() const;
    
    /// @brief Получить внешний порт
    unsigned int get_external_port() const;
    
    /// @brief Получить имя устройства
    const char* get_device_name() const;
    
    /// @brief Получить IP адрес устройства
    const char* get_device_ip() const;

private:
    char* device_name;
    char* device_ip;
    unsigned int internal_port;
    unsigned int external_port;
    bool port_opened;
};

typedef std::map<unsigned int, port_forward_rule*> port_forward_map;
typedef std::pair<unsigned int, port_forward_rule*> port_forward_pair;

/// @brief Менеджер сетевых настроек для управления перенаправлением портов
class network_settings_manager : public i_Lua_save_device, public i_cmd_device
{
public:
    virtual ~network_settings_manager();
    
    /// @brief Получить единственный экземпляр менеджера
    static network_settings_manager* get_instance();
    
    /// @brief Добавить правило перенаправления порта
    /// @param command_id ID команды для активации правила (например, 10001)
    /// @param device_name Имя устройства
    /// @param internal_port Внутренний порт устройства
    /// @param external_port Внешний порт для доступа (0 = использовать internal_port)
    /// @param device_ip IP адрес устройства
    void add_port_forward_rule(unsigned int command_id, const char* device_name,
                              unsigned int internal_port, unsigned int external_port,
                              const char* device_ip);
    
    /// @brief Обработать команду открытия/закрытия порта
    /// @param command_id ID команды
    /// @param open true для открытия, false для закрытия
    /// @return true если команда выполнена успешно
    bool process_command(unsigned int command_id, bool open = true);
    
    /// @brief Получить правило по ID команды
    /// @param command_id ID команды
    /// @return Указатель на правило или nullptr
    port_forward_rule* get_rule(unsigned int command_id);
    
    /// @brief Закрыть все открытые порты
    void close_all_ports();
    
    /// @brief Оценка состояния (вызывается в главном цикле)
    void evaluate();

    // Интерфейс i_Lua_save_device
    int save_device(char* buff) override;
    const char* get_name_in_Lua() const override;
    
    // Интерфейс i_cmd_device
    int set_cmd(const char* prop, u_int idx, double val) override;
    int set_cmd(const char* prop, u_int idx, const char* val) override;
    
    /// @brief Команды для управления через Монитор
    enum COMMANDS
    {
        CMD_OPEN_PORT = 1,      ///< Открыть порт (idx = command_id)
        CMD_CLOSE_PORT = 2,     ///< Закрыть порт (idx = command_id)
        CMD_CLOSE_ALL_PORTS = 3 ///< Закрыть все порты
    };

protected:
    network_settings_manager();
    port_forward_map rules;
    static auto_smart_ptr<network_settings_manager> instance;
};

/// @brief Глобальная функция для доступа к менеджеру сетевых настроек
network_settings_manager* G_NETWORK_SETTINGS_MANAGER();
