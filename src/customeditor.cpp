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

    if(!parseJSON()) {
        qDebug() << "closing window!!!"; // doesn't work
        setAttribute(Qt::WA_DeleteOnClose);
        qDebug() << this->close();
    }

    QVBoxLayout *vbox = new QVBoxLayout(this);
    //TODO: Get physical layout from daemon and use
    QJsonObject keyboardLayout = keys["de_DE"].toObject();

    // Iterate over rows in the object
    QJsonObject::const_iterator it;
    for(it = keyboardLayout.constBegin(); it != keyboardLayout.constEnd(); ++it) {
        QJsonArray row = (*it).toArray();

        QHBoxLayout *hbox = new QHBoxLayout();
        hbox->setAlignment(Qt::AlignLeft);

//         hbox->setSpacing(2);
//         qDebug() << hbox->contentsMargins();

        // Iterate over keys in row
        QJsonArray::const_iterator jt;
        for(jt = row.constBegin(); jt != row.constEnd(); ++jt) {
            QJsonObject obj = (*jt).toObject();

            if(!obj["label"].isNull()) {
                QPushButton *btn = new QPushButton(obj["label"].toString());
                int width = obj.contains("width") ? obj.value("width").toInt() : 60;
                int height = /*obj.contains("height") ? obj.value("height").toInt() : */63;
                btn->setFixedSize(width, height);
                /*if(obj["cut"] == "enter") {
                    QPixmap pixmap("../../data/de_DE_mask.png");
                    btn->setMask(pixmap.mask());
                }*/
                hbox->addWidget(btn);
            } else {
                QSpacerItem *spacer = new QSpacerItem(66, 69, QSizePolicy::Fixed, QSizePolicy::Fixed);
                hbox->addItem(spacer);
            }
        }
        vbox->addLayout(hbox);
    }
}

CustomEditor::~CustomEditor()
{

}

bool CustomEditor::parseJSON()
{
    QFile *file; // Pointer to file object to use
    QFile file_devel("../../data/keyboard_layouts.json"); // File during developemnt
    QFile file_prod("../share/razergenie/keyboard_layouts.json"); // File for production

    // Try to open the dev file (higher priority)
    if(file_devel.open(QIODevice::ReadOnly)) {
        qDebug() << "RazerGenie: Using the development keyboard_layouts.json file.";
        file = &file_devel;
    } else {
        qDebug() << "RazerGenie: Development keyboard_layouts.json failed to open. Trying the production location. Error: " << file_devel.errorString();

        // Try to open the production file
        if(file_prod.open(QIODevice::ReadOnly)) {
            file = &file_prod;
        } else {
            QMessageBox::information(0, "Error loading keyboard_layouts.json!", "The file keyboard_layouts.json, used for the custom editor failed to load: " + file_prod.errorString() + "\nThe editor won't open now.");
            return false;
        }
    }

    // Read the file
    QTextStream in(file);
    QString data = in.readAll();
    file->close();

    // Convert it to a QJsonObject
    keys = QJsonDocument::fromJson(data.toUtf8()).object();

    return true;
}
