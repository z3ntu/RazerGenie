#include "kbdlayout.h"
#include <QDebug>

KbdLayout::KbdLayout()
{
    
}

KbdLayout::~KbdLayout()
{
    
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
    this->mjsLangs = this->mjsKbdLayoutDoc.object();
    this->mjsRows = this->mjsLangs.value(this->mjsLangStr).toObject();
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
    file.close();
    this->initLayout();
}

void KbdLayout::saveKbdLayout(const QString &filename)
{
    this->updateLayout();
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    qDebug() << "JSON to write : " << this->mjsKbdLayoutDoc;
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
