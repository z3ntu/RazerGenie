// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SCROLLWIDGET_H
#define SCROLLWIDGET_H
#include <QWidget>
#include <QDBusObjectPath>
class QDBusInterface;
class QComboBox;
class QCheckBox;
class QLabel;
class ScrollWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollWidget(const QDBusObjectPath &path, QWidget *parent = nullptr);
    static bool isAvailable(const QDBusObjectPath &path);
private:
    void refresh();
    void pollSettings();
    void apply(const QString &method, const QVariant &value);
    QDBusInterface *interface;
    QComboBox *mode = nullptr;
    QCheckBox *smartReel = nullptr;
    QCheckBox *acceleration = nullptr;
    QLabel *status;
    int pollsPending = 0;
    QStringList pollErrors;
    QString lastPollError;
    unsigned int settingsGeneration = 0;
};
#endif
