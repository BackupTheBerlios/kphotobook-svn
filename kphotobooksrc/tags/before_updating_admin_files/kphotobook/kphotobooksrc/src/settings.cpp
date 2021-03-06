// This file is generated by kconfig_compiler from settings.kcfg.
// All changes you do to this file will be lost.

#include "settings.h"

#include <kstaticdeleter.h>

Settings *Settings::mSelf = 0;
static KStaticDeleter<Settings> staticDeleter;

Settings *Settings::self()
{
  if ( !mSelf ) {
    staticDeleter.setObject( mSelf, new Settings() );
    mSelf->readConfig();
  }

  return mSelf;
}

Settings::Settings(  )
  : KConfigSkeleton( "kphotobookrc" )
{
  mSelf = this;
  setCurrentGroup( "General" );

  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesGeneralViewMode;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = "IDEAlMode";
    valuesGeneralViewMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = "TabPageMode";
    valuesGeneralViewMode.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemGeneralViewMode;
  itemGeneralViewMode = new KConfigSkeleton::ItemEnum( currentGroup(), "ViewMode", mGeneralViewMode, valuesGeneralViewMode, EnumGeneralViewMode::TabPageMode );
  addItem( itemGeneralViewMode, "GeneralViewMode" );
  KConfigSkeleton::ItemBool  *itemGeneralRescanWhileStartup;
  itemGeneralRescanWhileStartup = new KConfigSkeleton::ItemBool( currentGroup(), "RescanWhileStartup", mGeneralRescanWhileStartup, true );
  addItem( itemGeneralRescanWhileStartup, "GeneralRescanWhileStartup" );

  setCurrentGroup( "TagTree" );

  KConfigSkeleton::ItemInt  *itemTagTreeToolBarIconSize;
  itemTagTreeToolBarIconSize = new KConfigSkeleton::ItemInt( currentGroup(), "ToolBarIconSize", mTagTreeToolBarIconSize, 16 );
  itemTagTreeToolBarIconSize->setMinValue(16);
  itemTagTreeToolBarIconSize->setMaxValue(32);
  addItem( itemTagTreeToolBarIconSize, "TagTreeToolBarIconSize" );
  KConfigSkeleton::ItemInt  *itemTagTreeIconSize;
  itemTagTreeIconSize = new KConfigSkeleton::ItemInt( currentGroup(), "IconSize", mTagTreeIconSize, 22 );
  itemTagTreeIconSize->setMinValue(16);
  itemTagTreeIconSize->setMaxValue(48);
  addItem( itemTagTreeIconSize, "TagTreeIconSize" );
  KConfigSkeleton::ItemFont  *itemTagTreeFont;
  itemTagTreeFont = new KConfigSkeleton::ItemFont( currentGroup(), "Font", mTagTreeFont, KGlobalSettings::generalFont() );
  addItem( itemTagTreeFont, "TagTreeFont" );
  KConfigSkeleton::ItemBool  *itemTagTreeShowIcons;
  itemTagTreeShowIcons = new KConfigSkeleton::ItemBool( currentGroup(), "ShowIcons", mTagTreeShowIcons, true );
  addItem( itemTagTreeShowIcons, "TagTreeShowIcons" );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesTagTreeFilterOperator;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = "And";
    valuesTagTreeFilterOperator.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = "Or";
    valuesTagTreeFilterOperator.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemTagTreeFilterOperator;
  itemTagTreeFilterOperator = new KConfigSkeleton::ItemEnum( currentGroup(), "FilterOperator", mTagTreeFilterOperator, valuesTagTreeFilterOperator, EnumTagTreeFilterOperator::And );
  addItem( itemTagTreeFilterOperator, "TagTreeFilterOperator" );

  setCurrentGroup( "SourceDirTree" );

  KConfigSkeleton::ItemInt  *itemSourceDirTreeToolBarIconSize;
  itemSourceDirTreeToolBarIconSize = new KConfigSkeleton::ItemInt( currentGroup(), "ToolBarIconSize", mSourceDirTreeToolBarIconSize, 16 );
  itemSourceDirTreeToolBarIconSize->setMinValue(16);
  itemSourceDirTreeToolBarIconSize->setMaxValue(32);
  addItem( itemSourceDirTreeToolBarIconSize, "SourceDirTreeToolBarIconSize" );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesSourceDirTreeIconSize;
  KConfigSkeleton::ItemEnum  *itemSourceDirTreeIconSize;
  itemSourceDirTreeIconSize = new KConfigSkeleton::ItemEnum( currentGroup(), "IconSize", mSourceDirTreeIconSize, valuesSourceDirTreeIconSize, 22 );
  itemSourceDirTreeIconSize->setMinValue(16);
  itemSourceDirTreeIconSize->setMaxValue(48);
  addItem( itemSourceDirTreeIconSize, "SourceDirTreeIconSize" );
  KConfigSkeleton::ItemFont  *itemSourceDirTreeFont;
  itemSourceDirTreeFont = new KConfigSkeleton::ItemFont( currentGroup(), "Font", mSourceDirTreeFont, KGlobalSettings::generalFont() );
  addItem( itemSourceDirTreeFont, "SourceDirTreeFont" );
  KConfigSkeleton::ItemBool  *itemSourceDirTreeShowIcons;
  itemSourceDirTreeShowIcons = new KConfigSkeleton::ItemBool( currentGroup(), "ShowIcons", mSourceDirTreeShowIcons, true );
  addItem( itemSourceDirTreeShowIcons, "SourceDirTreeShowIcons" );

  setCurrentGroup( "ImagePreview" );

  KConfigSkeleton::ItemInt  *itemImagePreviewSize;
  itemImagePreviewSize = new KConfigSkeleton::ItemInt( currentGroup(), "Size", mImagePreviewSize, 48 );
  itemImagePreviewSize->setMinValue(32);
  itemImagePreviewSize->setMaxValue(160);
  addItem( itemImagePreviewSize, "ImagePreviewSize" );
  KConfigSkeleton::ItemFont  *itemImagePreviewFont;
  itemImagePreviewFont = new KConfigSkeleton::ItemFont( currentGroup(), "Font", mImagePreviewFont, KGlobalSettings::generalFont() );
  addItem( itemImagePreviewFont, "ImagePreviewFont" );
  KConfigSkeleton::ItemBool  *itemImagePreviewShowName;
  itemImagePreviewShowName = new KConfigSkeleton::ItemBool( currentGroup(), "ShowName", mImagePreviewShowName, true );
  addItem( itemImagePreviewShowName, "ImagePreviewShowName" );
  KConfigSkeleton::ItemBool  *itemImagePreviewAutoRefresh;
  itemImagePreviewAutoRefresh = new KConfigSkeleton::ItemBool( currentGroup(), "AutoRefresh", mImagePreviewAutoRefresh, true );
  addItem( itemImagePreviewAutoRefresh, "ImagePreviewAutoRefresh" );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesImagePreviewSelectionMode;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = "Extended";
    valuesImagePreviewSelectionMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = "Multi";
    valuesImagePreviewSelectionMode.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemImagePreviewSelectionMode;
  itemImagePreviewSelectionMode = new KConfigSkeleton::ItemEnum( currentGroup(), "SelectionMode", mImagePreviewSelectionMode, valuesImagePreviewSelectionMode, EnumImagePreviewSelectionMode::Extended );
  addItem( itemImagePreviewSelectionMode, "ImagePreviewSelectionMode" );

  setCurrentGroup( "FileFilter" );

  QStringList defaultFileFilterFileToHandle;
  defaultFileFilterFileToHandle.append( ".*\\.jpg" );
  defaultFileFilterFileToHandle.append( ".*\\.jpeg" );

  KConfigSkeleton::ItemStringList  *itemFileFilterFileToHandle;
  itemFileFilterFileToHandle = new KConfigSkeleton::ItemStringList( currentGroup(), "FileToHandle", mFileFilterFileToHandle, defaultFileFilterFileToHandle );
  addItem( itemFileFilterFileToHandle, "FileFilterFileToHandle" );
  QStringList defaultFileFilterSubdirsToIgnore;
  defaultFileFilterSubdirsToIgnore.append( "\\..*" );
  defaultFileFilterSubdirsToIgnore.append( "[Tt]humbs" );
  defaultFileFilterSubdirsToIgnore.append( "[Tt]humbnails" );
  defaultFileFilterSubdirsToIgnore.append( "[Tt]humbNails" );

  KConfigSkeleton::ItemStringList  *itemFileFilterSubdirsToIgnore;
  itemFileFilterSubdirsToIgnore = new KConfigSkeleton::ItemStringList( currentGroup(), "SubdirsToIgnore", mFileFilterSubdirsToIgnore, defaultFileFilterSubdirsToIgnore );
  addItem( itemFileFilterSubdirsToIgnore, "FileFilterSubdirsToIgnore" );

  setCurrentGroup( "Tools" );

  KConfigSkeleton::ItemString  *itemToolsDefaultExternalTool;
  itemToolsDefaultExternalTool = new KConfigSkeleton::ItemString( currentGroup(), "DefaultExternalTool", mToolsDefaultExternalTool, "kuickshow" );
  addItem( itemToolsDefaultExternalTool, "ToolsDefaultExternalTool" );
  QStringList defaultToolsExternalTools;
  defaultToolsExternalTools.append( "kuickshow" );

  KConfigSkeleton::ItemStringList  *itemToolsExternalTools;
  itemToolsExternalTools = new KConfigSkeleton::ItemStringList( currentGroup(), "ExternalTools", mToolsExternalTools, defaultToolsExternalTools );
  addItem( itemToolsExternalTools, "ToolsExternalTools" );

  setCurrentGroup( "FileSystem" );

  KConfigSkeleton::ItemString  *itemFileSystemLastOpenedFile;
  itemFileSystemLastOpenedFile = new KConfigSkeleton::ItemString( currentGroup(), "LastOpenedFile", mFileSystemLastOpenedFile );
  addItem( itemFileSystemLastOpenedFile, "FileSystemLastOpenedFile" );
  KConfigSkeleton::ItemString  *itemFileSystemLastAddedSourcedir;
  itemFileSystemLastAddedSourcedir = new KConfigSkeleton::ItemString( currentGroup(), "LastAddedSourcedir", mFileSystemLastAddedSourcedir );
  addItem( itemFileSystemLastAddedSourcedir, "FileSystemLastAddedSourcedir" );
}

Settings::~Settings()
{
  if ( mSelf == this )
    staticDeleter.setObject( mSelf, 0, false );
}

