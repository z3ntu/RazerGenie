#include "kbdlayout.h"
#include <QDebug>

KbdLayout::KbdLayout()
{
    qDebug() << __PRETTY_FUNCTION__ << " : Constructed()" << endl;
}

KbdLayout::~KbdLayout()
{
    qDebug() << __PRETTY_FUNCTION__ << " : Destroyed()" << endl;
}


void KbdLayout::setKbdLayout(const QJsonObject &langs)
{
    this->mjsKbdLayoutDoc.setObject(langs);
}

void KbdLayout::setKbdLayLangs(const QJsonObject &rows)
{
    QJsonObject newlang;
    newlang.insert(this->mjsLangStr, rows);
    this->mjsLangs = newlang;
}

void KbdLayout::setKbdLayRows(const QJsonObject &row)
{
    QJsonObject newrows;
    newrows.insert(this->mjsRowStr, row);
    this->mjsRows = row;
}

void KbdLayout::initLayout()
{
    this->mjsLangs = this->mjsKbdLayoutDoc[this->mjsLangStr].toObject();
    
    //qDebug() << __PRETTY_FUNCTION__ << " : Rows => " << this->mjsRows;
    
    for(int i = 0; i < 6; i++)
    {
        this->mjsRows.insert(this->mjsRowStr+QString::number(i), this->mjsLangs.value(this->mjsRowStr+QString::number(i)) );
    }
    //qDebug() << __PRETTY_FUNCTION__ << " : Lang contents => " << this->mjsLangs << endl; 
    //qDebug() << __PRETTY_FUNCTION__ << " : Rows contents => " << this->mjsRows << endl; 
}

void KbdLayout::updateLayout()
{
    this->setKbdLayLangs(this->mjsRows);
    this->setKbdLayout(this->mjsLangs);
}

void KbdLayout::openKbdLayout(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    this->mjsKbdLayoutDoc = QJsonDocument::fromJson(file.readAll());
    //qDebug() << __PRETTY_FUNCTION__ << " : File content => " << this->mjsKbdLayoutDoc << endl;
    file.close();
    this->initLayout();
}

void KbdLayout::saveKbdLayout(const QString &filename)
{
    this->updateLayout();
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    //qDebug() << "JSON datas to write : " << this->mjsKbdLayoutDoc;
    file.write(this->mjsKbdLayoutDoc.toJson());
    file.close();
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
