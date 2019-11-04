/************************************************************************
**
**  Copyright (C) 2015-2019 Kevin B. Hendricks, Stratford, Ontario Canada
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

#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtWidgets/QFileIconProvider>

#include "Misc/Utility.h"
#include "ResourceObjects/Resource.h"

const int WAIT_FOR_WRITE_DELAY = 100;

Resource::Resource(const QString &mainfolder, const QString &fullfilepath, QObject *parent)
    :
    QObject(parent),
    m_Identifier(Utility::CreateUUID()),
    m_MainFolder(mainfolder),
    m_FullFilePath(fullfilepath),
    m_LastSaved(0),
    m_LastWrittenTo(0),
    m_LastWrittenSize(0),
    m_CurrentBookRelPath(""),
    m_EpubVersion("2.0"),
    m_MediaType(""),
    m_ReadWriteLock(QReadWriteLock::Recursive)
{
}

bool Resource::operator< (const Resource &other)
{
    return Filename() < other.Filename();
}


QString Resource::GetIdentifier() const
{
    return m_Identifier;
}


QString Resource::Filename() const
{
#if 0  
    // accessing the file system just to extract a file name is very slow
    // especially when we know this resource path ends with a filename
    return QFileInfo(m_FullFilePath).fileName();
#else
    return GetRelativePath().split('/').last();
#endif
}



// relative path of the resource's directory within the EPUB (a book path to the folder)
QString Resource::GetFolder() const
{
    return QFileInfo(GetRelativePath()).path();
}


// Pathname of the file within the EPUB.  Sometimes called the book path
QString Resource::GetRelativePath() const
{
    // Note m_MainFolder *never* ends with a path separator - see Misc/TempFolder.cpp
    return m_FullFilePath.right(m_FullFilePath.length() - m_MainFolder.length() - 1);
}


// Generate a unique path segment ending in file name for this resource
QString Resource::ShortPathName() const
{
    return m_ShortName;
}


// Use to generate relative path **from** some **other** start_resource 
// (OPFResource, NCXResource, NavResource, etc) "to" **this** resource
QString Resource::GetRelativePathFromResource(const Resource* start_resource) const
{
    if (GetRelativePath() == start_resource->GetRelativePath()) return "";
    // return Utility::relativePath(GetRelativePath(), start_resource->GetFolder());
    return Utility::relativePath(m_FullFilePath, start_resource->GetFullFolderPath());
} 


// Use to generate relative path from **this** resource to  some **other** dest_resource
QString Resource::GetRelativePathToResource(const Resource* dest_resource) const
{
    if (GetRelativePath() == dest_resource->GetRelativePath()) return "";
    // return Utility::relativePath(dest_resource->GetRelativePath(), dest_resource->GetFolder());
    return Utility::relativePath(dest_resource->GetFullPath(), GetFullFolderPath());
} 


QString Resource::GetFullPath() const
{
    return m_FullFilePath;
}


QString Resource::GetFullFolderPath() const
{
    return QFileInfo(m_FullFilePath).absolutePath();
}


QUrl Resource::GetBaseUrl() const
{
    return QUrl::fromLocalFile(QFileInfo(m_FullFilePath).absolutePath() + "/");
}


void Resource::SetCurrentBookRelPath(const QString& current_path)
{
    m_CurrentBookRelPath = current_path;
}


QString Resource::GetCurrentBookRelPath()
{
  if (m_CurrentBookRelPath.isEmpty()) {
      return GetRelativePath();
  }
  return m_CurrentBookRelPath;
}

void Resource::SetEpubVersion(const QString& version)
{
    m_EpubVersion = version;
}


QString Resource::GetEpubVersion() const
{
  return m_EpubVersion;
}


void Resource::SetMediaType(const QString& mtype)
{
    m_MediaType = mtype;
}


QString Resource::GetMediaType() const
{
  return m_MediaType;
}


void Resource::SetShortPathName(const QString& shortname)
{
    m_ShortName = shortname;
}


QString Resource::GetFullPathToBookFolder() const
{
   return m_MainFolder;
}


QReadWriteLock &Resource::GetLock() const
{
    return m_ReadWriteLock;
}


QIcon Resource::Icon() const
{
    return QFileIconProvider().icon(QFileInfo(m_FullFilePath));
}


bool Resource::RenameTo(const QString &new_filename)
{
    QString new_path;
    bool successful = false;
    {
        QWriteLocker locker(&m_ReadWriteLock);
        new_path = QFileInfo(m_FullFilePath).absolutePath() + "/" + new_filename;
        successful = Utility::RenameFile(m_FullFilePath, new_path);
    }

    if (successful) {
        QString old_path = m_FullFilePath;
        m_FullFilePath = new_path;
	SetShortPathName(new_filename);
        emit Renamed(this, old_path);
    }

    return successful;
}

bool Resource::MoveTo(const QString &new_bookpath)
{
    QString new_path;
    bool successful = false;
    {
        QWriteLocker locker(&m_ReadWriteLock);
	new_path = GetFullPathToBookFolder() + "/" + new_bookpath;
        successful = Utility::SMoveFile(m_FullFilePath, new_path);
    }

    if (successful) {
        QString old_path = m_FullFilePath;
        m_FullFilePath = new_path;
        emit Moved(this, old_path);
    }

    return successful;
}

bool Resource::Delete()
{
    bool successful = false;
    {
        QWriteLocker locker(&m_ReadWriteLock);
        successful = Utility::SDeleteFile(m_FullFilePath);
    }

    if (successful) {
        emit Deleted(this);
        // try to prevent any resource modified signals from going out
	// while we wait for delete to actually happen
	disconnect(this, 0, 0, 0);
        deleteLater();
    }

    return successful;
}


Resource::ResourceType Resource::Type() const
{
    return Resource::GenericResourceType;
}

bool Resource::LoadFromDisk()
{
    return false;
}

void Resource::SaveToDisk(bool book_wide_save)
{
    const QDateTime lastModifiedDate = QFileInfo(m_FullFilePath).lastModified();

    if (lastModifiedDate.isValid()) {
        m_LastSaved = lastModifiedDate.toMSecsSinceEpoch();
    }
}

void Resource::FileChangedOnDisk()
{
    QFileInfo latestFileInfo(m_FullFilePath);
    const QDateTime lastModifiedDate = latestFileInfo.lastModified();
    m_LastWrittenTo = lastModifiedDate.isValid() ? lastModifiedDate.toMSecsSinceEpoch() : 0;
    m_LastWrittenSize = latestFileInfo.size();
    QTimer::singleShot(WAIT_FOR_WRITE_DELAY, this, SLOT(ResourceFileModified()));
}

void Resource::ResourceFileModified()
{
    QFileInfo newFileInfo(m_FullFilePath);
    const QDateTime lastModifiedDate = newFileInfo.lastModified();
    qint64 latestWrittenTo = lastModifiedDate.isValid() ? lastModifiedDate.toMSecsSinceEpoch() : 0;
    qint64 latestWrittenSize = newFileInfo.size();

    if (latestWrittenTo == m_LastSaved) {
        // The FileChangedOnDisk has triggered even though the data in the file has not changed.
        // This can happen if the FileWatcher is monitoring a file that Sigil has just performed
        // a disk operation with, such as Saving before a Merge. In this circumstance the data
        // loaded in memory by Sigil may be more up to date than that on disk (such as after the
        // merge but before user has chosen to Save) so we want to ignore the file change notification.
        return;
    }

    if ((latestWrittenTo != m_LastWrittenTo) || (latestWrittenSize != m_LastWrittenSize)) {
        // The file is still being written to.
        m_LastWrittenTo = latestWrittenTo;
        m_LastWrittenSize = latestWrittenSize;
        QTimer::singleShot(WAIT_FOR_WRITE_DELAY, this, SLOT(ResourceFileModified()));
    } else {
        if (LoadFromDisk()) {
            // will trigger marking the book as modified
            emit ResourceUpdatedFromDisk(this);
        }

        // will trigger updates in other resources that link to this resource
        emit ResourceUpdatedOnDisk();
    }
}
