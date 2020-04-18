#ifndef KBDLAYOUT_H
#define KBDLAYOUT_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class KbdLayout
{
public:
    QString mjsLangStr      = "fr_FR";
    QString mjsRowStr       = "row";
    QString mjsLabelStr     = "label";
    QString mjsMatrixStr    = "matrix";
    QString mjsColorsStr    = "colors";
    
    KbdLayout();
    ~KbdLayout();
    
    void setKbdLayout(const QJsonObject &langs);
    void setKbdLayLangs(const QJsonObject &rows);
    void setKbdLayRows(const QJsonObject &row);
    
    void initLayout();
    void updateLayout();
    
    void openKbdLayout(const QString &filename);
    void saveKbdLayout(const QString &filename);
    
    QJsonDocument getKbdLayout();
    QJsonObject   getKbdLayLangs();
    QJsonObject   getKbdLayRows();
    
private:
    QJsonDocument mjsKbdLayoutDoc;
    QJsonObject   mjsLangs;
    QJsonObject   mjsRows;
};

#endif // KBDLAYOUT_H
