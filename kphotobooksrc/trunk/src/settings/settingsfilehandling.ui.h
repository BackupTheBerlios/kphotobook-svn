/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "settings.h"

#include <kinputdialog.h>
#include <klocale.h>

#include <qstring.h>

void SettingsFileHandling::filePatternAdd_clicked() {

    QString newFilePattern = KInputDialog::getText(
	    i18n("Add file pattern"),
	    i18n("Enter the new pattern for files to handle"),
	    "");

    if (!newFilePattern.isEmpty()) {
        kcfg_FileFilterFileToHandle->insertItem(newFilePattern);
    }

    // select the new added pattern
    kcfg_FileFilterFileToHandle->setSelected(kcfg_FileFilterFileToHandle->count() - 1, true);

    // enable the remove button
    filePatternRemove->setEnabled(true);
 }


void SettingsFileHandling::filePatternEdit_clicked() {

    int currentItem = kcfg_FileFilterFileToHandle->currentItem();

    QString newFilePattern = KInputDialog::getText(
	    i18n("Edit file pattern"),
	    i18n("Change the pattern for files to handle"),
	    kcfg_FileFilterFileToHandle->currentText());

    if (!newFilePattern.isEmpty()) {
        kcfg_FileFilterFileToHandle->changeItem(newFilePattern, currentItem);
    }

    // select the just changed pattern
    kcfg_FileFilterFileToHandle->setSelected(currentItem, true);

    // enable the remove button
    filePatternRemove->setEnabled(true);
}


void SettingsFileHandling::filePatternRemove_clicked() {

    unsigned int currentItem = kcfg_FileFilterFileToHandle->currentItem();
    kcfg_FileFilterFileToHandle->removeItem(currentItem);

    // select the item at the same position as the deleted item
    if (kcfg_FileFilterFileToHandle->count() > 0) {
        if (currentItem < (kcfg_FileFilterFileToHandle->count())) {
            kcfg_FileFilterFileToHandle->setSelected(currentItem, true);
        } else {
            kcfg_FileFilterFileToHandle->setSelected(kcfg_FileFilterFileToHandle->count() - 1, true);
        }
    }

    // enable the remove button if there are items
    filePatternRemove->setEnabled(kcfg_FileFilterFileToHandle->count());
}


void SettingsFileHandling::filePatternDefault_clicked() {

    Settings* defaultSettings = Settings::self();
    defaultSettings->useDefaults(true);

    kcfg_FileFilterFileToHandle->clear();
    kcfg_FileFilterFileToHandle->insertStringList(Settings::fileFilterFileToHandle());
    kcfg_FileFilterFileToHandle->setSelected(0, true);

    defaultSettings->useDefaults(false);
}


void SettingsFileHandling::dirPatternAdd_clicked() {

    QString newDirPattern = KInputDialog::getText(
	    i18n("Add directory pattern"),
	    i18n("Enter the new pattern for directories to ignore"),
	    "");

    if (!newDirPattern.isEmpty()) {
        kcfg_FileFilterSubdirsToIgnore->insertItem(newDirPattern);
    }

    // select the new added pattern
    kcfg_FileFilterSubdirsToIgnore->setSelected(kcfg_FileFilterSubdirsToIgnore->count() - 1, true);

    // enable the remove button
    dirPatternRemove->setEnabled(true);
}


void SettingsFileHandling::dirPatternEdit_clicked() {

    int currentItem = kcfg_FileFilterSubdirsToIgnore->currentItem();

    QString newDirPattern = KInputDialog::getText(
	    i18n("Edit directory pattern"),
	    i18n("Change the pattern for directories to ignore"),
	    kcfg_FileFilterSubdirsToIgnore->currentText());

    if (!newDirPattern.isEmpty()) {
        kcfg_FileFilterSubdirsToIgnore->changeItem(newDirPattern, currentItem);
    }

    // select the just changed pattern
    kcfg_FileFilterSubdirsToIgnore->setSelected(currentItem, true);

    // enable the remove button
    dirPatternRemove->setEnabled(true);
}


void SettingsFileHandling::dirPatternRemove_clicked() {

    unsigned int currentItem = kcfg_FileFilterSubdirsToIgnore->currentItem();
    kcfg_FileFilterSubdirsToIgnore->removeItem(currentItem);

    // select the item at the same position as the deleted item
    if (kcfg_FileFilterSubdirsToIgnore->count() > 0) {
        if (currentItem < (kcfg_FileFilterSubdirsToIgnore->count())) {
            kcfg_FileFilterSubdirsToIgnore->setSelected(currentItem, true);
        } else {
            kcfg_FileFilterSubdirsToIgnore->setSelected(kcfg_FileFilterSubdirsToIgnore->count() - 1, true);
        }
    }

    // enable the remove button if there are items
    dirPatternRemove->setEnabled(kcfg_FileFilterSubdirsToIgnore->count());
}


void SettingsFileHandling::dirPatternDefault_clicked() {

    Settings* defaultSettings = Settings::self();
    defaultSettings->useDefaults(true);

    kcfg_FileFilterSubdirsToIgnore->clear();
    kcfg_FileFilterSubdirsToIgnore->insertStringList(Settings::fileFilterSubdirsToIgnore());
    kcfg_FileFilterSubdirsToIgnore->setSelected(0, true);

    defaultSettings->useDefaults(false);
}
