/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Luca Weiss <luca@z3ntu.xyz>
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

#include "devicedelegate.h"
#include <iostream>
#include <QSpinBox>

DeviceDelegate::DeviceDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    std::cout << "CONSTRUCTOR" << std::endl;
}

QWidget *DeviceDelegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem &/* option */,
                                      const QModelIndex &/* index */) const
{
    std::cout << "Create editor" << std::endl;
    QSpinBox *editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0);
    editor->setMaximum(100);

    return editor;
}

void DeviceDelegate::setEditorData(QWidget *editor,
                                   const QModelIndex &index) const
{
    std::cout << "Set editor data" << std::endl;
    // Get the value via index of the Model
    int value = index.model()->data(index, Qt::EditRole).toInt();

    // Put the value into the SpinBox
    QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
    spinbox->setValue(value);
}

void DeviceDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    std::cout << "Set model data" << std::endl;
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}

void DeviceDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    std::cout << "Update editor geometry" << std::endl;
    editor->setGeometry(option.rect);
}
