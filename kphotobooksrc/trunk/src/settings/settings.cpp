// This file is generated by kconfig_compiler from settings.kcfg.
// All changes you do to this file will be lost.

#include "settings.h"

#include <kstaticdeleter.h>

Settings *Settings::mSelf = 0;
static KStaticDeleter<Settings> staticSettingsDeleter;

Settings *Settings::self()
{
  if ( !mSelf ) {
    staticSettingsDeleter.setObject( mSelf, new Settings() );
    mSelf->readConfig();
  }

  return mSelf;
}

Settings::Settings(  )
  : KConfigSkeleton( QString::fromLatin1( "kphotobookrc" ) )
{
  mSelf = this;
  setCurrentGroup( QString::fromLatin1( "General" ) );

  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesGeneralViewMode;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "IDEAlMode" );
    valuesGeneralViewMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "TabPageMode" );
    valuesGeneralViewMode.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemGeneralViewMode;
  itemGeneralViewMode = new KConfigSkeleton::ItemEnum( currentGroup(), QString::fromLatin1( "ViewMode" ), mGeneralViewMode, valuesGeneralViewMode, EnumGeneralViewMode::TabPageMode );
  addItem( itemGeneralViewMode, QString::fromLatin1( "GeneralViewMode" ) );
  KConfigSkeleton::ItemBool  *itemGeneralRescanWhileStartup;
  itemGeneralRescanWhileStartup = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "RescanWhileStartup" ), mGeneralRescanWhileStartup, true );
  addItem( itemGeneralRescanWhileStartup, QString::fromLatin1( "GeneralRescanWhileStartup" ) );
  KConfigSkeleton::ItemBool  *itemGeneralCheckUntaggedOnExit;
  itemGeneralCheckUntaggedOnExit = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "CheckUntaggedOnExit" ), mGeneralCheckUntaggedOnExit, true );
  addItem( itemGeneralCheckUntaggedOnExit, QString::fromLatin1( "GeneralCheckUntaggedOnExit" ) );

  setCurrentGroup( QString::fromLatin1( "TagTree" ) );

  KConfigSkeleton::ItemInt  *itemTagTreeToolBarIconSize;
  itemTagTreeToolBarIconSize = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "ToolBarIconSize" ), mTagTreeToolBarIconSize, 16 );
  itemTagTreeToolBarIconSize->setMinValue(16);
  itemTagTreeToolBarIconSize->setMaxValue(32);
  addItem( itemTagTreeToolBarIconSize, QString::fromLatin1( "TagTreeToolBarIconSize" ) );
  KConfigSkeleton::ItemInt  *itemTagTreeIconSize;
  itemTagTreeIconSize = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "IconSize" ), mTagTreeIconSize, 22 );
  itemTagTreeIconSize->setMinValue(16);
  itemTagTreeIconSize->setMaxValue(48);
  addItem( itemTagTreeIconSize, QString::fromLatin1( "TagTreeIconSize" ) );
  KConfigSkeleton::ItemFont  *itemTagTreeFont;
  itemTagTreeFont = new KConfigSkeleton::ItemFont( currentGroup(), QString::fromLatin1( "Font" ), mTagTreeFont, KGlobalSettings::generalFont() );
  addItem( itemTagTreeFont, QString::fromLatin1( "TagTreeFont" ) );
  KConfigSkeleton::ItemBool  *itemTagTreeShowIcons;
  itemTagTreeShowIcons = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "ShowIcons" ), mTagTreeShowIcons, true );
  addItem( itemTagTreeShowIcons, QString::fromLatin1( "TagTreeShowIcons" ) );
  KConfigSkeleton::ItemBool  *itemTagTreeBoldMatches;
  itemTagTreeBoldMatches = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "BoldMatches" ), mTagTreeBoldMatches, true );
  addItem( itemTagTreeBoldMatches, QString::fromLatin1( "TagTreeBoldMatches" ) );
  KConfigSkeleton::ItemBool  *itemTagTreeBoldMatchesCollapsedOnly;
  itemTagTreeBoldMatchesCollapsedOnly = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "BoldMatchesCollapsedOnly" ), mTagTreeBoldMatchesCollapsedOnly, false );
  addItem( itemTagTreeBoldMatchesCollapsedOnly, QString::fromLatin1( "TagTreeBoldMatchesCollapsedOnly" ) );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesTagTreeFilterOperator;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "And" );
    valuesTagTreeFilterOperator.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Or" );
    valuesTagTreeFilterOperator.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemTagTreeFilterOperator;
  itemTagTreeFilterOperator = new KConfigSkeleton::ItemEnum( currentGroup(), QString::fromLatin1( "FilterOperator" ), mTagTreeFilterOperator, valuesTagTreeFilterOperator, EnumTagTreeFilterOperator::And );
  addItem( itemTagTreeFilterOperator, QString::fromLatin1( "TagTreeFilterOperator" ) );
  KConfigSkeleton::ItemBool  *itemTagTreeFilterRegexp;
  itemTagTreeFilterRegexp = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "FilterRegexp" ), mTagTreeFilterRegexp, false );
  addItem( itemTagTreeFilterRegexp, QString::fromLatin1( "TagTreeFilterRegexp" ) );
  KConfigSkeleton::ItemBool  *itemTagTreeRememberTree;
  itemTagTreeRememberTree = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "RememberTree" ), mTagTreeRememberTree, true );
  addItem( itemTagTreeRememberTree, QString::fromLatin1( "TagTreeRememberTree" ) );
  KConfigSkeleton::ItemBool  *itemTagTreeRememberFilter;
  itemTagTreeRememberFilter = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "RememberFilter" ), mTagTreeRememberFilter, true );
  addItem( itemTagTreeRememberFilter, QString::fromLatin1( "TagTreeRememberFilter" ) );
  KConfigSkeleton::ItemBool  *itemTagTreeLocked;
  itemTagTreeLocked = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "Locked" ), mTagTreeLocked, false );
  addItem( itemTagTreeLocked, QString::fromLatin1( "TagTreeLocked" ) );

  setCurrentGroup( QString::fromLatin1( "SourceDirTree" ) );

  KConfigSkeleton::ItemInt  *itemSourceDirTreeToolBarIconSize;
  itemSourceDirTreeToolBarIconSize = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "ToolBarIconSize" ), mSourceDirTreeToolBarIconSize, 16 );
  itemSourceDirTreeToolBarIconSize->setMinValue(16);
  itemSourceDirTreeToolBarIconSize->setMaxValue(32);
  addItem( itemSourceDirTreeToolBarIconSize, QString::fromLatin1( "SourceDirTreeToolBarIconSize" ) );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesSourceDirTreeIconSize;
  KConfigSkeleton::ItemEnum  *itemSourceDirTreeIconSize;
  itemSourceDirTreeIconSize = new KConfigSkeleton::ItemEnum( currentGroup(), QString::fromLatin1( "IconSize" ), mSourceDirTreeIconSize, valuesSourceDirTreeIconSize, 22 );
  itemSourceDirTreeIconSize->setMinValue(16);
  itemSourceDirTreeIconSize->setMaxValue(48);
  addItem( itemSourceDirTreeIconSize, QString::fromLatin1( "SourceDirTreeIconSize" ) );
  KConfigSkeleton::ItemFont  *itemSourceDirTreeFont;
  itemSourceDirTreeFont = new KConfigSkeleton::ItemFont( currentGroup(), QString::fromLatin1( "Font" ), mSourceDirTreeFont, KGlobalSettings::generalFont() );
  addItem( itemSourceDirTreeFont, QString::fromLatin1( "SourceDirTreeFont" ) );
  KConfigSkeleton::ItemBool  *itemSourceDirTreeShowIcons;
  itemSourceDirTreeShowIcons = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "ShowIcons" ), mSourceDirTreeShowIcons, true );
  addItem( itemSourceDirTreeShowIcons, QString::fromLatin1( "SourceDirTreeShowIcons" ) );
  KConfigSkeleton::ItemBool  *itemSourceDirTreeRememberTree;
  itemSourceDirTreeRememberTree = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "RememberTree" ), mSourceDirTreeRememberTree, true );
  addItem( itemSourceDirTreeRememberTree, QString::fromLatin1( "SourceDirTreeRememberTree" ) );
  KConfigSkeleton::ItemBool  *itemSourceDirTreeRememberFilter;
  itemSourceDirTreeRememberFilter = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "RememberFilter" ), mSourceDirTreeRememberFilter, true );
  addItem( itemSourceDirTreeRememberFilter, QString::fromLatin1( "SourceDirTreeRememberFilter" ) );

  setCurrentGroup( QString::fromLatin1( "ImagePreview" ) );

  KConfigSkeleton::ItemInt  *itemImagePreviewSize;
  itemImagePreviewSize = new KConfigSkeleton::ItemInt( currentGroup(), QString::fromLatin1( "Size" ), mImagePreviewSize, 48 );
  itemImagePreviewSize->setMinValue(32);
  itemImagePreviewSize->setMaxValue(160);
  addItem( itemImagePreviewSize, QString::fromLatin1( "ImagePreviewSize" ) );
  KConfigSkeleton::ItemFont  *itemImagePreviewFont;
  itemImagePreviewFont = new KConfigSkeleton::ItemFont( currentGroup(), QString::fromLatin1( "Font" ), mImagePreviewFont, KGlobalSettings::generalFont() );
  addItem( itemImagePreviewFont, QString::fromLatin1( "ImagePreviewFont" ) );
  KConfigSkeleton::ItemBool  *itemImagePreviewShowName;
  itemImagePreviewShowName = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "ShowName" ), mImagePreviewShowName, true );
  addItem( itemImagePreviewShowName, QString::fromLatin1( "ImagePreviewShowName" ) );
  KConfigSkeleton::ItemBool  *itemImagePreviewAutoRefresh;
  itemImagePreviewAutoRefresh = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "AutoRefresh" ), mImagePreviewAutoRefresh, true );
  addItem( itemImagePreviewAutoRefresh, QString::fromLatin1( "ImagePreviewAutoRefresh" ) );
  QValueList<KConfigSkeleton::ItemEnum::Choice> valuesImagePreviewSelectionMode;
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Extended" );
    valuesImagePreviewSelectionMode.append( choice );
  }
  {
    KConfigSkeleton::ItemEnum::Choice choice;
    choice.name = QString::fromLatin1( "Multi" );
    valuesImagePreviewSelectionMode.append( choice );
  }
  KConfigSkeleton::ItemEnum  *itemImagePreviewSelectionMode;
  itemImagePreviewSelectionMode = new KConfigSkeleton::ItemEnum( currentGroup(), QString::fromLatin1( "SelectionMode" ), mImagePreviewSelectionMode, valuesImagePreviewSelectionMode, EnumImagePreviewSelectionMode::Extended );
  addItem( itemImagePreviewSelectionMode, QString::fromLatin1( "ImagePreviewSelectionMode" ) );

  setCurrentGroup( QString::fromLatin1( "FileFilter" ) );

  QStringList defaultFileFilterFileToHandle;
  defaultFileFilterFileToHandle.append( QString::fromUtf8( "*.jpg" ) );
  defaultFileFilterFileToHandle.append( QString::fromUtf8( "*.jpeg" ) );

  KConfigSkeleton::ItemStringList  *itemFileFilterFileToHandle;
  itemFileFilterFileToHandle = new KConfigSkeleton::ItemStringList( currentGroup(), QString::fromLatin1( "FileToHandle" ), mFileFilterFileToHandle, defaultFileFilterFileToHandle );
  addItem( itemFileFilterFileToHandle, QString::fromLatin1( "FileFilterFileToHandle" ) );
  QStringList defaultFileFilterSubdirsToIgnore;
  defaultFileFilterSubdirsToIgnore.append( QString::fromUtf8( ".*" ) );
  defaultFileFilterSubdirsToIgnore.append( QString::fromUtf8( "[Tt]humbs" ) );
  defaultFileFilterSubdirsToIgnore.append( QString::fromUtf8( "[Tt]humbnails" ) );
  defaultFileFilterSubdirsToIgnore.append( QString::fromUtf8( "[Tt]humbNails" ) );

  KConfigSkeleton::ItemStringList  *itemFileFilterSubdirsToIgnore;
  itemFileFilterSubdirsToIgnore = new KConfigSkeleton::ItemStringList( currentGroup(), QString::fromLatin1( "SubdirsToIgnore" ), mFileFilterSubdirsToIgnore, defaultFileFilterSubdirsToIgnore );
  addItem( itemFileFilterSubdirsToIgnore, QString::fromLatin1( "FileFilterSubdirsToIgnore" ) );

  setCurrentGroup( QString::fromLatin1( "Tools" ) );

  KConfigSkeleton::ItemBool  *itemToolsUseInternalImageViewer;
  itemToolsUseInternalImageViewer = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "UseInternalImageViewer" ), mToolsUseInternalImageViewer, true );
  addItem( itemToolsUseInternalImageViewer, QString::fromLatin1( "ToolsUseInternalImageViewer" ) );
  KConfigSkeleton::ItemString  *itemToolsDefaultExternalTool;
  itemToolsDefaultExternalTool = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "DefaultExternalTool" ), mToolsDefaultExternalTool, QString::fromLatin1( "kuickshow" ) );
  addItem( itemToolsDefaultExternalTool, QString::fromLatin1( "ToolsDefaultExternalTool" ) );
  QStringList defaultToolsExternalTools;
  defaultToolsExternalTools.append( QString::fromUtf8( "kuickshow" ) );

  KConfigSkeleton::ItemStringList  *itemToolsExternalTools;
  itemToolsExternalTools = new KConfigSkeleton::ItemStringList( currentGroup(), QString::fromLatin1( "ExternalTools" ), mToolsExternalTools, defaultToolsExternalTools );
  addItem( itemToolsExternalTools, QString::fromLatin1( "ToolsExternalTools" ) );
  KConfigSkeleton::ItemBool  *itemToolsViewerUseSmoothScaling;
  itemToolsViewerUseSmoothScaling = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "ViewerUseSmoothScaling" ), mToolsViewerUseSmoothScaling, false );
  addItem( itemToolsViewerUseSmoothScaling, QString::fromLatin1( "ToolsViewerUseSmoothScaling" ) );
  KConfigSkeleton::ItemBool  *itemToolsViewerShowImageCounter;
  itemToolsViewerShowImageCounter = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "ViewerShowImageCounter" ), mToolsViewerShowImageCounter, true );
  addItem( itemToolsViewerShowImageCounter, QString::fromLatin1( "ToolsViewerShowImageCounter" ) );
  KConfigSkeleton::ItemBool  *itemToolsViewerShowFileInfos;
  itemToolsViewerShowFileInfos = new KConfigSkeleton::ItemBool( currentGroup(), QString::fromLatin1( "ViewerShowFileInfos" ), mToolsViewerShowFileInfos, true );
  addItem( itemToolsViewerShowFileInfos, QString::fromLatin1( "ToolsViewerShowFileInfos" ) );
  KConfigSkeleton::ItemString  *itemToolsViewerOverlayColor;
  itemToolsViewerOverlayColor = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "ViewerOverlayColor" ), mToolsViewerOverlayColor, QString::fromLatin1( "#FFEB10" ) );
  addItem( itemToolsViewerOverlayColor, QString::fromLatin1( "ToolsViewerOverlayColor" ) );

  setCurrentGroup( QString::fromLatin1( "FileSystem" ) );

  KConfigSkeleton::ItemString  *itemFileSystemLastOpenedFile;
  itemFileSystemLastOpenedFile = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "LastOpenedFile" ), mFileSystemLastOpenedFile );
  addItem( itemFileSystemLastOpenedFile, QString::fromLatin1( "FileSystemLastOpenedFile" ) );
  KConfigSkeleton::ItemString  *itemFileSystemLastAddedSourcedir;
  itemFileSystemLastAddedSourcedir = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "LastAddedSourcedir" ), mFileSystemLastAddedSourcedir );
  addItem( itemFileSystemLastAddedSourcedir, QString::fromLatin1( "FileSystemLastAddedSourcedir" ) );
  KConfigSkeleton::ItemString  *itemFileSystemLastExportedToDirectory;
  itemFileSystemLastExportedToDirectory = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "LastExportedToDirectory" ), mFileSystemLastExportedToDirectory );
  addItem( itemFileSystemLastExportedToDirectory, QString::fromLatin1( "FileSystemLastExportedToDirectory" ) );
}

Settings::~Settings()
{
  if ( mSelf == this )
    staticSettingsDeleter.setObject( mSelf, 0, false );
}

