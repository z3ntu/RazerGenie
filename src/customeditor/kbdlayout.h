#ifndef KBDLAYOUT_H
#define KBDLAYOUT_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class KbdLayout
{
public:
    QString mjsLangStr            = "en_US";
    const QString mjsRowStr       = "row";
    const QString mjsLabelStr     = "label";
    const QString mjsMatrixStr    = "matrix";
    const QString mjsColorsStr    = "colors";
    const QString mjsWidthStr     = "width";
    const QString mjsDisabledStr  = "disabled";
    const QString mjsDefColor     = "#000000";
    
    KbdLayout();
    ~KbdLayout();
    
    void setKbdLayout(const QJsonObject &langs);
    void setKbdLayLangs(const QJsonObject &rows);
    void setKbdLayRows(const QJsonObject &row);
    void setParsedKeys(const QJsonObject &keys);
    
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
    QJsonObject   mjsKeys;
};

#endif // KBDLAYOUT_H
