/************************************************************************
**
**  Copyright (C) 2016 Kevin B. Hendricks, Stratford, ON, Canada
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

#include <QtCore/QDate>
#include <QtCore/QModelIndex>

#include "Dialogs/AddSemantics.h"
#include "Misc/SettingsStore.h"
#include "Misc/Utility.h"

static const QString SETTINGS_GROUP = "add_semantics";

AddSemantics::AddSemantics(const QHash<QString, DescriptiveInfo> &infomap, const QString & current_code, QWidget *parent)
    :
    QDialog(parent),
    m_SemanticsInfo(infomap)
{
    ui.setupUi(this);

    connect(ui.lwProperties, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this,	          SLOT(UpdateDescription(QListWidgetItem *)));
    connect(this, SIGNAL(accepted()), this, SLOT(WriteSettings()));
    connect(ui.lwProperties, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(accept()));

    // Fill the dialog with sorted translated semantics property names
    QStringList names;
    foreach (QString code, m_SemanticsInfo.keys()) {
        QString name = m_SemanticsInfo.value(code, DescriptiveInfo()).name;
        // add a space and check mark to the end of the name if current semantic set 
        if (code == current_code) {
	    name.append(' ');
            name.append(QChar(10003));
	}
        m_Name2Code[name] = code;
        names.append(name);
    }
    names = Utility::LocaleAwareSort(names);

    foreach(QString name, names) {
        ui.lwProperties->addItem(name);
    }
    ReadSettings();
}

void AddSemantics::UpdateDescription(QListWidgetItem *current)
{
    QString text;
    QString code = m_Name2Code.value(current->text(), QString());
    if (!code.isEmpty()) {
        text = m_SemanticsInfo.value(code, DescriptiveInfo() ).description;
    }
    if (!text.isEmpty()) {
        ui.lbDescription->setText(text);
    }
}

QStringList AddSemantics::GetSelectedEntries()
{
    return m_SelectedEntries;
}

void AddSemantics::SaveSelection()
{
    m_SelectedEntries.clear();
    foreach(QListWidgetItem * item, ui.lwProperties->selectedItems()) {
        QString code = m_Name2Code.value(item->text(), QString() );
        m_SelectedEntries.append(code);
    }
}


void AddSemantics::ReadSettings()
{
    SettingsStore settings;
    settings.beginGroup(SETTINGS_GROUP);
    // The size of the window and it's full screen status
    QByteArray geometry = settings.value("geometry").toByteArray();

    if (!geometry.isNull()) {
        restoreGeometry(geometry);
    }

    QByteArray splitter_position = settings.value("splitter").toByteArray();

    if (!splitter_position.isNull()) {
        ui.splitter->restoreState(splitter_position);
    }

    settings.endGroup();
}


void AddSemantics::WriteSettings()
{
    SaveSelection();
    SettingsStore settings;
    settings.beginGroup(SETTINGS_GROUP);
    // The size of the window and it's full screen status
    settings.setValue("geometry", saveGeometry());
    // The position of the splitter handle
    settings.setValue("splitter", ui.splitter->saveState());
    settings.endGroup();
}
