/************************************************************************
**
**  Copyright (C) 2016-2019 Kevin B. Hendricks, Stratford, Ontario, Canada
**  Copyright (C) 2013      Dave Heiland
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#pragma once
#ifndef EDITTOC_H
#define EDITTOC_H

#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtWidgets/QDialog>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

#include "MainUI/TOCModel.h"
#include "BookManipulation/Headings.h"
#include "ResourceObjects/NCXResource.h"

#include "ui_EditTOC.h"

class Book;
class QStandardItem;
class Resource;

class EditTOC : public QDialog
{
    Q_OBJECT

public:

    EditTOC(QSharedPointer<Book> book, QList<Resource *> resources, QWidget *parent = 0);

    ~EditTOC();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void Save();

    void Rename();
    void CollapseAll();
    void ExpandAll();

    void AddEntryAbove();
    void AddEntryBelow();
    void DeleteEntry();
    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDown();
    void SelectTarget();

    void OpenContextMenu(const QPoint &point);

private:
    void AddEntry(bool above);
    QModelIndex CheckSelection(int row);

    TOCModel::TOCEntry ConvertTableToEntries();
    TOCModel::TOCEntry ConvertItemToEntry(QStandardItem *item);

    void BuildModel(const TOCModel::TOCEntry &root_entry);
    void AddEntryToParentItem(const TOCModel::TOCEntry &entry, QStandardItem *parent, int level);

    void ExpandChildren(QStandardItem *item);

    void CreateContextMenuActions();
    void SetupContextMenu(const QPoint &point);

    void UpdateTreeViewDisplay();

    void CreateTOCModel();

    void ReadSettings();
    void WriteSettings();

    void ConnectSignalsToSlots();

    ///////////////////////////////
    // PRIVATE MEMBER VARIABLES
    ///////////////////////////////

    QSharedPointer<Book> m_Book;

    QList<Resource *> m_Resources;

    QStandardItemModel *m_TableOfContents;

    QMenu *m_ContextMenu;

    QAction *m_Rename;
    QAction *m_Delete;
    QAction *m_CollapseAll;
    QAction *m_ExpandAll;
    QAction *m_MoveDown;
    QAction *m_MoveUp;

    TOCModel *m_TOCModel;

    Resource * m_BaseResource;

    Ui::EditTOC ui;
};

#endif // EDITTOC_H
