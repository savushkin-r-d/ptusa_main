"""Проверка аргументов и команд службы без запуска контроллера."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


SHELL = os.environ.get("PTUSA_TEST_SHELL") or shutil.which("sh")
SCRIPT = Path(__file__).with_name("setup-ptusa-autostart.sh")


@unittest.skipUnless(SHELL, "Для проверки нужен POSIX sh (PTUSA_TEST_SHELL).")
class AutostartTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="ptusa-autostart-")
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)
        self.config = self.directory / "ptusa_main.ini"
        self.defaults = self.directory / "defaults"
        self.shortcuts = self.directory / "commands"
        self.legacy_shortcuts = self.directory / "legacy-commands"
        self.project = self.directory / "main"
        self.project.mkdir()
        self.service = self.directory / "service.sh"
        self.service.write_text(
            "#!/bin/sh\n# ptusa_main-autostart-v1\n"
            "printf 'service\\n'\nprintf '%s\\n' \"$@\"\n",
            encoding="utf-8")
        self.utilities = self.directory / "utilities"
        self.utilities.mkdir()
        # Подменяем только проверку UID и su: реальные права не повышаются.
        (self.utilities / "id").write_text(
            '#!/bin/sh\nprintf \'%s\\n\' "${PTUSA_TEST_UID:-0}"\n',
            encoding="utf-8")
        (self.utilities / "su").write_text('''#!/bin/sh
printf 'su\\n' >> "$PTUSA_TEST_SU_LOG"
if [ -n "${PTUSA_TEST_SU_FAILURE:-}" ]; then
    exit "$PTUSA_TEST_SU_FAILURE"
fi
[ "$1" = -s ] || exit 90
shell=$2
shift 2
[ "$1" = -c ] || exit 91
command=$2
shift 2
[ "$1" = root ] || exit 92
shift
export PTUSA_TEST_UID=0
exec "$shell" -c "$command" "$@"
''', encoding="utf-8")
        for utility in self.utilities.iterdir():
            utility.chmod(0o755)

    def run_shell(self, program, check=True):
        environment = os.environ.copy()
        environment["PTUSA_TEST_SCRIPT"] = SCRIPT.as_posix()
        environment["PTUSA_TEST_DIRECTORY"] = self.directory.as_posix()
        environment["PTUSA_TEST_UID"] = "0"
        environment["PTUSA_TEST_SU_LOG"] = (
            self.directory / "su.log").as_posix()
        # Переопределяем системные пути: все файлы создаются во временной папке.
        result = subprocess.run(
            [SHELL, "-c", '''
set -- help
. "$PTUSA_TEST_SCRIPT" >/dev/null
DEFAULT_SHORTCUT_DIR=$SHORTCUT_DIR
DEFAULT_SERVICE_PATH=$PATH
CONFIG_FILE=$PTUSA_TEST_DIRECTORY/ptusa_main.ini
DEFAULTS=$PTUSA_TEST_DIRECTORY/defaults
SHORTCUT_DIR=$PTUSA_TEST_DIRECTORY/commands
LEGACY_SHORTCUT_DIR=$PTUSA_TEST_DIRECTORY/legacy-commands
APP_DIR=$PTUSA_TEST_DIRECTORY/main
SERVICE=$PTUSA_TEST_DIRECTORY/service.sh
PATH=$(cd "$PTUSA_TEST_DIRECTORY/utilities" && pwd):$PATH
export PATH
chown() { :; }
''' + program], env=environment, capture_output=True,
            text=True, encoding="utf-8", timeout=10)
        if check:
            self.assertEqual(result.returncode, 0, result.stderr)
        return result

    def load_arguments(self):
        result = self.run_shell('load_defaults\nprintf \'%s\\n\' "$PTUSA_ARGS"')
        return result.stdout.strip()

    def write_defaults(self, arguments):
        self.defaults.write_text(
            "PTUSA_ARGS='" + arguments + "'\n", encoding="utf-8")

    def test_shell_syntax(self):
        result = subprocess.run([SHELL, "-n", SCRIPT.as_posix()],
                                capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_without_config_uses_main_script(self):
        self.assertEqual(self.load_arguments(), "main.plua")

    def test_config_is_used_by_default(self):
        self.config.touch()
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix())

    def test_existing_default_arguments_use_config(self):
        self.config.touch()
        self.write_defaults("main.plua")
        original = self.defaults.read_bytes()
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix())
        self.assertEqual(self.defaults.read_bytes(), original)

    def test_custom_arguments_override_config(self):
        self.config.touch()
        arguments = "main.plua --read_only_io --opc off --sleep_time=2"
        self.write_defaults(arguments)
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix() + " " + arguments)

    def test_explicit_config_is_preserved(self):
        self.config.touch()
        for option in ("--config other.ini", "--config=other.ini",
                       "-c other.ini", "-cother.ini", "-c=other.ini"):
            with self.subTest(option=option):
                self.write_defaults(option + " --debug")
                self.assertEqual(self.load_arguments(), option + " --debug")

    def test_without_default_config_preserves_custom_arguments(self):
        arguments = "main.plua --read_only_io --opc off"
        self.write_defaults(arguments)
        self.assertEqual(self.load_arguments(), arguments)

    def test_positional_separator_does_not_hide_default_config(self):
        self.config.touch()
        arguments = "-- --config"
        self.write_defaults(arguments)
        self.assertEqual(self.load_arguments(),
                         "--config " + self.config.as_posix() + " " + arguments)

    def test_shortcuts_forward_actions_and_arguments(self):
        result = self.run_shell('''
install_shortcuts
PATH=$(cd "$SHORTCUT_DIR" && pwd):$PATH
export PATH
for action in start stop restart install uninstall; do
    "ptusa$action" --check "two words"
done
''')
        expected = []
        for action in ("start", "stop", "restart", "install", "uninstall"):
            expected.extend(["service", action, "--check", "two words"])
        self.assertEqual(result.stdout.splitlines(), expected)
        self.assertEqual(len(list(self.shortcuts.iterdir())), 5)
        self.assertFalse((self.directory / "su.log").exists())

    def test_shortcut_directory_is_in_system_path(self):
        result = self.run_shell(
            'printf \'%s\\n\' "$DEFAULT_SHORTCUT_DIR" "$DEFAULT_SERVICE_PATH"')
        directory, path = result.stdout.splitlines()
        self.assertIn(directory, path.split(":"))

    def test_admin_shortcuts_request_root_once_and_preserve_arguments(self):
        result = self.run_shell('''
install_shortcuts
export PTUSA_TEST_UID=1002
for action in start stop restart install uninstall; do
    "$SHORTCUT_DIR/ptusa$action" "two words" '; exit 7'
done
''')
        expected = []
        for action in ("start", "stop", "restart", "install", "uninstall"):
            expected.extend(["service", action, "two words", "; exit 7"])
        self.assertEqual(result.stdout.splitlines(), expected)
        self.assertEqual((self.directory / "su.log").read_text().splitlines(),
                         ["su"] * 5)

    def test_failed_authentication_does_not_call_service(self):
        result = self.run_shell('''
install_shortcuts
export PTUSA_TEST_UID=1002
export PTUSA_TEST_SU_FAILURE=1
"$SHORTCUT_DIR/ptusastart"
''', check=False)
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, "")

    def test_admin_shortcut_preserves_service_exit_code(self):
        self.service.write_text("#!/bin/sh\nexit 23\n", encoding="utf-8")
        result = self.run_shell('''
install_shortcuts
export PTUSA_TEST_UID=1002
"$SHORTCUT_DIR/ptusastart"
''', check=False)
        self.assertEqual(result.returncode, 23)

    def test_reinstall_migrates_owned_shortcuts_from_legacy_directory(self):
        self.legacy_shortcuts.mkdir()
        managed = self.legacy_shortcuts / "ptusastart"
        managed.write_text("#!/bin/sh\n# ptusa_main-shortcut-v1\n",
                           encoding="utf-8")
        foreign = self.legacy_shortcuts / "ptusastop"
        foreign.write_text("foreign\n", encoding="utf-8")
        self.run_shell("install_shortcuts")
        self.assertFalse(managed.exists())
        self.assertEqual(foreign.read_text(encoding="utf-8"), "foreign\n")
        self.assertEqual(len(list(self.shortcuts.iterdir())), 5)

    def test_install_shortcut_uses_updated_project_script(self):
        self.run_shell("install_shortcuts")
        installer = self.project / "setup-ptusa-autostart.sh"
        installer.write_text("#!/bin/sh\nprintf 'updated\\n'\n"
                             "printf '%s\\n' \"$@\"\n", encoding="utf-8")
        result = self.run_shell('"$SHORTCUT_DIR/ptusainstall" --check')
        self.assertEqual(result.stdout.splitlines(),
                         ["updated", "install", "--check"])

    def test_reinstall_updates_managed_shortcuts(self):
        self.run_shell("install_shortcuts")
        command = self.shortcuts / "ptusastart"
        command.write_text("#!/bin/sh\n# ptusa_main-shortcut-v1\nexit 7\n",
                           encoding="utf-8")
        result = self.run_shell('install_shortcuts\n"$SHORTCUT_DIR/ptusastart"')
        self.assertEqual(result.stdout.splitlines(), ["service", "start"])

    def test_install_does_not_overwrite_foreign_commands(self):
        self.shortcuts.mkdir()
        command = self.shortcuts / "ptusastop"
        command.write_text("foreign\n", encoding="utf-8")
        result = self.run_shell("install_shortcuts", check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(command.read_text(encoding="utf-8"), "foreign\n")
        self.assertEqual(list(self.shortcuts.iterdir()), [command])

    def test_uninstall_preserves_foreign_files(self):
        self.run_shell("install_shortcuts")
        command = self.shortcuts / "ptusastop"
        command.write_text("foreign\n", encoding="utf-8")
        unrelated = self.shortcuts / "unrelated"
        unrelated.touch()
        self.run_shell("remove_shortcuts")
        self.assertEqual(set(self.shortcuts.iterdir()), {command, unrelated})

    def test_uninstall_service_removes_shortcuts(self):
        self.legacy_shortcuts.mkdir()
        (self.legacy_shortcuts / "ptusastart").write_text(
            "#!/bin/sh\n# ptusa_main-shortcut-v1\n", encoding="utf-8")
        self.run_shell('''
require_root() { :; }
stop_service() { :; }
update-rc.d() { :; }
install_shortcuts
uninstall_service
''')
        self.assertFalse(self.service.exists())
        self.assertEqual(list(self.shortcuts.iterdir()), [])
        self.assertEqual(list(self.legacy_shortcuts.iterdir()), [])

    def test_install_does_not_follow_shortcut_symlinks(self):
        self.shortcuts.mkdir()
        command = self.shortcuts / "ptusastart"
        try:
            command.symlink_to(self.service)
        except OSError:
            self.skipTest("Создание символических ссылок недоступно.")
        original = self.service.read_bytes()
        result = self.run_shell("install_shortcuts", check=False)
        self.assertNotEqual(result.returncode, 0)
        self.assertTrue(command.is_symlink())
        self.assertEqual(self.service.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
