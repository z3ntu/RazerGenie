#include "kbdlayout.h"
#include <QDebug>

KbdLayout::KbdLayout()
{
    //qDebug() << __FUNCTION__ << " : Constructed()" << endl;
}

KbdLayout::~KbdLayout()
{
    //qDebug() << __FUNCTION__ << " : Destroyed()" << endl;
}

void KbdLayout::setKbdLayout(const QJsonObject &langs)
{
    this->mjsKbdLayoutDoc.setObject(langs);
    qDebug() << __FUNCTION__ << ": JSON keyboard layout document created";
}

void KbdLayout::setKbdLayLangs(const QJsonObject &rows)
{
    QJsonObject newlang;
    newlang.insert(this->mjsLangStr, rows);
    this->mjsLangs = newlang;
    qDebug() << __FUNCTION__ << ": JSON keyboard lang layout container filled";
}

void KbdLayout::setKbdLayRows(const QJsonObject &row)
{
    QJsonObject newrows;
    newrows.insert(this->mjsRowStr, row);
    this->mjsRows = row;
    qDebug() << __FUNCTION__ << ": JSON keyboard row inserted into rowS container";
}

void KbdLayout::setParsedKeys(const QJsonObject &keys)
{
    this->mjsKeys = keys;
}

void KbdLayout::initLayout()
{
    this->mjsLangs = this->mjsKbdLayoutDoc[this->mjsLangStr].toObject();
    
    //qDebug() << __FUNCTION__ << " : Rows => " << this->mjsRows;
    
    for(int i = 0; i < 6; i++)
    {
        this->mjsRows.insert(this->mjsRowStr+QString::number(i), this->mjsLangs.value(this->mjsRowStr+QString::number(i)) );
    }
    //qDebug() << __FUNCTION__ << " : Lang contents => " << this->mjsLangs << endl; 
    //qDebug() << __FUNCTION__ << " : Rows contents => " << this->mjsRows << endl; 
    qDebug() << __FUNCTION__ << ": JSON keyboard layout document Initialised";
}

void KbdLayout::updateLayout()
{
    this->setKbdLayLangs(this->mjsRows);
    this->setKbdLayout(this->mjsLangs);
    qDebug() << __FUNCTION__ << ": JSON Keyboard layout document updated";
}

void KbdLayout::openKbdLayout(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    this->mjsKbdLayoutDoc = QJsonDocument::fromJson(file.readAll());
    //qDebug() << __FUNCTION__ << " : File content => " << this->mjsKbdLayoutDoc << endl;
    file.close();
    this->initLayout();
    qDebug() << __FUNCTION__ << ": Grabbed successfully JSON datas from file : " << filename;
}

void KbdLayout::saveKbdLayout(const QString &filename)
{
    this->updateLayout();
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(this->mjsKbdLayoutDoc.toJson());
    file.close();
    qDebug() << __FUNCTION__ << ": Saved successfully JSON datas to file : " << filename;
}

QJsonDocument KbdLayout::getKbdLayout()
{
    return this->mjsKbdLayoutDoc;
}

QJsonObject KbdLayout::getKbdLayLangs()
{
    return this->mjsLangs;
}

QJsonObject KbdLayout::getKbdLayRows()
{
    return this->mjsRows;
}
