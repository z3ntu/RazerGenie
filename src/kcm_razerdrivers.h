#ifndef KCM_RAZERDRIVERS_H
#define KCM_RAZERDRIVERS_H

#include <kcmodule.h>
#include "ui_kcm_razerdrivers.h"

class kcm_razerdrivers : public KCModule
{
    Q_OBJECT
public:
    kcm_razerdrivers(QWidget* parent, const QVariantList& args);
    ~kcm_razerdrivers();
private:
    Ui::RazerDriversKcmUi ui;
};

#endif
