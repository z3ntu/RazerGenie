// SPDX-License-Identifier: GPL-3.0-or-later
#include "scrollwidget.h"
#include "devicecapabilities.h"
#include <QCheckBox>
#include <QAbstractItemView>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QTimer>
#include <QComboBox>
#include <QDBusInterface>
#include <QDBusReply>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>
namespace {
bool supported(const QSet<QString> &methods, const QString &name)
{
    return methods.contains("getScroll" + name) && methods.contains("setScroll" + name);
}
}
bool ScrollWidget::isAvailable(const QDBusObjectPath &path)
{
    auto methods = deviceMethods(path, "razer.device.scroll");
    return supported(methods, "Mode") || supported(methods, "SmartReel") || supported(methods, "Acceleration");
}
ScrollWidget::ScrollWidget(const QDBusObjectPath &path, QWidget *parent) : QWidget(parent)
{
    interface = new QDBusInterface("org.razer", path.path(), "razer.device.scroll", QDBusConnection::sessionBus(), this);
    auto methods = deviceMethods(path, "razer.device.scroll");
    auto *layout = new QVBoxLayout(this);
    auto *header = new QLabel(tr("Scroll wheel"), this);
    auto font = header->font(); font.setPointSize(15); font.setBold(true); header->setFont(font);
    layout->addWidget(header);
    if (supported(methods, "Mode")) {
        layout->addWidget(new QLabel(tr("Scrolling mode"), this));
        mode = new QComboBox(this);
        mode->setObjectName("scrollMode");
        mode->addItem(tr("Tactile"), 0);
        mode->addItem(tr("Free-spin"), 1);
        layout->addWidget(mode);
    }
    if (supported(methods, "SmartReel")) {
        smartReel = new QCheckBox(tr("Smart-Reel"), this);
        smartReel->setObjectName("scrollSmartReel");
        smartReel->setToolTip(tr("Automatically switches to free-spin when you scroll quickly, then returns to tactile mode."));
        layout->addWidget(smartReel);
    }
    if (supported(methods, "Acceleration")) {
        acceleration = new QCheckBox(tr("Scroll acceleration"), this);
        acceleration->setObjectName("scrollAcceleration");
        acceleration->setToolTip(tr("Increases scrolling distance when you scroll faster."));
        layout->addWidget(acceleration);
    }
    if (smartReel) {
        auto *hint = new QLabel(tr("For constant free-spin, turn Smart-Reel off."), this);
        hint->setWordWrap(true); layout->addWidget(hint);
    }
    status = new QLabel(this); status->setObjectName("scrollStatus"); status->setWordWrap(true); layout->addWidget(status);
    auto *refreshButton = new QPushButton(tr("Refresh"), this);
    refreshButton->setObjectName("refreshScrollSettings"); layout->addWidget(refreshButton);
    layout->addStretch();
    refresh(); // Read actual values before connecting signals; opening the page sends no writes.
    if (mode) connect(mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        apply("setScrollMode", QVariant::fromValue(static_cast<uchar>(mode->currentData().toUInt())));
    });
    if (smartReel) connect(smartReel, &QCheckBox::clicked, this, [this](bool value) { apply("setScrollSmartReel", value); });
    if (acceleration) connect(acceleration, &QCheckBox::clicked, this, [this](bool value) { apply("setScrollAcceleration", value); });
    connect(refreshButton, &QPushButton::clicked, this, [this] { status->clear(); refresh(); });
    auto *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, [this] { pollSettings(); });
    timer->start();
}
void ScrollWidget::refresh()
{
    ++settingsGeneration;
    QStringList errors;
    if (mode) {
        QSignalBlocker blocker(mode);
        QDBusReply<uchar> reply = interface->call("getScrollMode");
        bool valid = reply.isValid() && reply.value() <= 1;
        mode->setEnabled(valid); mode->setCurrentIndex(valid ? mode->findData(reply.value()) : -1);
        if (!valid) errors.append(tr("Unable to read scrolling mode."));
    }
    for (const auto &entry : {qMakePair(smartReel, QString("getScrollSmartReel")), qMakePair(acceleration, QString("getScrollAcceleration"))}) {
        if (!entry.first) continue;
        QSignalBlocker blocker(entry.first);
        QDBusReply<bool> reply = interface->call(entry.second);
        entry.first->setEnabled(reply.isValid()); entry.first->setChecked(reply.isValid() && reply.value());
        if (!reply.isValid()) errors.append(tr("Unable to read %1.").arg(entry.first->text()));
    }
    if (!errors.isEmpty()) status->setText(errors.join("\n"));
}
void ScrollWidget::apply(const QString &method, const QVariant &value)
{
    ++settingsGeneration;
    QDBusReply<void> reply = interface->call(method, value);
    status->clear();
    if (!reply.isValid()) status->setText(tr("Could not apply scroll setting: %1").arg(reply.error().message()));
    refresh(); // Restore controls to hardware readback, including effects of Smart-Reel.
}

void ScrollWidget::pollSettings()
{
    if (!isVisible() || window()->isMinimized() || pollsPending || (mode && mode->view()->isVisible()))
        return;
    QList<QPair<QWidget *, QString>> requests;
    if (mode) requests.append({mode, "getScrollMode"});
    if (smartReel) requests.append({smartReel, "getScrollSmartReel"});
    if (acceleration) requests.append({acceleration, "getScrollAcceleration"});
    pollsPending = requests.size();
    pollErrors.clear();
    const auto generation = settingsGeneration;
    for (const auto &request : requests) {
        auto *control = request.first;
        auto *watcher = new QDBusPendingCallWatcher(interface->asyncCall(request.second), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, generation, control](QDBusPendingCallWatcher *call) {
            --pollsPending;
            call->deleteLater();
            // Do not replace newer manual changes, hidden controls or an open menu.
            if (generation != settingsGeneration || !isVisible() || window()->isMinimized() || (mode && mode->view()->isVisible()))
                return;
            QSignalBlocker blocker(control);
            if (control == mode) {
                QDBusPendingReply<uchar> reply = *call;
                const bool valid = reply.isValid() && reply.value() <= 1;
                mode->setEnabled(valid);
                mode->setCurrentIndex(valid ? mode->findData(reply.value()) : -1);
                if (!valid) pollErrors.append(tr("Unable to read scrolling mode."));
            } else {
                auto *checkbox = static_cast<QCheckBox *>(control);
                QDBusPendingReply<bool> reply = *call;
                checkbox->setEnabled(reply.isValid());
                checkbox->setChecked(reply.isValid() && reply.value());
                if (!reply.isValid()) pollErrors.append(tr("Unable to read %1.").arg(checkbox->text()));
            }
            if (!pollsPending) {
                // Background reads can clear their own errors, but preserve write errors.
                const auto error = pollErrors.join("\n");
                if (status->text().isEmpty() || status->text() == lastPollError)
                    status->setText(error);
                lastPollError = error;
            }
        });
    }
}
