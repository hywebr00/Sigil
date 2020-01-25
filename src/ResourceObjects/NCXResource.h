/************************************************************************
**
**  Copyright (C) 2015-2019 Kevin B. Hendricks, Stratford Ontario Canada 
**  Copyright (C) 2009-2011 Strahinja Markovic  <strahinja.markovic@gmail.com>
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
#ifndef NCXRESOURCE_H
#define NCXRESOURCE_H

#include "MainUI/TOCModel.h"
#include "ResourceObjects/XMLResource.h"

class Book;

class NCXResource : public XMLResource
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param fullfilepath The full path to the file that this
     *                     resource is representing.
     * @param parent The object's parent.
     */
    NCXResource(const QString &mainfolder, 
		const QString &fullfilepath, 
                const QString &version = QString(),
		QObject *parent = NULL);

    // inherited

    virtual bool RenameTo(const QString &new_filename);

    virtual bool MoveTo(const QString &newbookpath);

    virtual ResourceType Type() const;

    void SetMainID(const QString &main_id);

    bool GenerateNCXFromBookContents(const Book *book);
    void GenerateNCXFromTOCContents(const Book *book, TOCModel *toc_model);
    void GenerateNCXFromTOCEntries(const Book *book, TOCModel::TOCEntry toc_root_entry);
    void FillWithDefaultText(const QString &version, const QString &default_text_folder);
    void FillWithDefaultTextToBookPath(const QString &version, const QString &start_bookpath);
};

#endif // NCXRESOURCE_H
