/************************************************************************
**
**  Copyright (C) 2019 Kevin B. Hendricks
**  Copyright (C) 2012 Dave Heiland
**  Copyright (C) 2012 John Schember <john@nachtimwald.com>
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

#ifndef TOCHTMLWRITER_H
#define TOCHTMLWRITER_H

#include "MainUI/TOCModel.h"

class QXmlStreamWriter;

/**
 * Writes the TOC into an HTML file of the EPUB publication.
 */
class TOCHTMLWriter
{
public:
    TOCHTMLWriter(const QString& toc_bookpath, 
		  const QString& css_bookpath, 
		  TOCModel::TOCEntry toc_root_entry);
    ~TOCHTMLWriter();

    QString WriteXML(const QString &version);

private:
    void WriteHead();
    void WriteBody();
    void WriteEntries(TOCModel::TOCEntry entry, int level = 1);

    QXmlStreamWriter *m_Writer;

    TOCModel::TOCEntry m_TOCRootEntry;
    QString m_TOCBookPath;
    QString m_CSSBookPath;
};

#endif // TOCHTMLWRITER_H
