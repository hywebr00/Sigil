#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:ts=4:sw=4:softtabstop=4:smarttab:expandtab

import sys
import os

from urllib.parse import unquote
from urllib.parse import urlsplit

# default to using the preferred media-types ffrom the epub 3.2 spec
# https://www.w3.org/publishing/epub3/epub-spec.html#sec-cmt-supported

ext_mime_map = {
    '.bm'    : 'image/bmp',
    '.bmp'   : 'image/bmp',
    '.css'   : 'text/css',
    '.epub'  : 'application/epub+zip',
    '.gif'   : 'image/gif',
    '.htm'   : 'application/xhtml+xml',
    '.html'  : 'application/xhtml+xml',
    '.jpeg'  : 'image/jpeg',
    '.jpg'   : 'image/jpeg',
    '.js'    : 'application/javascript',
    '.m4a'   : 'audio/mp4',
    '.m4v'   : 'video/mp4',
    '.mp3'   : 'audio/mpeg',
    '.mp4'   : 'video/mp4',
    '.ncx'   : 'application/x-dtbncx+xml',
    '.oga'   : 'audio/ogg',
    '.ogg'   : 'audio/ogg',
    '.ogv'   : 'video/ogg',
    '.opf'   : 'application/oebps-package+xml',
    '.otf'   : 'font/otf',
    '.pls'   : 'application/pls+xml',
    '.png'   : 'image/png',
    '.smil'  : 'application/smil+xml',
    '.svg'   : 'image/svg+xml',
    '.tif'   : 'image/tiff',
    '.tiff'  : 'image/tiff',
    '.ttc'   : 'font/collection',
    '.ttf'   : 'font/ttf',
    '.ttml'  : 'application/ttml+xml',
    '.txt'   : 'text/plain',
    '.vtt'   : 'text/vtt',
    '.webm'  : 'video/webm',
    '.webp'  : 'image/webp',
    '.woff'  : 'font/woff',
    '.woff2' : 'font/woff2',
    '.xhtml' : 'application/xhtml+xml',
    '.xml'   : 'application/oebps-page-map+xml',
    '.xpgt'  : 'application/vnd.adobe-page-template+xml',
    #'.js'    : = "text/javascript',
    #'.otf'   : 'application/x-font-opentype',
    #'.otf'   : 'application/font-sfnt',
}


# deprecated font mediatypes
# See https://www.iana.org/assignments/media-types/media-types.xhtml#font

mime_group_map = {
    'image/jpeg'                              : 'Images',
    'image/png'                               : 'Images',
    'image/gif'                               : 'Images',
    'image/svg+xml'                           : 'Images',
    'image/bmp'                               : 'Images', # not a core media type
    'image/tiff'                              : 'Images', # not a core media type
    'image/webp'                              : 'Images', # not a core media type
    'text/html'                               : 'Text',
    'application/xhtml+xml'                   : 'Text',
    'application/x-dtbook+xml'                : 'Text',
    'font/woff2'                              : 'Fonts',
    'font/woff'                               : 'Fonts',
    'font/ttf'                                : 'Fonts',
    'font/otf'                                : 'Fonts',
    'font/sfnt'                               : 'Fonts',
    'font/collection'                         : 'Fonts',
    'application/vnd.ms-opentype'             : 'Fonts',
    'application/font-sfnt'                   : 'Fonts',  # deprecated
    'application/font-ttf'                    : 'Fonts',  # deprecated
    'application/font-otf'                    : 'Fonts',  # deprecated
    'application/font-woff'                   : 'Fonts',  # deprecated
    'application/font-woff2'                  : 'Fonts',  # deprecated
    'application/x-font-ttf'                  : 'Fonts',  # deprecated
    'application/x-truetype-font'             : 'Fonts',  # deprecated
    'application/x-opentype-font'             : 'Fonts',  # deprecated
    'application/x-font-ttf'                  : 'Fonts',  # deprecated
    'application/x-font-otf'                  : 'Fonts',  # deprecated
    'application/x-font-opentype'             : 'Fonts',  # deprecated
    'application/x-font-truetype'             : 'Fonts',  # deprecated
    'application/x-font-truetype-collection'  : 'Fonts',  # deprecated
    'audio/mpeg'                              : 'Audio',
    'audio/mp3'                               : 'Audio',
    'audio/mp4'                               : 'Audio',
    'audio/ogg'                               : 'Audio',  # not a core media type 
    'video/mp4'                               : 'Video',
    'video/ogg'                               : 'Video',
    'video/webm'                              : 'Video',
    'text/vtt'                                : 'Video',
    'application/ttml+xml'                    : 'Video',
    'text/css'                                : 'Styles',
    'application/x-dtbncx+xml'                : 'ncx',
    'application/oebps-package+xml'           : 'opf',
    'application/oebps-page-map+xml'          : 'Misc',
    'application/vnd.adobe-page-map+xml'      : 'Misc',
    'application/vnd.adobe.page-map+xml'      : 'Misc',
    'application/smil+xml'                    : 'Misc',
    'application/adobe-page-template+xml'     : 'Misc',
    'application/vnd.adobe-page-template+xml' : 'Misc',
    'text/javascript'                         : 'Misc',
    'application/javascript'                  : 'Misc',
    'application/pls+xml'                     : 'Misc',
    'text/plain'                              : 'Misc',
}


ASCII_CHARS   = set(chr(x) for x in range(128))
URL_SAFE      = set('ABCDEFGHIJKLMNOPQRSTUVWXYZ'
                    'abcdefghijklmnopqrstuvwxyz'
                    '0123456789' '#' '_.-/~')
IRI_UNSAFE = ASCII_CHARS - URL_SAFE

# returns a quoted IRI (not a URI)                                                                                     
def quoteurl(href):
    if isinstance(href,bytes):
        href = href.decode('utf-8')
    (scheme, netloc, path, query, fragment) = urlsplit(href, scheme="", allow_fragments=True)
    if scheme != "":
        scheme += "://"
        href = href[len(scheme):]
    result = []
    for char in href:
        if char in IRI_UNSAFE:
            char = "%%%02x" % ord(char)
        result.append(char)
    return scheme + ''.join(result)

# unquotes url/iri                                                                                                     
def unquoteurl(href):
    if isinstance(href,bytes):
        href = href.decode('utf-8')
    href = unquote(href)
    return href


def relativePath(to_bkpath, start_dir):
    # remove any trailing path separators from both paths
    dsegs = to_bkpath.rstrip('/').split('/')
    ssegs = start_dir.rstrip('/').split('/')
    if dsegs == ['']: dsegs=[]
    if ssegs == ['']: ssegs=[]
    res = []
    i = 0
    for s1, s2 in zip(dsegs, ssegs):
        if s1 != s2: break
        i+=1
    for p in range(i, len(ssegs),1): res.append('..')
    for p in range(i, len(dsegs),1): res.append(dsegs[p])
    return '/'.join(res)


def resolveRelativeSegmentsInFilePath(file_path):
    res = []
    segs = file_path.split('/')
    for i in range(len(segs)):
        if segs[i] == '.': continue
        if segs[i] == '..':
            if res:
                res.pop()
            else:
                print("Error resolving relative path segments")
        else:
            res.append(segs[i])
    return '/'.join(res)


def buildRelativePath(from_bkpath, to_bkpath):
    if from_bkpath == to_bkpath: return ""
    return relativePath(to_bkpath, startingDir(from_bkpath))


def buildBookPath(dest_relpath, start_folder):
    if start_folder == "" or start_folder.strip() == "": 
        return dest_relpath;
    bookpath = start_folder.rstrip('/') + '/' + dest_relpath
    return resolveRelativeSegmentsInFilePath(bookpath)


def startingDir(file_path):
    ssegs = file_path.split('/')
    ssegs.pop()
    return '/'.join(ssegs)

    
def longestCommonPath(bookpaths):
    # handle special cases
    if len(bookpaths) == 0: return ""
    if len(bookpaths) == 1: return startingDir(bookpaths[0]) + '/'
    fpaths = bookpaths
    fpaths.sort()
    segs1 = fpaths[0].split('/')
    segs2 = fpaths[-1].split('/')
    res = []
    for s1, s2 in zip(segs1, segs2):
        if s1 != s2: break
        res.append(s1)
    if not res or len(res) == 0:
        return ""
    return '/'.join(res) + '/'


def main():
    argv = sys.argv
    p1 = 'This/is/the/../../end.txt'
    print('Testing resolveRelativeSegmentsInFilePath(file_path)')
    print('    file_path: ', p1)
    print(resolveRelativeSegmentsInFilePath(p1))
    print('    ')

    p1 = 'hello.txt'
    p2 = 'goodbye.txt'
    print('Testing buildRelativePath(from_bkpath,to_bkpath')
    print('    from_bkpath: ',p1)
    print('    to_bkpath:   ',p2)
    print(buildRelativePath(p1, p2))
    print('    ')

    p1 = 'OEBPS/Text/book1/chapter1.xhtml'
    p2 = 'OEBPS/Text/book2/chapter1.xhtml'
    print('Testing buildRelativePath(from_bkpath,to_bkpath)')
    print('    from_bkpath: ',p1)
    print('    to_bkpath:   ',p2)
    print(buildRelativePath(p1, p2))
    print('    ')
    
    p1 = 'OEBPS/package.opf'
    p2 = 'OEBPS/Text/book1/chapter1.xhtml'
    print('Testing buildRelativePath(from_bkpath, to_bkpath)')
    print('    from_bkpath: ',p1)
    print('    to_bkpath:   ',p2)
    print(buildRelativePath(p1,p2))
    print('    ')

    p1 = '../../Images/image.png'
    p2 = 'OEBPS/Text/book1/'
    print('Testing buildBookPath(destination_href, start_dir)')
    print('    destination_href: ',p1)
    print('    starting_dir:     ',p2)
    print(buildBookPath(p1, p2))
    print('    ')

    p1 = 'image.png'
    p2 = ''
    print('Testing buildBookPath(destination_href, start_dir)')
    print('    destination_href: ',p1)
    print('    starting_dir:     ',p2)
    print(buildBookPath(p1, p2))
    print('    ')

    p1 = 'content.opf'
    print('Testing startingDir(bookpath')
    print('    bookpath: ',p1)
    print('"'+ startingDir(p1)+'"')
    print('    ')

    bookpaths = []
    bookpaths.append('OEBPS/book1/text/chapter1.xhtml')
    bookpaths.append('OEBPS/book1/html/chapter2.xhtml')
    bookpaths.append('OEBPS/book2/text/chapter3.xhtml')
    print('Testing longestCommonPath(bookpaths)')
    print('    bookpaths: ',bookpaths)
    print('"'+ longestCommonPath(bookpaths)+'"')
    print('    ')

    return 0


if __name__ == '__main__':
    sys.exit(main())
