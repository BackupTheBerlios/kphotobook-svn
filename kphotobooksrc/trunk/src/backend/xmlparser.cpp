/***************************************************************************
 *   Copyright (C) 2003 by Michael Christen                                *
 *   starcube@my-mail.ch                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "xmlparser.h"

#include "../engine/engine.h"
#include "../engine/file.h"
#include "../engine/filetagnodeassocboolean.h"
#include "../engine/filetagnodeassocdatetime.h"
#include "../engine/filetagnodeassocradio.h"
#include "../engine/filetagnodeassocstring.h"
#include "../engine/folder.h"
#include "../engine/tagnode.h"
#include "../engine/tagnodeboolean.h"
#include "../engine/tagnodedatetime.h"
#include "../engine/tagnoderadio.h"
#include "../engine/tagnodestring.h"
#include "../engine/tagnodetitle.h"

#include <qfileinfo.h>
#include <qpixmap.h>

#include <typeinfo>


Tracer* XmlParser::tracer = Tracer::getInstance("kde.kphotobook.backend", "XmlParser");


//
// methods of content handler
//
bool XmlParser::startElement( __attribute__((unused)) const QString& namespaceURI, const QString& localName,
                              __attribute__((unused)) const QString& qName,  const QXmlAttributes& atts)
{
    //
    // handle starttag 'kphotobook'
    //
    if (localName == ELEMENT_KPHOTOBOOK) {
        // state must be UNDEFINED_START
        if (m_section != SECTION_UNDEFINED_START) {
            m_exception = new EngineException(
                "Starttag 'kphotobook' found at an unexpected position.",
                "");
            return false;
        }

        // the only valid and mandatory attribute for kphotobook is 'id'
        // for compatibility reasons the attribute name is accepted and ignored
        if (atts.length() == 1) {
            if (atts.localName(0) != ATTRIBUTE_KPHOTOBOOK_NAME && atts.localName(0) != ATTRIBUTE_KPHOTOBOOK_UID) {
                QString msg = QString("Tag 'kphotobook' must contain the attribute 'id' but attribute '%1' found.").arg(atts.localName(0));
                m_exception = new EngineException(msg,"");
                return false;
            }
        } else {
            m_exception = new EngineException("Tag 'kphotobook' must contain the attribute 'id' exactly once.", "");
            return false;
        }

        // everything is ok --> store the name
        if (atts.localName(0) == ATTRIBUTE_KPHOTOBOOK_UID) {
            m_engine->m_uid = new QString(atts.value(0));
        }

        // change the state to KPHOTOBOOK
        m_section = SECTION_KPHOTOBOOK;

        return true;
    } else

    //
    // handle starttag 'sourcedirs'
    //
    if (localName == ELEMENT_SOURCEDIRS) {
        // state must be KPHOTOBOOK
        if (m_section != SECTION_KPHOTOBOOK) {
            m_exception = new EngineException("Starttag 'sourcedirs' found at an unexpected position.", "");
            return false;
        }

        // there must be no attribute
        if (atts.length() != 0) {
            m_exception = new EngineException("Tag 'sourcedirs' must not contain any attribute.", "");
            return false;
        }

        // change the state to SOURCEDIRS
        m_section = SECTION_SOURCEDIRS;
    } else

    //
    // handle starttag 'sourcedir'
    //
    if (localName == ELEMENT_SOURCEDIR) {
        // state must be SOURCEDIRS
        if (m_section != SECTION_SOURCEDIRS) {
            m_exception = new EngineException("Starttag 'sourcedir' found at an unexpected position.", "");
            return false;
        }

        return handleSourceDir(atts);
    } else

    //
    // handle starttag 'tags'
    //
    if (localName == ELEMENT_TAGS) {
        // state must be SOURCEDIRS_DONE
        if (m_section != SECTION_SOURCEDIRS_DONE) {
            m_exception = new EngineException("Starttag 'tags' found at an unexpected position.", "");
            return false;
        }

        // there must be no attribute
        if (atts.length() != 0) {
            m_exception = new EngineException("Tag 'tags' must not contain any attribute.", "");
            return false;
        }

        // change the state to TAGS
        m_section = SECTION_TAGS;
    } else

    //
    // handle starttag 'tag'
    //
    if (localName == ELEMENT_TAG) {
        // state must be TAGS
        if (m_section != SECTION_TAGS) {
            m_exception = new EngineException("Starttag 'tag' found at an unexpected position.", "");
            return false;
        }

        return handleTag(atts);
    } else

    //
    // handle starttag 'files'
    //
    if (localName == ELEMENT_FILES) {
        // state must be SECTION_TAGS_DONE or SECTION_FILES_DONE
        if (m_section != SECTION_TAGS_DONE && m_section != SECTION_FILES_DONE) {
            m_exception = new EngineException("Starttag 'files' found at an unexpected position.", "");
            return false;
        }

        // the only valid and mandatory attribute for files is 'sourcedirId'
        int sourcedirId = -1;
        if (atts.length() == 1) {
            if (atts.localName(0) != ATTRIBUTE_FILES_SOURCEDIRID) {
                QString msg = QString("Tag 'files' must contain the attribute 'sourcedirId' but attribute '%1' found.").arg(atts.localName(0));
                m_exception = new EngineException(msg, "");
                return false;
            }

            bool ok;
            sourcedirId = atts.value(0).toInt(&ok); // convert the string id to an int

            if (!ok) {
                QString msg = QString("Value of attribute 'sourcedirId' is not an integer: '%1'.").arg(atts.value(0));
                m_exception = new EngineException(msg, "");
                return false;
            }
        } else {
            m_exception = new EngineException("Tag 'files' must contain the attribute 'sourcedirId' exactly once.", "");
            return false;
        }

        // everything is ok --> get the sourcedir
        m_currentSourceDir = m_engine->m_sourceDirDict->find(sourcedirId);

        // throw an exception if the sourcedir does not exist
        if (!m_currentSourceDir) {
            QString msg = QString("No sourcedir found with id '%1'.").arg(sourcedirId);
            m_exception = new EngineException(msg, "");
            return false;
        }

        // change the state to FILES
        m_section = SECTION_FILES;

        return true;
    } else

    //
    // handle starttag 'file'
    //
    if (localName == ELEMENT_FILE) {
        // state must be FILES
        if (m_section != SECTION_FILES) {
            m_exception = new EngineException("Starttag 'file' found at an unexpected position.", "");
            return false;
        }

        return handleFile(atts);
    } else

    //
    // handle starttag 'tagAssoc'
    //
    if (localName == ELEMENT_TAGASSOC) {
        // state must be FILE
        if (m_section != SECTION_FILES || !m_currentFile) {
            m_exception = new EngineException("Starttag 'tagassoc' found at an unexpected position.", "");
            return false;
        }

        return handleTagAssoc(atts);
    }

    return true;
}


bool XmlParser::endElement( __attribute__((unused)) const QString& namespaceURI,  const QString& localName,
                            __attribute__((unused)) const QString& qName)
{
    //
    // handle endtag 'kphotobook'
    //
    if (localName == ELEMENT_KPHOTOBOOK) {
        // state must be FILES_DONE or TAGS (if file contains no sourcedirs)
        if (m_section != SECTION_FILES_DONE && m_section != SECTION_TAGS_DONE) {
            m_exception = new EngineException("Endtag '/kphotobook' found at an unexpected position.", "");
            return false;
        }

        // change the state to UNDEFINED_END
        m_section = SECTION_UNDEFINED_END;
    } else

    //
    // handle endtag 'sourcedirs'
    //
    if (localName == ELEMENT_SOURCEDIRS) {
        // state must be SOURCEDIRS
        if (m_section != SECTION_SOURCEDIRS && m_sourceDirstack.isEmpty()) {
            m_exception = new EngineException("Endtag '/sourcedirs' found at an unexpected position.","");
            return false;
        }

        // change the state to KPHOTOBOOK
        m_section = SECTION_SOURCEDIRS_DONE;
    } else

    //
    // handle endtag 'sourcedir'
    //
    if (localName == ELEMENT_SOURCEDIR) {
        // state must be 'SOURCEDIR'
        if (m_section != SECTION_SOURCEDIRS || m_sourceDirstack.isEmpty()) {
            m_exception = new EngineException("Endtag '/sourcedir' found at an unexpected position.", "");
            return false;
        }

        // remove the just ended sourcedir from the sourcedir stack
        m_sourceDirstack.remove();
    } else

    //
    // handle endtag 'tags'
    //
    if (localName == ELEMENT_TAGS) {
        // state must be TAGS
        if (m_section != SECTION_TAGS && m_tagNodeStack.isEmpty()) {
            m_exception = new EngineException("Endtag '/tags' found at an unexpected position.", "");
            return false;
        }

        // change the state to KPHOTOBOOK
        m_section = SECTION_TAGS_DONE;
    } else

    //
    // handle endtag 'tag'
    //
    if (localName == ELEMENT_TAG) {
        // state must be 'TAG'
        if (m_section != SECTION_TAGS || m_tagNodeStack.isEmpty()) {
            m_exception = new EngineException("Endtag '/tag' found at an unexpected position.", "");
            return false;
        }

        // remove the just ended tag from the tagnode stack
        m_tagNodeStack.remove();
    } else

    //
    // handle endtag 'files'
    //
    if (localName == ELEMENT_FILES) {
        // state must be FILES
        if (m_section != SECTION_FILES || !m_currentSourceDir) {
            m_exception = new EngineException("Endtag '/files' found at an unexpected position.", "");
            return false;
        }

        // remove the currently stored sourcedir
        m_currentSourceDir = 0;

        // change the state to KPHOTOBOOK
        m_section = SECTION_FILES_DONE;
    } else

    //
    // handle endtag 'file'
    //
    if (localName == ELEMENT_FILE) {
        // state must be 'FILE'
        if (m_section != SECTION_FILES || !m_currentFile) {
            m_exception = new EngineException("Endtag '/file' found at an unexpected position.", "");
            return false;
        }

        // change the state to FILES
        m_section = SECTION_FILES;
        m_currentFile = 0;
        m_tagNodesOfCurrentFile = 0;
    } else

    //
    // handle endtag 'tagassoc'
    //
    if (localName == ELEMENT_TAGASSOC) {
        // state must be 'TAGASSOC'
        if (m_section != SECTION_FILES || !m_currentFile || !m_tagNodesOfCurrentFile ) {
            m_exception = new EngineException("Endtag '/tagassoc' found at an unexpected position.",  "");
            return false;
        }

        // we do not change the state here!
    }

    return true;
}


bool XmlParser::characters(__attribute__((unused)) const QString& ch)
{
    // we never have characters inside a container
    return true;
}


//
// methods of error handler
//
bool XmlParser::warning(const QXmlParseException& exception)
{
    fatalError(exception);
    return true;
}


bool XmlParser::error(const QXmlParseException& exception)
{
    fatalError(exception);
    return true;
}


bool XmlParser::fatalError(const QXmlParseException& exception)
{
    QString message = QString("Parsing error occured @ line: %1, column:%2").arg(exception.lineNumber()).arg(exception.columnNumber());

    if (m_exception) {
        // if there exists an exception already -> add the detailmessage
        m_exception->appendDetailMessage(message);
    } else {
        // create an exception if there does not exist one already
        m_exception = new EngineException(exception.message(), message );
    }

    return true;
}


//
// private methods
//
bool XmlParser::handleSourceDir(const QXmlAttributes& atts)
{
    tracer->sinvoked(__func__) << "with attributes: " << toString(atts) << endl;

    // <sourcedir
    //   id="0"              (mandatory)
    //   dir="/fff/rt"       (mandatory)
    //   recursive="true"    (optional)
    // >

    Folder* sourceDir;

    // the attributes 'id' and 'dir' are mandatory and the attribute 'recursive' is optional
    if (atts.length() == 2 || atts.length() == 3) {
        // parse first attribute 'id'
        int id = -1;
        if (atts.localName(0) == ATTRIBUTE_SOURCEDIR_ID) {
            bool ok;
            id = atts.value(0).toInt(&ok); // convert the string id to an int

            if (!ok) {
                QString msg = QString("Value of attribute 'id' is not an integer: '%1'.").arg(atts.value(0));
                m_exception = new EngineException(msg, "");
                return false;
            }
        } else {
            QString msg = QString("Mandatory first attribute of tag 'sourcedir' must be 'id', but attribute '%1' found.").arg(atts.localName(0));
            m_exception = new EngineException(msg, "");
            return false;
        }

        // parse second attribute 'dir'
        QDir* dir = 0;
        if (atts.localName(1) == ATTRIBUTE_SOURCEDIR_DIR) {
            QString dirStr = atts.value(1);

            if (dirStr.length() > 0) {
                dir = new QDir(dirStr);
            } else {
                m_exception = new EngineException("Value of attribute 'dir' is rather empty, but should contain an absolute directory.", "");
                return false;
            }

            if (dir->isRelative()) {
                QString msg = QString("Directory of tag 'sourcedir' is not absolute: '%1'.").arg(atts.value(1));
                m_exception = new EngineException(msg, "");
                return false;
            }
        } else {
            QString msg = QString("Mandatory second attribute of tag 'sourcedir' must be 'dir', but attribute '%1' found.").arg(atts.localName(1));
            m_exception = new EngineException(msg, "");
            return false;
        }

        // parse third attribute 'recursive'
        bool recursive = false;
        if (atts.length() == 3) {
            if (atts.localName(2) == ATTRIBUTE_SOURCEDIR_RECURSIVE) {
                QString recursiveStr = atts.value(2);

                if (recursiveStr == "true") {
                    recursive = true;
                } else if (recursiveStr == "false") {
                    recursive = false;
                } else {
                    QString msg = QString("Value of attribute 'recursive' must be 'true' or 'false', but is '%1'.").arg(atts.value(2));
                    m_exception = new EngineException(msg, "" );
                    return false;
                }
            } else {
                QString msg = QString("Optional third attribute of tag 'sourcedir' must be 'recursive', but attribute '%1' found.").arg(atts.localName(2));
                m_exception = new EngineException(msg, "");
                return false;
            }
        }

        // test if the parsed directory is addable if it is toplevel sourcedir
        if (!m_sourceDirstack.current()) {
            try {
                m_engine->fileSystemScanner()->testIfFolderIsAddable(dir, recursive);
            } catch(EngineException* ex) {
                m_exception = new EngineException(ex->message(), ex->detailMessage());
                delete ex;
                return false;
            }
        }

        // everything is ok --> create the sourcedir
        sourceDir = new Folder(id, dir, recursive);
    } else {
        m_exception = new EngineException(
            "Tag 'sourcedir' must contain the attributes 'id', 'dir' and optionally 'recursive'.", "");
        return false;
    }

    // check that the id of the sourcedir is not already used
    Folder* conflictingSourceDir = m_engine->m_sourceDirDict->find(sourceDir->id());
    if (conflictingSourceDir) {
        QString msg = QString("The id (%1) of the sourcedir '%2' conflicts with the sourcedir '%3'.").arg(sourceDir->id()).arg(sourceDir->dir()->absPath()).arg(conflictingSourceDir->dir()->absPath());
        m_exception = new EngineException(msg, "");
        return false;
    }

    // get the enclosing sourcedirectory from the stack
    Folder* parentSourceDir = m_sourceDirstack.current();

    if (parentSourceDir) {
        // the current sourcedir seems to be the child of another sourcedir --> link it to its parent
        sourceDir->setParent(parentSourceDir);
    } else {
        // the current sourcedir seems to be a toplevel sourcedir --> add the sourcedir to the engine
        m_engine->m_sourceDirs->append(sourceDir);
    }


    // put the sourcedir into the sourcedir dictionary (id to sourcedir map)
    m_engine->m_sourceDirDict->insert(sourceDir->id(), sourceDir);

    // adjust the next available id for sourcedirectories
    if ((sourceDir->id() + 1) > m_engine->m_nextSourceDirId) {
        m_engine->m_nextSourceDirId = sourceDir->id() + 1;
    }

    // put the sourcedir onto the sourcedir stack
    m_sourceDirstack.push(sourceDir);

    return true;
}


bool XmlParser::handleTag(const QXmlAttributes& atts)
{
    tracer->sinvoked(__func__) << "with attributes: " << toString(atts) << endl;

    // <tagnode
    //   id="0"              (mandatory)
    //   name="Event"        (mandatory)
    //   type="title"        (mandatory)
    //   icon="/..././png"   (optional)
    // />

    int id = -1;
    QString name    = QString::null;
    QString comment = "";
    TagNode::Type tagNodeTypeId = TagNode::TYPE_INVALID;
    QString icon;
    bool secret = false;
    bool readonly = false;

    QString curKey, curVal;
    // first read all arguments...
    for (int i = 0; i <= atts.length(); ++i)
    {
        curKey = atts.localName(i);
        curVal = atts.value(i);

        if (curKey == ATTRIBUTE_TAG_ID) {
            bool ok;
            id = curVal.toInt(&ok); // convert the string id to an int

            if (!ok) {
                QString msg = QString("Value of attribute 'id' is not an integer: '%1'.").arg(curVal);
                m_exception = new EngineException(msg, "");
                return false;
            }
        }
        else if (curKey == ATTRIBUTE_TAG_NAME) {
            name = curVal;
        }
        else if (curKey == ATTRIBUTE_TAG_COMMENT) {
            comment = curVal;
        }
        else if (curKey == ATTRIBUTE_TAG_TYPE) {
            QString tagnodeTypeStr = curVal;

            tagNodeTypeId = TagNode::tagNodeTypeId(tagnodeTypeStr);

            if (tagNodeTypeId == TagNode::TYPE_INVALID) {
                QString msg = QString("Value of attribute 'type' must be '%1' or '%2', but is '%3'.").arg(TagNode::tagNodeType(TagNode::TYPE_TITLE)).arg(TagNode::tagNodeType(TagNode::TYPE_BOOLEAN)).arg(curVal);
                m_exception = new EngineException(msg,"");
                return false;
            }

        }
        else if (curKey == ATTRIBUTE_TAG_ICON) {
            icon = curVal;
        }
        else if (curKey == ATTRIBUTE_TAG_SECRET) {
            secret = (curVal == "yes");
        }
        else if (curKey == ATTRIBUTE_TAG_READONLY) {
            readonly = (curVal == "yes");
        }
    }

    //and then do the consistency check:
    // mandatory id given?
    if (id < 0) {
        m_exception = new EngineException(
                "Value of attribute 'id' is not set, but should contain the id of the tagnode.", "");
        return false;
    }

    //mandatory name given?
    if (name.isNull() || name.length() <= 0) {
        m_exception = new EngineException(
                "Value of attribute 'name' is not set or empty, but should contain the name of the tagnode.", "");
        return false;
    }

    //mandatory type given?
    if (tagNodeTypeId == TagNode::TYPE_INVALID) {
        m_exception = new EngineException(
                "Value of attribute 'type' is not set, but should contain the type of the tagnode.", "");
        return false;
    }

    // comment is optional`
    if (comment.length() <= 0) {
        tracer->sdebug(__func__) << "Optional attribute 'comment' of tagnode with the name '" << name
            << "' is empty." << endl;
    }

    // icon is optional`
    if (icon.length() <= 0) {
        tracer->sdebug(__func__) << "Optional attribute 'icon' of tagnode with the name '" << name
            << "' is empty." << endl;
    }

    // secret is optional`
    if (!secret) {
        tracer->sdebug(__func__) << "Optional attribute 'secret' of tagnode with the name '" << name
            << "' is empty." << endl;
    }

    // secret is optional`
    if (!readonly) {
        tracer->sdebug(__func__) << "Optional attribute 'readonly' of tagnode with the name '" << name
            << "' is empty." << endl;
    }

    // check that the id of the tagNode is not already used
    TagNode* conflictingTagNode = m_engine->m_tagNodeDict->find(id);
    if (conflictingTagNode) {
        QString msg = QString("The id (%1) of the tagnode '%2' conflicts with the tagnode '%3'.").arg(id).arg(name).arg(*conflictingTagNode->text());
        m_exception = new EngineException(msg, "");
        return false;
    }

    // get the enclosing tagnode from the stack
    TagNode* parentTagNode = m_tagNodeStack.current();

    if (!m_engine->isTagTextValid(parentTagNode, name, true)) {
        QString msg = QString("The name (%1) of the tagnode with id '%2' is invalid.").arg(name).arg(id);
        m_exception = new EngineException(msg, "");
        return false;
    }

    // everything is ok --> create the concrete tagnode
    TagNode* tagNode = TagNode::createInstance(tagNodeTypeId, id, name, comment, icon, parentTagNode);
    tagNode->setSecret(secret);
    tagNode->setReadonly(readonly);

    if (!parentTagNode) {
        // the current tagNode seems to be a toplevel tagNode --> add the tagNode to the engine
        m_engine->m_tagForest->append(tagNode);

        if (tagNodeTypeId == TagNode::TYPE_TITLE && name.upper() == "EXIF") {
            // this is the EXIF tagnode
            m_engine->m_exifTitleTag = dynamic_cast<TagNodeTitle*>(tagNode);
        }
    }

    // put the tagNode into the tagNode dictionary (id to tagNode map)
    m_engine->m_tagNodeDict->insert(tagNode->id(), tagNode);

    // adjust the next available id for tagNode
    if ((tagNode->id() + 1) > m_engine->m_nextTagNodeId) {
        m_engine->m_nextTagNodeId = tagNode->id() + 1;
    }

    // put the tagNode onto the tagNode stack
    m_tagNodeStack.push(tagNode);

    return true;
}


bool XmlParser::handleFile(const QXmlAttributes& atts)
{
//    tracer->sinvoked(__func__) << "with attributes: " << toString(atts) << endl;

    // <file
    //   name="2003-06-04-10:01:45.jpg"    (mandatory)
    //   rotate="0"                        (optional)
    // >

    // the attribute 'name' is mandatory and the attribute 'rotate' is optional
    if (atts.length() == 1 || atts.length() == 2) {
        // parse first attribute 'name'
        QString fileName;
        if (atts.localName(0) == ATTRIBUTE_FILE_NAME) {
            fileName = atts.value(0);

            if (fileName.length() < 1) {
                m_exception = new EngineException(
                    "Value of attribute 'name' of tag 'file' is rather empty, but should contain a filename.", "");
                return false;
            }
        } else {
            QString msg = QString("Mandatory first attribute of tag 'file' must be 'name', but attribute '%1' found.").arg(atts.localName(1));
            m_exception = new EngineException(msg, "");
            return false;
        }

        // parse optional second attribute 'rotate'
        int degrees = 0;
        if (atts.length() == 2) {
            if (atts.localName(1) == ATTRIBUTE_FILE_ROTATE) {
                bool ok;
                degrees = atts.value(1).toInt(&ok); // convert the string id to an int

                if (!ok) {
                    QString msg = QString("Value of attribute 'rotate' is not an integer: '%1'.").arg(atts.value(1));
                    m_exception = new EngineException(msg, "");
                    return false;
                }
            } else {
                QString msg = QString("Optional second attribute of tag 'file' must be 'rotate', but attribute '%1' found.").arg(atts.localName(1));
                m_exception = new EngineException(msg, "");
                return false;
            }
        }

        // everything is ok --> create the file
        QFileInfo* fileInfo = new QFileInfo(*(m_currentSourceDir->dir()), fileName);

        m_currentFile = new File(m_engine, m_currentSourceDir, fileInfo, degrees);
    } else {
        m_exception = new EngineException("Tag 'file' must contain the attributes 'name' and optionally 'rotate'.", "");
        return false;
    }

    // add the file to the list of files in the sourcedir
    m_currentSourceDir->appendFile(m_currentFile);

    // add the file to the list of files in the engine
    m_engine->m_fileList->append(m_currentFile);
    m_engine->m_fileDict->insert(m_currentFile->fileInfo()->absFilePath(), m_currentFile);

    return true;
}


bool XmlParser::handleTagAssoc(const QXmlAttributes& atts) {

    // <tagassoc
    //   tagId="4"       (mandatory)
    //   value="true"    (mandatory)
    // />

    // initialize the list of tagnodes linked with the current file
    if (!m_tagNodesOfCurrentFile) {
        m_tagNodesOfCurrentFile = new QIntDict<TagNode>();
    }

    // get the tagnode
    int tagId = -1;
    QString value;

    // the attributes 'tagId' and 'value' are mandatory
    if (atts.length() == 2) {
        // parse first attribute 'tagId'
        if (atts.localName(0) == ATTRIBUTE_TAGASSOC_TAGID) {
            bool ok;
            tagId = atts.value(0).toInt(&ok); // convert the string id to an int

            if (!ok) {
                QString msg = QString("Value of attribute 'tagId' is not an integer: '%1'.").arg(atts.value(0));
                m_exception = new EngineException( msg, "");
                return false;
            }
        } else {
            QString msg = QString("Mandatory first attribute of tag 'tagassoc' must be 'tagId', but attribute '%1' found.").arg(atts.localName(0));
            m_exception = new EngineException( msg, "");
            return false;
        }

        // parse second attribute 'value'
        if (atts.localName(1) == ATTRIBUTE_TAGASSOC_VALUE) {
            value = atts.value(1);

            if (value.length() < 1) {
                m_exception = new EngineException(
                    "Value of attribute 'value' of tag 'tagassoc' is rather empty, but should contain a value.", "");
                return false;
            }
        } else {
            QString msg = QString("Mandatory second attribute of tag 'tagassoc' must be 'value', but attribute '%1' found.").arg(atts.localName(1));
            m_exception = new EngineException( msg, "");
            return false;
        }
    } else {
        m_exception = new EngineException( "Tag 'file' must contain the attributes 'name' and optionally 'rotate'.", "");
        return false;
    }

    // everything seems to be ok

    //get the tagnode with the parsed tagId
    TagNode* tagNode = m_engine->m_tagNodeDict->find(tagId);

    if (!tagNode) {
        QString msg = QString("There exists no tagNode with id '%1'.").arg(tagId);
        m_exception = new EngineException( msg, "");
        return false;
    }

    // link the current file to the found tagNode
    if (typeid(*tagNode) == typeid(TagNodeBoolean)) {
        new FileTagNodeAssocBoolean(m_currentFile, dynamic_cast<TagNodeBoolean*>(tagNode), value);
    } else
    if (typeid(*tagNode) == typeid(TagNodeString)) {
        new FileTagNodeAssocString(m_currentFile, dynamic_cast<TagNodeString*>(tagNode), value);
    } else
    if (typeid(*tagNode) == typeid(TagNodeRadio)) {
        new FileTagNodeAssocRadio(m_currentFile, dynamic_cast<TagNodeRadio*>(tagNode), value);
    } else
    if (typeid(*tagNode) == typeid(TagNodeDateTime)) {
        new FileTagNodeAssocDateTime(m_currentFile, dynamic_cast<TagNodeDateTime*>(tagNode), value);
    } else {
        tracer->warning(__func__, "Unknown tagNode type found!");
    }

    return true;
}


QString XmlParser::toString(const QXmlAttributes& atts)
{
    QString buf = QString();
    for (int i = 0; i < atts.length(); i++) {
        if (i > 0) {
            buf.append(", ");
        }
        buf.append(atts.localName(i));
        buf.append("=");
        buf.append(atts.value(i));
    }

    return buf;
}

