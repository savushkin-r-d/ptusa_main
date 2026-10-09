#!/bin/sh
### BEGIN INIT INFO
# Provides:          ptusa_main
# Required-Start:    $local_fs $network
# Required-Stop:     $local_fs $network
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: ptusa_main с локальной шиной PLCnext
### END INIT INFO
# ptusa_main-autostart-v1
# Установка от root: sh setup-ptusa-autostart.sh install
# Установка включает автозапуск, но не запускает программу немедленно.

set -eu
PATH=/usr/sbin:/usr/bin:/sbin:/bin
export PATH

APP_DIR=/opt/ptusamain
WORK_DIR=/opt/main
EXECUTABLE=$APP_DIR/ptusa_main
CONFIG_FILE=$WORK_DIR/ptusa_main.ini
SERVICE=/etc/init.d/ptusa_main
DEFAULTS=/etc/default/ptusa_main
SHORTCUT_DIR=/usr/bin
LEGACY_SHORTCUT_DIR=/usr/local/bin
PID_DIR=/run/ptusa_main
PID_FILE=$PID_DIR/ptusa_main.pid
LOG_FILE=/var/log/ptusa_main.log
RUN_USER=plcnext_firmware
RUN_GROUP=plcnext
PTUSA_ARGS=main.plua

fail()
{
    echo "Ошибка: $*" >&2
    exit 1
}

require_root()
{
    [ "$(id -u)" = 0 ] || fail "Эта команда должна выполняться от root."
}

has_config_arg()
(
    # Проверяем аргументы без подстановки имён файлов и без eval.
    set -f
    for argument in $PTUSA_ARGS; do
        case "$argument" in
            --) return 1 ;;
            --config|--config=*|-c|-c?*) return 0 ;;
        esac
    done
    return 1
)

load_defaults()
{
    # Файл создаётся от root; аргументы разделяются пробелами, без eval.
    if [ -f "$DEFAULTS" ]; then
        . "$DEFAULTS"
    fi
    [ -n "$PTUSA_ARGS" ] || fail "PTUSA_ARGS не должен быть пустым."
    if [ -f "$CONFIG_FILE" ] && ! has_config_arg; then
        if [ "$PTUSA_ARGS" = main.plua ]; then
            # Старое значение по умолчанию не переопределяет script в конфиге.
            PTUSA_ARGS="--config $CONFIG_FILE"
        else
            # Консольные настройки сохраняют приоритет над конфигом.
            PTUSA_ARGS="--config $CONFIG_FILE $PTUSA_ARGS"
        fi
    fi
}

check_application_files()
{
    [ -d "$APP_DIR" ] || fail "Нет каталога $APP_DIR."
    [ -d "$WORK_DIR" ] || fail "Нет рабочего каталога $WORK_DIR."
    [ -f "$EXECUTABLE" ] || fail "Нет программы $EXECUTABLE."
    [ -r "$WORK_DIR/main.plua" ] || fail "Нет доступного $WORK_DIR/main.plua."
    [ -r "$APP_DIR/libAxiobus.so.11" ] || fail "Нет libAxiobus.so.11."
}

check_files()
{
    check_application_files
    [ -f /etc/init.d/plcnext ] || fail "Не найдена служба SysV PLCnext."
    id "$RUN_USER" >/dev/null 2>&1 || fail "Нет пользователя $RUN_USER."
    grep -q "^$RUN_GROUP:" /etc/group || fail "Нет группы $RUN_GROUP."
    for utility in start-stop-daemon update-rc.d pidof install find chgrp; do
        command -v "$utility" >/dev/null || fail "Нет команды $utility."
    done
}

plcnext_running()
{
    if [ -r /run/plcnext/plcnext.pid ]; then
        plc_pid=$(cat /run/plcnext/plcnext.pid)
        case "$plc_pid" in
            ''|*[!0-9]*) ;;
            *)
                if [ "$plc_pid" -gt 1 ] && kill -0 "$plc_pid" 2>/dev/null; then
                    return 0
                fi
                ;;
        esac
    fi
    pidof Arp.System.Application >/dev/null 2>&1
}

running()
{
    start-stop-daemon --status --pidfile "$PID_FILE" \
        --exec "$EXECUTABLE" >/dev/null 2>&1
}

prepare_directory_permissions()
{
    # Программе нужны чтение её файлов и запись в рабочий каталог.
    # Сохраняем владельцев; find не следует по символическим ссылкам.
    [ ! -L "$APP_DIR" ] || fail "$APP_DIR не должен быть ссылкой."
    [ ! -L "$WORK_DIR" ] || fail "$WORK_DIR не должен быть ссылкой."
    find "$APP_DIR" -type d -exec chgrp "$RUN_GROUP" {} + \
        -exec chmod g+rx {} +
    find "$APP_DIR" -type f -exec chgrp "$RUN_GROUP" {} + \
        -exec chmod g+r {} +
    find "$WORK_DIR" -type d -exec chgrp "$RUN_GROUP" {} + \
        -exec chmod g+rwx {} +
    find "$WORK_DIR" -type f -exec chgrp "$RUN_GROUP" {} + \
        -exec chmod g+rw {} +
    chmod 755 "$EXECUTABLE"
}

prepare_permissions()
{
    prepare_directory_permissions

    # Драйвер устройства создаётся службой localbus при загрузке системы.
    waited=0
    while [ ! -c /dev/axio_xfer0 ] && [ "$waited" -lt 20 ]; do
        sleep 1
        waited=$((waited + 1))
    done
    [ -c /dev/axio_xfer0 ] || fail "Нет устройства /dev/axio_xfer0."
    chown "$RUN_USER:$RUN_GROUP" /dev/axio_xfer0
    chmod 600 /dev/axio_xfer0

    # Не удаляем файл с mutex библиотеки и не меняем его содержимое.
    [ ! -L /tmp/axiopdi ] || fail "/tmp/axiopdi не должен быть ссылкой."
    if [ -e /tmp/axiopdi ]; then
        [ -f /tmp/axiopdi ] || fail "/tmp/axiopdi не является файлом."
        chown "$RUN_USER:$RUN_GROUP" /tmp/axiopdi
        chmod 660 /tmp/axiopdi
    fi

    install -d -m 755 "$PID_DIR"
    [ ! -L "$LOG_FILE" ] || fail "$LOG_FILE не должен быть ссылкой."
    touch "$LOG_FILE"
    chown "$RUN_USER:$RUN_GROUP" "$LOG_FILE"
    chmod 660 "$LOG_FILE"
}

check_unmanaged_processes()
{
    # При смене каталога старый экземпляр также не должен владеть шиной.
    for executable_path in "$EXECUTABLE" "$WORK_DIR/ptusa_main"; do
        if start-stop-daemon --status --exec "$executable_path" \
            >/dev/null 2>&1; then
            fail "$executable_path запущена вне этой службы. Сначала остановите её."
        fi
    done
}

start_service()
{
    require_root
    load_defaults
    check_files
    if running; then
        echo "ptusa_main уже запущена."
        return 0
    fi
    if plcnext_running; then
        fail "PLCnext работает. Сначала остановите /etc/init.d/plcnext."
    fi
    check_unmanaged_processes
    prepare_permissions
    rm -f "$PID_FILE"
    # Отключаем подстановку имён файлов при разделении PTUSA_ARGS на слова.
    set -f
    start-stop-daemon --start --background --make-pidfile \
        --pidfile "$PID_FILE" --exec "$EXECUTABLE" \
        --chuid "$RUN_USER:$RUN_GROUP" --chdir "$WORK_DIR" \
        --umask 002 --output "$LOG_FILE" -- $PTUSA_ARGS
    sleep 2
    if ! running; then
        fail "ptusa_main завершилась при старте. Проверьте $LOG_FILE."
    fi
    echo "$EXECUTABLE запущена (PID $(cat "$PID_FILE"))."
    echo "Рабочий каталог: $WORK_DIR."
}

stop_service()
{
    require_root
    # SIGINT используется приложением для штатного завершения главного цикла.
    start-stop-daemon --stop --oknodo --pidfile "$PID_FILE" \
        --exec "$EXECUTABLE" --retry INT/15/KILL/2
    rm -f "$PID_FILE"
    echo "ptusa_main остановлена."
}

is_our_shortcut()
{
    [ ! -L "$1" ] && [ -f "$1" ] &&
        grep -q '^# ptusa_main-shortcut-v1$' "$1"
}

check_shortcuts()
{
    for shortcut_action in start stop restart install uninstall; do
        shortcut_file=$SHORTCUT_DIR/ptusa$shortcut_action
        if [ -e "$shortcut_file" ] || [ -L "$shortcut_file" ]; then
            is_our_shortcut "$shortcut_file" || \
                fail "$shortcut_file уже существует и создан не установщиком."
        fi
    done
}

install_shortcuts()
{
    check_shortcuts
    install -d -m 755 "$SHORTCUT_DIR"
    for shortcut_action in start stop restart install uninstall; do
        shortcut_file=$SHORTCUT_DIR/ptusa$shortcut_action
        (umask 022; cat > "$shortcut_file" <<EOF
#!/bin/sh
# ptusa_main-shortcut-v1
if [ "\$(id -u)" != 0 ]; then
    exec su -s /bin/sh -c 'exec /bin/sh "\$@"' root sh "\$0" "\$@"
fi
EOF
        )
        if [ "$shortcut_action" = install ]; then
            # Повторная установка использует скрипт из каталога программы.
            cat >> "$shortcut_file" <<EOF
if [ -f "$APP_DIR/setup-ptusa-autostart.sh" ]; then
    exec /bin/sh "$APP_DIR/setup-ptusa-autostart.sh" install "\$@"
fi
EOF
        fi
        cat >> "$shortcut_file" <<EOF
exec /bin/sh "$SERVICE" "$shortcut_action" "\$@"
EOF
        chown root:root "$shortcut_file"
        chmod 755 "$shortcut_file"
    done
    if [ "$SHORTCUT_DIR" != "$LEGACY_SHORTCUT_DIR" ]; then
        remove_shortcuts_from "$LEGACY_SHORTCUT_DIR"
    fi
}

remove_shortcuts_from()
{
    # Чужие файлы и ссылки с такими именами сохраняются.
    for shortcut_action in start stop restart install uninstall; do
        shortcut_file=$1/ptusa$shortcut_action
        if is_our_shortcut "$shortcut_file"; then
            rm -f "$shortcut_file"
        fi
    done
}

remove_shortcuts()
{
    remove_shortcuts_from "$SHORTCUT_DIR"
    remove_shortcuts_from "$LEGACY_SHORTCUT_DIR"
}

install_service()
{
    require_root
    check_files
    source_file=$(readlink -f "$0")
    if [ -e "$SERVICE" ]; then
        grep -q '^# ptusa_main-autostart-v1$' "$SERVICE" || \
            fail "$SERVICE уже существует и создан другим установщиком."
    fi
    # Конфликты команд выявляем до остановки служб и изменения настроек.
    check_shortcuts
    if [ -f "$SERVICE" ] && [ "$source_file" != "$SERVICE" ]; then
        # Старая служба знает путь своего бинарника до переноса программы.
        /bin/sh "$SERVICE" stop
    elif running; then
        stop_service
    fi
    check_unmanaged_processes
    if plcnext_running; then
        /etc/init.d/plcnext stop
    fi
    if plcnext_running; then
        fail "PLCnext не остановился; права Axiobus не изменены."
    fi
    prepare_permissions
    if [ ! -e "$DEFAULTS" ]; then
        install -d -m 755 /etc/default
        (umask 022; cat > "$DEFAULTS" <<'EOF'
# Аргументы ptusa_main; имена с пробелами не поддерживаются.
# Если есть /opt/main/ptusa_main.ini, служба подключает его автоматически.
# Другой конфиг: PTUSA_ARGS='--config /opt/main/other.ini'
# Для диагностики: PTUSA_ARGS='main.plua --read_only_io --opc off'
PTUSA_ARGS='main.plua'
EOF
        )
    fi
    chown root:root "$DEFAULTS"
    chmod 644 "$DEFAULTS"
    if [ "$source_file" != "$SERVICE" ]; then
        install -m 755 "$source_file" "$SERVICE"
    fi
    chown root:root "$SERVICE"
    chmod 755 "$SERVICE"
    install_shortcuts

    # disable сохраняет ссылки остановки PLCnext и допускает enable при откате.
    update-rc.d plcnext disable
    update-rc.d -f ptusa_main remove
    update-rc.d ptusa_main defaults 99 01
    echo "Автозапуск ptusa_main включён; автозапуск PLCnext отключён."
    echo "Программа запустится после перезагрузки. Для запуска сейчас:"
    echo "  $SERVICE start"
    echo "Лог: $LOG_FILE; аргументы: $DEFAULTS."
    echo "Программа: $EXECUTABLE; рабочий каталог: $WORK_DIR."
    echo "Конфиг по умолчанию (если существует): $CONFIG_FILE."
    echo "Команды в $SHORTCUT_DIR: ptusastart ptusastop ptusarestart"
    echo "  ptusainstall ptusauninstall (запрашивают пароль root через su)."
}

uninstall_service()
{
    require_root
    if [ -e "$SERVICE" ]; then
        grep -q '^# ptusa_main-autostart-v1$' "$SERVICE" || \
            fail "$SERVICE создан другим установщиком."
    fi
    stop_service
    update-rc.d -f ptusa_main remove
    update-rc.d plcnext enable
    remove_shortcuts
    rm -f "$SERVICE"
    echo "Автозапуск ptusa_main удалён; автозапуск PLCnext восстановлен."
    echo "PLCnext запустится после перезагрузки. Проект, настройки и лог сохранены."
}

case "${1:-help}" in
    install) install_service ;;
    uninstall) uninstall_service ;;
    start) start_service ;;
    stop) stop_service ;;
    restart) stop_service; start_service ;;
    status)
        if running; then
            echo "ptusa_main работает (PID $(cat "$PID_FILE"))."
        else
            echo "ptusa_main не работает."
            exit 3
        fi
        ;;
    check)
        check_files
        echo "Файлы проекта, пользователь и команды SysV доступны."
        ;;
    help|--help|-h)
        echo "Использование: $0 {install|uninstall|start|stop|restart|status|check}"
        echo "install/uninstall/start/stop/restart требуют root."
        echo "install не запускает программу; start разрешает запись DO/AO."
        ;;
    *) fail "Неизвестная команда: $1. Используйте --help." ;;
esac
