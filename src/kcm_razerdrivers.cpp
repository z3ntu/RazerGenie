#include <iostream>
#include <KPluginFactory>
#include "kcm_razerdrivers.h"
#include <kdeversion.h>
#include <KAboutData>
#include <KI18n/KLocalizedString>
#include <config.h>

K_PLUGIN_FACTORY(RazerDriversKcmFactory, registerPlugin<kcm_razerdrivers>();)

kcm_razerdrivers::kcm_razerdrivers(QWidget* parent, const QVariantList& args) : KCModule(parent, args)
{
    // About dialog
    KAboutData *about = new KAboutData(
        "kcm_razerdrivers",
        "RazerDrivers",
        KCM_RAZERDRIVERS_VERSION,
        QString("A KDE system settings module for managing razer devices."),
        KAboutLicense::GPL_V3,
        "Copyright (C) 2016 Luca Weiss",
        QString(),
        "https://github.com/z3ntu/kcm_razerdrivers",
        "https://github.com/z3ntu/kcm_razerdrivers/issues");
    about->addAuthor("Luca Weiss", "Main Developer", "luca@z3ntu.xyz");
    setAboutData(about);
    
    ui.setupUi(this);

}

kcm_razerdrivers::~kcm_razerdrivers()
{
}

#include "kcm_razerdrivers.moc"
