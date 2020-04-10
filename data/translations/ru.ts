<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru" sourcelanguage="en">
<context>
    <name>CustomEditor</name>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="29"/>
        <source>RazerGenie - Custom Editor</source>
        <translation>RazerGenie - Редактор</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="75"/>
        <location filename="../../src/customeditor/customeditor.cpp" line="85"/>
        <source>Unknown matrix dimensions</source>
        <translation>Неизвестные измерения матрицы</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="75"/>
        <location filename="../../src/customeditor/customeditor.cpp" line="85"/>
        <source>Please open an issue in the RazerGenie repository. Device name: %1 - matrix dimens: %2 %3</source>
        <translation>Пожалуйста, откройте жалобу в репозитории RazerGenie. Device name: %1 - matrix dimens: %2 %3</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="91"/>
        <source>Device type not implemented!</source>
        <translation>Тип устройства не реализован!</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="91"/>
        <source>Please open an issue in the RazerGenie repository. Device type: %1</source>
        <translation>Пожалуйста, откройте жалобу в репозитории RazerGenie. Device type: %1</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="123"/>
        <source>Set</source>
        <translation>Установить</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="124"/>
        <source>Clear</source>
        <translation>Очистить</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="125"/>
        <source>Clear All</source>
        <translation>Очистить все</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="151"/>
        <source>You are using a keyboard with a layout which is not known to the daemon. Please help us by visiting &lt;a href=&apos;https://github.com/openrazer/openrazer/wiki/Keyboard-layouts&apos;&gt;https://github.com/openrazer/openrazer/wiki/Keyboard-layouts&lt;/a&gt;. Using a fallback layout for now.</source>
        <translation>Вы используете клавиатуру с раскладкой, неизвестной сервису OpenRazer. Пожалуйста помогите нам это исправит посетив &lt;a href=&apos;https://github.com/openrazer/openrazer/wiki/Keyboard-layouts&apos;&gt;https://github.com/openrazer/openrazer/wiki/Keyboard-layouts&lt;/a&gt; Будет использованна запасная раскладка.</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="153"/>
        <source>Your keyboard layout (%1) is not yet supported by RazerGenie for this keyboard. Please open an issue in the RazerGenie repository.</source>
        <translation>Раскладка вашей клавиатуры (%1) еще не поддерживается RazerGenie. Пожалуйста откройте жалобу на странице репозитория RazerGenie.</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="167"/>
        <source>Neither one of these layouts was found in the layout file: %1. Exiting.</source>
        <translation>Ни одна раскладка не была найдена в файле: %1. Редактор будет закрыт.</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="273"/>
        <source>Error loading %1.json!</source>
        <translation>Ошибка загрузки %1.json!</translation>
    </message>
    <message>
        <location filename="../../src/customeditor/customeditor.cpp" line="273"/>
        <source>The file %1.json, used for the custom editor failed to load: %2
The editor won&apos;t open now.</source>
        <translation>Файл %1.json, нужный редактору, загрузить не удалось: %2
Редактор открыт не будет.</translation>
    </message>
</context>
<context>
    <name>DeviceListWidget</name>
    <message>
        <location filename="../../src/devicelistwidget.cpp" line="39"/>
        <source>Downloading image...</source>
        <translation>Загрузка изображения...</translation>
    </message>
    <message>
        <location filename="../../src/devicelistwidget.cpp" line="79"/>
        <source>No image</source>
        <translation>Нет изображения</translation>
    </message>
</context>
<context>
    <name>Preferences</name>
    <message>
        <location filename="../../src/preferences/preferences.cpp" line="28"/>
        <location filename="../../src/preferences/preferences.cpp" line="71"/>
        <source>RazerGenie - Preferences</source>
        <translation>RazerGenie - Настройки</translation>
    </message>
    <message>
        <location filename="../../src/preferences/preferences.cpp" line="35"/>
        <source>About:</source>
        <translation>О программе:</translation>
    </message>
    <message>
        <location filename="../../src/preferences/preferences.cpp" line="39"/>
        <source>RazerGenie Version: %1</source>
        <translation>RazerGenie Version: %1</translation>
    </message>
    <message>
        <location filename="../../src/preferences/preferences.cpp" line="42"/>
        <source>OpenRazer Daemon Version: %1</source>
        <translation>OpenRazer Daemon Version: %1</translation>
    </message>
    <message>
        <location filename="../../src/preferences/preferences.cpp" line="45"/>
        <source>General:</source>
        <translation>Основное:</translation>
    </message>
    <message>
        <location filename="../../src/preferences/preferences.cpp" line="49"/>
        <source>For displaying device images, RazerGenie downloads the image behind the URL specified for a device in the OpenRazer daemon source code. This will only be done for devices that are connected to the PC and only once, as the images are cached locally. For reviewing, what information Razer might collect with these connections, please consult the Razer Privacy Policy (https://www.razer.com/legal/privacy-policy).</source>
        <translation>Для показа изображений устройств, RazerGenie загрузит изображение по ссылке, указанной в коде OpenRazer daemon. Это будет сделанно только для подключенных к ПК устройств и только один раз. Компания Razer может собирать информацию об этих соединениях. Для точной информации посетите страницу https://www.razer.com/legal/privacy-policy.</translation>
    </message>
    <message>
        <location filename="../../src/preferences/preferences.cpp" line="53"/>
        <source>Download device images</source>
        <translation>Загружать изображения устройств</translation>
    </message>
</context>
<context>
    <name>QMessageBox</name>
    <message>
        <location filename="../../src/util.cpp" line="26"/>
        <source>Error!</source>
        <translation>Ошибка!</translation>
    </message>
    <message>
        <location filename="../../src/util.cpp" line="33"/>
        <source>Information!</source>
        <translation>Информация!</translation>
    </message>
</context>
<context>
    <name>RazerGenie</name>
    <message>
        <location filename="../../src/razergenie.cpp" line="75"/>
        <source>The OpenRazer daemon is not started and you are not using systemd as your init system.
You have to either start the daemon manually every time you log in or set up another method of autostarting the daemon.

Manually starting would be running &quot;openrazer-daemon&quot; in a terminal.</source>
        <translation>OpenRazer daemon не запущен и вы не используете systemd в качестве системы инициальзации.
Вам придется запускать его каждый раз самому или добавить его в автозапуск другим способом.

Для ручного запуска используйте &quot;openrazer-daemon&quot; в терминале.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="59"/>
        <source>The OpenRazer daemon is not installed</source>
        <translation>OpenRazer Daemon не установлен</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="60"/>
        <source>The daemon is not installed or the version installed is too old. Please follow the installation instructions on the website!

If you are running RazerGenie as a flatpak, you will still have to install OpenRazer outside of flatpak from a distribution package.</source>
        <translation>Сервис OpenRazer не установлен или установлена слишком старая версия. Пожалуйста обновите или установите его, как это написано на вебсайте.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="61"/>
        <source>Open OpenRazer website</source>
        <translation>Открыть вебсайт OpenRazer</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="74"/>
        <source>The OpenRazer daemon is not available.</source>
        <translation>OpenRazer Daemon не доступен.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="86"/>
        <source>The OpenRazer daemon is currently not available. The status output is below.</source>
        <translation>Сервис OpenRazer в данный момент недоступен. Его текущий статус ниже.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="88"/>
        <source>If you think, there&apos;s a bug, you can report an issue on GitHub:</source>
        <translation>Если вам кажется, что здесь есть ошибка, пожалуста сообщите нам на GitHub:</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="89"/>
        <location filename="../../src/razergenie.cpp" line="823"/>
        <location filename="../../src/razergenie.cpp" line="831"/>
        <source>Report issue</source>
        <translation>Сообщить об ошибке</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="110"/>
        <source>The OpenRazer daemon is not set to auto-start. Click &quot;Enable&quot; to use the full potential of the daemon right after login.</source>
        <translation>OpenRazer Daemon не включен для автозапуска. Нажмите &quot;Включить&quot;, что бы начать использовать весь его потенциал сразу после входа в систему.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="111"/>
        <source>Enable</source>
        <translation>Включить</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="144"/>
        <source>Daemon version: %1</source>
        <translation>Версия сервиса: %1</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="165"/>
        <source>The D-Bus connection was re-established.</source>
        <translation>Соединение с D-Bus было восстановлено.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="173"/>
        <source>The D-Bus connection was lost, which probably means that the daemon has crashed.</source>
        <translation>Соединение с D-Bus было потерянно. Скорее всего сервис завис или вылетел.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="348"/>
        <location filename="../../src/razergenie.cpp" line="362"/>
        <source>Lighting</source>
        <translation>Анимация</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="364"/>
        <source>Lighting Logo</source>
        <translation>Анимация логотипа</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="366"/>
        <source>Lighting Scroll</source>
        <translation>Анимация колёсика</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="368"/>
        <source>Lighting Backlight</source>
        <translation>Анимация подсветки</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="404"/>
        <source>Brightness</source>
        <translation>Яркость</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="429"/>
        <source>Brightness Logo</source>
        <translation>Яркость логотипа</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="453"/>
        <source>Brightness Scroll</source>
        <translation>Яркость колёсика</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="476"/>
        <source>Brightness Backlight</source>
        <translation>Яркость подсветки</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="515"/>
        <source>Left</source>
        <translation>Лево</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="517"/>
        <source>Right</source>
        <translation>Право</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="541"/>
        <source>Set Logo Active</source>
        <translation>Сделать логотип активным</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="552"/>
        <source>Set Scroll Active</source>
        <translation>Сделать колёсико активным</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="563"/>
        <source>Set Backlight Active</source>
        <translation>Сделать подсветку активной</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="575"/>
        <source>Profile LED %1</source>
        <translation>Профиль LED %1</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="607"/>
        <location filename="../../src/razergenie.cpp" line="686"/>
        <source>DPI</source>
        <translation>DPI</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="614"/>
        <source>DPI X</source>
        <translation>DPI X</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="615"/>
        <source>DPI Y</source>
        <translation>DPI Y</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="636"/>
        <source>Lock X/Y</source>
        <translation>Связать оси X/Y</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="703"/>
        <source>Polling rate</source>
        <translation>Частота опроса</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="720"/>
        <source>Open custom editor</source>
        <translation>Открыть редактор</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="725"/>
        <source>Launch matrix discovery</source>
        <translation>Запуск поиска матрицы</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="736"/>
        <source>Serial number: %1</source>
        <translation>Серийный номер: %1</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="739"/>
        <source>Firmware version: %1</source>
        <translation>Версия прошивки: %1</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="819"/>
        <source>No device was detected</source>
        <translation>Устройства не обнаружены</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="820"/>
        <source>The OpenRazer daemon didn&apos;t detect a device that is supported.
This could also be caused due to a misconfiguration of this PC.</source>
        <translation>Сервис OpenRazer не обнаружил поддерживаемых устройств.
Это так же может быть вызвано неправильной настройкой этого ПК.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="821"/>
        <source>Open supported devices</source>
        <translation>Открыть список поддерживаемых устройств</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="826"/>
        <source>The daemon didn&apos;t detect a device that is connected</source>
        <translation>Сервис не обнаружил присоединенных устройств</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="827"/>
        <source>Linux detected connected devices but the daemon didn&apos;t. This could be either due to a permission problem or a kernel module problem.</source>
        <translation>Linux обнаружил устройства, но сервис - нет. Это может быть проблема с правами или проблема с модулем ядра.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="829"/>
        <source>Open troubleshooting page</source>
        <translation>Открыть страницу проблем</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="848"/>
        <source>Error while syncing devices.</source>
        <translation>Ошибка при синхронизации устройств.</translation>
    </message>
    <message>
        <location filename="../../src/razergenie.cpp" line="854"/>
        <source>Error while toggling &apos;turn off on screensaver&apos;</source>
        <translation>Ошибка при переключении &quot;Выключения при скринсейвере&quot;</translation>
    </message>
</context>
<context>
    <name>RazerGenieUi</name>
    <message>
        <location filename="../../ui/razergenie.ui" line="21"/>
        <source>RazerGenie</source>
        <translation>RazerGenie</translation>
    </message>
    <message>
        <location filename="../../ui/razergenie.ui" line="68"/>
        <source>Preferences</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../../ui/razergenie.ui" line="97"/>
        <source>Sync devices</source>
        <translation>Синхронизация устройств</translation>
    </message>
    <message>
        <location filename="../../ui/razergenie.ui" line="104"/>
        <source>Turn off on screensaver</source>
        <translation>Выключать при скринсейвере</translation>
    </message>
</context>
<context>
    <name>RazerImageDownloader</name>
    <message>
        <location filename="../../src/razerimagedownloader.cpp" line="52"/>
        <source>Image download disabled</source>
        <translation>Загрузка изображений отключена</translation>
    </message>
    <message>
        <location filename="../../src/razerimagedownloader.cpp" line="52"/>
        <source>Image downloading is disabled. Visit the preferences to enable it.</source>
        <translation>Загрузка изображений устройств отключена. Вы можете включить ее в настройках.</translation>
    </message>
    <message>
        <location filename="../../src/razerimagedownloader.cpp" line="77"/>
        <source>Network Error</source>
        <translation>Ошибка сети</translation>
    </message>
</context>
</TS>
