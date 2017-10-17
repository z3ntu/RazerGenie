/*
 * Copyright (C) 2017  Luca Weiss <luca (at) z3ntu (dot) xyz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "customeditor.h"
#include <QtWidgets>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>

CustomEditor::CustomEditor(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("RazerGenie - Custom Editor");

    parseJSON();

    QVBoxLayout *vbox = new QVBoxLayout(this);
    //TODO: Get physical layout from daemon and use
    QJsonObject keyboardLayout = keys["de_DE"].toObject();

    // Iterate over rows in the object
    QJsonObject::const_iterator it;
    for(it = keyboardLayout.constBegin(); it != keyboardLayout.constEnd(); ++it) {
        QJsonArray row = (*it).toArray();

        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->setAlignment(Qt::AlignLeft);

        // Iterate over keys in row
        QJsonArray::const_iterator jt;
        for(jt = row.constBegin(); jt != row.constEnd(); ++jt) {
            QJsonObject obj = (*jt).toObject();

            if(!obj["label"].isNull()) {
                QPushButton *btn = new QPushButton(obj["label"].toString());
                int width = obj.contains("width") ? obj.value("width").toInt() : 120;
                int height = obj.contains("height") ? obj.value("height").toInt() : 126;
                btn->setFixedSize(width, height);
                hbox->addWidget(btn);
            } else {
                QSpacerItem *spacer = new QSpacerItem(120, 126, QSizePolicy::Fixed, QSizePolicy::Fixed);
                hbox->addItem(spacer);
            }
        }
        vbox->addLayout(hbox);
    }
}

CustomEditor::~CustomEditor()
{

}

void CustomEditor::parseJSON()
{
    //TODO: Write code for release builds (/usr/share/razergenie/data/keyboard_layouts.json maybe?)
    QFile file("../../data/keyboard_layouts.json");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "error", file.errorString());

    QTextStream in(&file);
    QString data = in.readAll();
    file.close();

    keys = QJsonDocument::fromJson(data.toUtf8()).object();
}
