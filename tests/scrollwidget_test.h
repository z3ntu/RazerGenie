// SPDX-License-Identifier: GPL-3.0-or-later
#include <QObject>
#include <QProcess>
class ScrollWidgetTest : public QObject {
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void openingDoesNotWrite();
    void hardwareModeChangesUpdateWithoutWrites();
    void hiddenTabDoesNotPoll();
    void changesUseCorrectTypes();
    void rejectedWritesRestoreControls();
    void failedReadsDisableControls();
    void partialCapabilities();
    void unsupportedDevice();
    void unavailableChargingIsUnknown();
    void chargingReadFailureIsUnknown();
private:
    void start(const QString &capabilities = QString());
    QProcess mock;
};
