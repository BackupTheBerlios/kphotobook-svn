/*
 * Copyright (C) 2003 Michael Christen <starcube@my-mail.ch>
 */

#ifndef _KPHOTOBOOKPREF_H_
#define _KPHOTOBOOKPREF_H_

#include <kdialogbase.h>
#include <qframe.h>

class KPhotoBookPrefPageOne;
class KPhotoBookPrefPageTwo;

class KPhotoBookPreferences : public KDialogBase
{
    Q_OBJECT
public:
    KPhotoBookPreferences();

private:
    KPhotoBookPrefPageOne* m_pageOne;
    KPhotoBookPrefPageTwo* m_pageTwo;
};

class KPhotoBookPrefPageOne : public QFrame
{
    Q_OBJECT
public:
    KPhotoBookPrefPageOne(QWidget* parent = 0);
};

class KPhotoBookPrefPageTwo : public QFrame
{
    Q_OBJECT
public:
    KPhotoBookPrefPageTwo(QWidget* parent = 0);
};

#endif // _KPHOTOBOOKPREF_H_
