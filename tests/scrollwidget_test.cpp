// SPDX-License-Identifier: GPL-3.0-or-later
#include "scrollwidget_test.h"
#include "devicewidget/scrollwidget.h"
#include "devicewidget/powerwidget.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDBusInterface>
#include <QDBusReply>
#include <QLabel>
#include <QPushButton>
#include <QtTest>
static QDBusObjectPath path("/org/razer/device/test");
void ScrollWidgetTest::start(const QString &capabilities) {
    QStringList args{MOCK_PATH}; if (!capabilities.isEmpty()) args << capabilities;
    mock.start(TEST_PYTHON, args);
    QVERIFY(mock.waitForStarted());
    QVERIFY2(mock.waitForReadyRead(5000), mock.readAllStandardError().constData());
    QVERIFY2(mock.readAllStandardOutput().contains("READY"), mock.readAllStandardError().constData());
}
void ScrollWidgetTest::init() { start(); QVERIFY(ScrollWidget::isAvailable(path)); }
void ScrollWidgetTest::cleanup() { mock.terminate(); QVERIFY(mock.waitForFinished()); }
static int writes() {
    QDBusInterface service("org.razer", path.path(), "org.razer.Test");
    QDBusReply<int> reply = service.call("Writes"); return reply.isValid() ? reply.value() : -1;
}
void ScrollWidgetTest::openingDoesNotWrite() {
    QVERIFY(ScrollWidget::isAvailable(path));
    ScrollWidget widget(path);
    QCOMPARE(widget.findChild<QComboBox *>("scrollMode")->currentData().toInt(), 1);
    QVERIFY(!widget.findChild<QCheckBox *>("scrollSmartReel")->isChecked());
    QVERIFY(widget.findChild<QCheckBox *>("scrollAcceleration")->isChecked());
    QCOMPARE(writes(), 0);
    const auto screenshot = qEnvironmentVariable("RAZERGENIE_TEST_SCREENSHOT");
    if (!screenshot.isEmpty()) {
        widget.resize(480, 280); widget.show(); QTest::qWait(30);
        QVERIFY(widget.grab().save(screenshot));
    }
}
void ScrollWidgetTest::hardwareModeChangesUpdateWithoutWrites() {
    ScrollWidget widget(path); widget.show();
    auto *combo = widget.findChild<QComboBox *>("scrollMode");
    QDBusInterface service("org.razer", path.path(), "org.razer.Test");
    service.call("HardwareMode", QVariant::fromValue(static_cast<uchar>(0)));
    QTRY_COMPARE_WITH_TIMEOUT(combo->currentData().toInt(), 0, 3500);
    service.call("HardwareMode", QVariant::fromValue(static_cast<uchar>(1)));
    QTRY_COMPARE_WITH_TIMEOUT(combo->currentData().toInt(), 1, 3500);
    auto *smart = widget.findChild<QCheckBox *>("scrollSmartReel");
    auto *acceleration = widget.findChild<QCheckBox *>("scrollAcceleration");
    service.call("HardwareSettings", QVariant::fromValue(static_cast<uchar>(0)), true, false);
    QTRY_COMPARE_WITH_TIMEOUT(combo->currentData().toInt(), 0, 3500);
    QTRY_VERIFY_WITH_TIMEOUT(smart->isChecked(), 3500);
    QTRY_VERIFY_WITH_TIMEOUT(!acceleration->isChecked(), 3500);
    QCOMPARE(writes(), 0);
    service.call("Fail", true, false);
    QTRY_VERIFY_WITH_TIMEOUT(!combo->isEnabled(), 3500);
    QTRY_VERIFY_WITH_TIMEOUT(!smart->isEnabled() && !acceleration->isEnabled(), 3500);
    service.call("Fail", false, false);
    QTRY_VERIFY_WITH_TIMEOUT(combo->isEnabled(), 3500);
    QTRY_VERIFY_WITH_TIMEOUT(smart->isEnabled() && acceleration->isEnabled(), 3500);
    QCOMPARE(writes(), 0);
}
void ScrollWidgetTest::hiddenTabDoesNotPoll() {
    ScrollWidget widget(path);
    QDBusInterface service("org.razer", path.path(), "org.razer.Test");
    auto reads = [&service]() { return QDBusReply<int>(service.call("Reads")).value(); };
    const auto initial = reads();
    QTest::qWait(1250);
    QCOMPARE(reads(), initial);
    widget.show();
    QTRY_VERIFY_WITH_TIMEOUT(reads() > initial, 3500);
    widget.hide(); QTest::qWait(100);
    const auto hidden = reads();
    QTest::qWait(1250);
    QCOMPARE(reads(), hidden);
    QCOMPARE(writes(), 0);
}
void ScrollWidgetTest::changesUseCorrectTypes() {
    ScrollWidget widget(path);
    widget.findChild<QComboBox *>("scrollMode")->setCurrentIndex(0);
    widget.findChild<QCheckBox *>("scrollSmartReel")->click();
    widget.findChild<QCheckBox *>("scrollAcceleration")->click();
    QCOMPARE(writes(), 3);
    QCOMPARE(widget.findChild<QComboBox *>("scrollMode")->currentData().toInt(), 0);
    QVERIFY(widget.findChild<QCheckBox *>("scrollSmartReel")->isChecked());
    QVERIFY(!widget.findChild<QCheckBox *>("scrollAcceleration")->isChecked());
    QVERIFY(widget.findChild<QLabel *>("scrollStatus")->text().isEmpty());
}
void ScrollWidgetTest::rejectedWritesRestoreControls() {
    ScrollWidget widget(path);
    QDBusInterface service("org.razer", path.path(), "org.razer.Test"); service.call("Fail", false, true);
    widget.findChild<QCheckBox *>("scrollAcceleration")->click();
    QVERIFY(widget.findChild<QCheckBox *>("scrollAcceleration")->isChecked());
    QCOMPARE(writes(), 1);
    QVERIFY(!widget.findChild<QLabel *>("scrollStatus")->text().isEmpty());
}
void ScrollWidgetTest::failedReadsDisableControls() {
    ScrollWidget widget(path);
    QDBusInterface service("org.razer", path.path(), "org.razer.Test"); service.call("Fail", true, false);
    widget.findChild<QPushButton *>("refreshScrollSettings")->click();
    QVERIFY(!widget.findChild<QComboBox *>("scrollMode")->isEnabled());
    QVERIFY(!widget.findChild<QCheckBox *>("scrollSmartReel")->isEnabled());
    QVERIFY(!widget.findChild<QLabel *>("scrollStatus")->text().isEmpty());
    QCOMPARE(writes(), 0);
    service.call("Fail", false, false); widget.findChild<QPushButton *>("refreshScrollSettings")->click();
    QVERIFY(widget.findChild<QComboBox *>("scrollMode")->isEnabled());
    QVERIFY(widget.findChild<QLabel *>("scrollStatus")->text().isEmpty());
}
void ScrollWidgetTest::partialCapabilities() {
    mock.terminate(); QVERIFY(mock.waitForFinished()); start("mode");
    QVERIFY(ScrollWidget::isAvailable(path)); ScrollWidget widget(path);
    QVERIFY(widget.findChild<QComboBox *>("scrollMode"));
    QVERIFY(!widget.findChild<QCheckBox *>("scrollSmartReel"));
    QVERIFY(!widget.findChild<QCheckBox *>("scrollAcceleration"));
    QCOMPARE(writes(), 0);
}
void ScrollWidgetTest::unsupportedDevice() {
    mock.terminate(); QVERIFY(mock.waitForFinished()); start("none");
    QVERIFY(!ScrollWidget::isAvailable(path));
}
void ScrollWidgetTest::unavailableChargingIsUnknown() {
    libopenrazer::openrazer::Device device(path);
    PowerWidget widget(&device);
    QCOMPARE(widget.findChild<QLabel *>("chargingStatus")->text(), QString("Charging status unknown"));
    QCOMPARE(writes(), 0);
}
void ScrollWidgetTest::chargingReadFailureIsUnknown() {
    mock.terminate(); QVERIFY(mock.waitForFinished()); start("charging");
    libopenrazer::openrazer::Device device(path);
    PowerWidget known(&device);
    QCOMPARE(known.findChild<QLabel *>("chargingStatus")->text(), QString("Charging"));
    QDBusInterface service("org.razer", path.path(), "org.razer.Test"); service.call("Fail", true, false);
    PowerWidget unknown(&device);
    QCOMPARE(unknown.findChild<QLabel *>("chargingStatus")->text(), QString("Charging status unknown"));
}
QTEST_MAIN(ScrollWidgetTest)
