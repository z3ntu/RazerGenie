// Copyright (C) 2016-2018  Luca Weiss <luca (at) z3ntu (dot) xyz>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.h"
#include "razergenie.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("RazerGenie");
    QApplication::setApplicationVersion(RAZERGENIE_VERSION);
    QApplication::setOrganizationName("razergenie"); // for QSettings

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    QTranslator translator;
    bool ret = translator.load(QLocale::system(), QString(), QString(), QString(RAZERGENIE_DATADIR) + "/translations/");
    qDebug() << "Translation loaded:" << ret;
    app.installTranslator(&translator);

    RazerGenie w;
    w.show();

    return app.exec();
}
