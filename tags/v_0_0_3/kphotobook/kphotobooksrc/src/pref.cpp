/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#include "pref.h"

#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>

KPhotoBookPreferences::KPhotoBookPreferences()
    : KDialogBase(TreeList, i18n("KPhotoBook Preferences"),
                  Help|Default|Ok|Apply|Cancel, Ok)
{
    // this is the base class for your preferences dialog.  it is now
    // a Treelist dialog.. but there are a number of other
    // possibilities (including Tab, Swallow, and just Plain)
    QFrame* frame;
    frame = addPage(i18n("First Page"), i18n("Page One Options"));
    m_pageOne = new KPhotoBookPrefPageOne(frame);

    frame = addPage(i18n("Second Page"), i18n("Page Two Options"));
    m_pageTwo = new KPhotoBookPrefPageTwo(frame);
}

KPhotoBookPrefPageOne::KPhotoBookPrefPageOne(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}

KPhotoBookPrefPageTwo::KPhotoBookPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}

#include "pref.moc"
