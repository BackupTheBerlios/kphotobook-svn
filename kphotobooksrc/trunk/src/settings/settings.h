// This file is generated by kconfig_compiler from settings.kcfg.
// All changes you do to this file will be lost.
#ifndef SETTINGS_H
#define SETTINGS_H

#include <kconfigskeleton.h>

class Settings : public KConfigSkeleton
{
  public:
    class EnumGeneralViewMode
    {
      public:
      enum type { IDEAlMode, TabPageMode, COUNT };
    };
    class EnumTagTreeFilterOperator
    {
      public:
      enum type { And, Or, COUNT };
    };
    class EnumImagePreviewSelectionMode
    {
      public:
      enum type { Extended, Multi, COUNT };
    };

    static Settings *self();
    ~Settings();

    /**
      Set Defines the mdi view mode.
    */
    static
    void setGeneralViewMode( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "GeneralViewMode" ) ))
        self()->mGeneralViewMode = v;
    }

    /**
      Get Defines the mdi view mode.
    */
    static
    int generalViewMode()
    {
      return self()->mGeneralViewMode;
    }

    /**
      Set Defines if the filesystem is rescanned while starting up KPhotoBook.
    */
    static
    void setGeneralRescanWhileStartup( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "GeneralRescanWhileStartup" ) ))
        self()->mGeneralRescanWhileStartup = v;
    }

    /**
      Get Defines if the filesystem is rescanned while starting up KPhotoBook.
    */
    static
    bool generalRescanWhileStartup()
    {
      return self()->mGeneralRescanWhileStartup;
    }

    /**
      Set Should a check be performed on exit, if images without tags are in the database?.
    */
    static
    void setGeneralCheckUntaggedOnExit( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "GeneralCheckUntaggedOnExit" ) ))
        self()->mGeneralCheckUntaggedOnExit = v;
    }

    /**
      Get Should a check be performed on exit, if images without tags are in the database?.
    */
    static
    bool generalCheckUntaggedOnExit()
    {
      return self()->mGeneralCheckUntaggedOnExit;
    }

    /**
      Set The size of the icons in the ToolBar of the TagTree.
    */
    static
    void setTagTreeToolBarIconSize( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeToolBarIconSize" ) ))
        self()->mTagTreeToolBarIconSize = v;
    }

    /**
      Get The size of the icons in the ToolBar of the TagTree.
    */
    static
    int tagTreeToolBarIconSize()
    {
      return self()->mTagTreeToolBarIconSize;
    }

    /**
      Set The size of the icons in the TagTree.
    */
    static
    void setTagTreeIconSize( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeIconSize" ) ))
        self()->mTagTreeIconSize = v;
    }

    /**
      Get The size of the icons in the TagTree.
    */
    static
    int tagTreeIconSize()
    {
      return self()->mTagTreeIconSize;
    }

    /**
      Set The font used in the TagTree.
    */
    static
    void setTagTreeFont( const QFont & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeFont" ) ))
        self()->mTagTreeFont = v;
    }

    /**
      Get The font used in the TagTree.
    */
    static
    QFont tagTreeFont()
    {
      return self()->mTagTreeFont;
    }

    /**
      Set Defines if the icons in the TagTree are shown.
    */
    static
    void setTagTreeShowIcons( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeShowIcons" ) ))
        self()->mTagTreeShowIcons = v;
    }

    /**
      Get Defines if the icons in the TagTree are shown.
    */
    static
    bool tagTreeShowIcons()
    {
      return self()->mTagTreeShowIcons;
    }

    /**
      Set Should Treenodes, that match an image be drawn bold?
    */
    static
    void setTagTreeBoldMatches( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeBoldMatches" ) ))
        self()->mTagTreeBoldMatches = v;
    }

    /**
      Get Should Treenodes, that match an image be drawn bold?
    */
    static
    bool tagTreeBoldMatches()
    {
      return self()->mTagTreeBoldMatches;
    }

    /**
      Set Should only collapsed Treenodes be drawn bold when the match an image?
    */
    static
    void setTagTreeBoldMatchesCollapsedOnly( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeBoldMatchesCollapsedOnly" ) ))
        self()->mTagTreeBoldMatchesCollapsedOnly = v;
    }

    /**
      Get Should only collapsed Treenodes be drawn bold when the match an image?
    */
    static
    bool tagTreeBoldMatchesCollapsedOnly()
    {
      return self()->mTagTreeBoldMatchesCollapsedOnly;
    }

    /**
      Set Defines the operator when filtering images by the TagTree.
    */
    static
    void setTagTreeFilterOperator( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeFilterOperator" ) ))
        self()->mTagTreeFilterOperator = v;
    }

    /**
      Get Defines the operator when filtering images by the TagTree.
    */
    static
    int tagTreeFilterOperator()
    {
      return self()->mTagTreeFilterOperator;
    }

    /**
      Set Defines if the state of the TagTree (opened/closed nodes) is remebered (stored/restored).
    */
    static
    void setTagTreeRememberTree( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeRememberTree" ) ))
        self()->mTagTreeRememberTree = v;
    }

    /**
      Get Defines if the state of the TagTree (opened/closed nodes) is remebered (stored/restored).
    */
    static
    bool tagTreeRememberTree()
    {
      return self()->mTagTreeRememberTree;
    }

    /**
      Set Defines if the filter of the TagTree is remembered (stored/restored).
    */
    static
    void setTagTreeRememberFilter( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeRememberFilter" ) ))
        self()->mTagTreeRememberFilter = v;
    }

    /**
      Get Defines if the filter of the TagTree is remembered (stored/restored).
    */
    static
    bool tagTreeRememberFilter()
    {
      return self()->mTagTreeRememberFilter;
    }

    /**
      Set Defines if the tagtree is locked or not.
    */
    static
    void setTagTreeLocked( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "TagTreeLocked" ) ))
        self()->mTagTreeLocked = v;
    }

    /**
      Get Defines if the tagtree is locked or not.
    */
    static
    bool tagTreeLocked()
    {
      return self()->mTagTreeLocked;
    }

    /**
      Set The size of the icons in the ToolBar of the SourceDir.
    */
    static
    void setSourceDirTreeToolBarIconSize( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "SourceDirTreeToolBarIconSize" ) ))
        self()->mSourceDirTreeToolBarIconSize = v;
    }

    /**
      Get The size of the icons in the ToolBar of the SourceDir.
    */
    static
    int sourceDirTreeToolBarIconSize()
    {
      return self()->mSourceDirTreeToolBarIconSize;
    }

    /**
      Set The size of the icons in the SourceDirTree.
    */
    static
    void setSourceDirTreeIconSize( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "SourceDirTreeIconSize" ) ))
        self()->mSourceDirTreeIconSize = v;
    }

    /**
      Get The size of the icons in the SourceDirTree.
    */
    static
    int sourceDirTreeIconSize()
    {
      return self()->mSourceDirTreeIconSize;
    }

    /**
      Set The font used in the SourceDirTree.
    */
    static
    void setSourceDirTreeFont( const QFont & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "SourceDirTreeFont" ) ))
        self()->mSourceDirTreeFont = v;
    }

    /**
      Get The font used in the SourceDirTree.
    */
    static
    QFont sourceDirTreeFont()
    {
      return self()->mSourceDirTreeFont;
    }

    /**
      Set Defines if the icons in the SourceDirTree are shown.
    */
    static
    void setSourceDirTreeShowIcons( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "SourceDirTreeShowIcons" ) ))
        self()->mSourceDirTreeShowIcons = v;
    }

    /**
      Get Defines if the icons in the SourceDirTree are shown.
    */
    static
    bool sourceDirTreeShowIcons()
    {
      return self()->mSourceDirTreeShowIcons;
    }

    /**
      Set Defines if the state of the SourceDirTree (opened/closed nodes) is remebered (stored/restored).
    */
    static
    void setSourceDirTreeRememberTree( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "SourceDirTreeRememberTree" ) ))
        self()->mSourceDirTreeRememberTree = v;
    }

    /**
      Get Defines if the state of the SourceDirTree (opened/closed nodes) is remebered (stored/restored).
    */
    static
    bool sourceDirTreeRememberTree()
    {
      return self()->mSourceDirTreeRememberTree;
    }

    /**
      Set Defines if the filter of the SourceDirTree is remembered (stored/restored).
    */
    static
    void setSourceDirTreeRememberFilter( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "SourceDirTreeRememberFilter" ) ))
        self()->mSourceDirTreeRememberFilter = v;
    }

    /**
      Get Defines if the filter of the SourceDirTree is remembered (stored/restored).
    */
    static
    bool sourceDirTreeRememberFilter()
    {
      return self()->mSourceDirTreeRememberFilter;
    }

    /**
      Set The size of the image previews.
    */
    static
    void setImagePreviewSize( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ImagePreviewSize" ) ))
        self()->mImagePreviewSize = v;
    }

    /**
      Get The size of the image previews.
    */
    static
    int imagePreviewSize()
    {
      return self()->mImagePreviewSize;
    }

    /**
      Set The font used in the ImagePreview.
    */
    static
    void setImagePreviewFont( const QFont & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ImagePreviewFont" ) ))
        self()->mImagePreviewFont = v;
    }

    /**
      Get The font used in the ImagePreview.
    */
    static
    QFont imagePreviewFont()
    {
      return self()->mImagePreviewFont;
    }

    /**
      Set Defines if the name of the file is shown.
    */
    static
    void setImagePreviewShowName( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ImagePreviewShowName" ) ))
        self()->mImagePreviewShowName = v;
    }

    /**
      Get Defines if the name of the file is shown.
    */
    static
    bool imagePreviewShowName()
    {
      return self()->mImagePreviewShowName;
    }

    /**
      Set Defines if the preview is refreshed autiomatically if the user changes something.
    */
    static
    void setImagePreviewAutoRefresh( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ImagePreviewAutoRefresh" ) ))
        self()->mImagePreviewAutoRefresh = v;
    }

    /**
      Get Defines if the preview is refreshed autiomatically if the user changes something.
    */
    static
    bool imagePreviewAutoRefresh()
    {
      return self()->mImagePreviewAutoRefresh;
    }

    /**
      Set Defines the selectionmode for the image previews.
    */
    static
    void setImagePreviewSelectionMode( int v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ImagePreviewSelectionMode" ) ))
        self()->mImagePreviewSelectionMode = v;
    }

    /**
      Get Defines the selectionmode for the image previews.
    */
    static
    int imagePreviewSelectionMode()
    {
      return self()->mImagePreviewSelectionMode;
    }

    /**
      Set Contains the regularexpressions for files to handle.
    */
    static
    void setFileFilterFileToHandle( const QStringList & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "FileFilterFileToHandle" ) ))
        self()->mFileFilterFileToHandle = v;
    }

    /**
      Get Contains the regularexpressions for files to handle.
    */
    static
    QStringList fileFilterFileToHandle()
    {
      return self()->mFileFilterFileToHandle;
    }

    /**
      Set Contains the regularexpressions for subdirectories to ignore.
    */
    static
    void setFileFilterSubdirsToIgnore( const QStringList & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "FileFilterSubdirsToIgnore" ) ))
        self()->mFileFilterSubdirsToIgnore = v;
    }

    /**
      Get Contains the regularexpressions for subdirectories to ignore.
    */
    static
    QStringList fileFilterSubdirsToIgnore()
    {
      return self()->mFileFilterSubdirsToIgnore;
    }

    /**
      Set Defines, if the internal image viewer is used to view images, rather then an external tool
    */
    static
    void setToolsUseInternalImageViewer( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ToolsUseInternalImageViewer" ) ))
        self()->mToolsUseInternalImageViewer = v;
    }

    /**
      Get Defines, if the internal image viewer is used to view images, rather then an external tool
    */
    static
    bool toolsUseInternalImageViewer()
    {
      return self()->mToolsUseInternalImageViewer;
    }

    /**
      Set Contains the external to open an image with when doubleclicked.
    */
    static
    void setToolsDefaultExternalTool( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ToolsDefaultExternalTool" ) ))
        self()->mToolsDefaultExternalTool = v;
    }

    /**
      Get Contains the external to open an image with when doubleclicked.
    */
    static
    QString toolsDefaultExternalTool()
    {
      return self()->mToolsDefaultExternalTool;
    }

    /**
      Set Contains the list of tools an image can be opened with.
    */
    static
    void setToolsExternalTools( const QStringList & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ToolsExternalTools" ) ))
        self()->mToolsExternalTools = v;
    }

    /**
      Get Contains the list of tools an image can be opened with.
    */
    static
    QStringList toolsExternalTools()
    {
      return self()->mToolsExternalTools;
    }

    /**
      Set Should the internal viewer use (slower) smooth scaling, or fast scaling algorithms?
    */
    static
    void setToolsViewerUseSmoothScale( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ToolsViewerUseSmoothScale" ) ))
        self()->mToolsViewerUseSmoothScale = v;
    }

    /**
      Get Should the internal viewer use (slower) smooth scaling, or fast scaling algorithms?
    */
    static
    bool toolsViewerUseSmoothScale()
    {
      return self()->mToolsViewerUseSmoothScale;
    }

    /**
      Set Should the image counter be shown in the image viewer?
    */
    static
    void setToolsViewerShowImageCounter( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ToolsViewerShowImageCounter" ) ))
        self()->mToolsViewerShowImageCounter = v;
    }

    /**
      Get Should the image counter be shown in the image viewer?
    */
    static
    bool toolsViewerShowImageCounter()
    {
      return self()->mToolsViewerShowImageCounter;
    }

    /**
      Set Should fileinfos (name etc.) be shown in the image viewer?
    */
    static
    void setToolsViewerShowFileInfos( bool v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ToolsViewerShowFileInfos" ) ))
        self()->mToolsViewerShowFileInfos = v;
    }

    /**
      Get Should fileinfos (name etc.) be shown in the image viewer?
    */
    static
    bool toolsViewerShowFileInfos()
    {
      return self()->mToolsViewerShowFileInfos;
    }

    /**
      Set The color the overlay is drawn with in the image viewer.
    */
    static
    void setToolsViewerOverlayColor( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "ToolsViewerOverlayColor" ) ))
        self()->mToolsViewerOverlayColor = v;
    }

    /**
      Get The color the overlay is drawn with in the image viewer.
    */
    static
    QString toolsViewerOverlayColor()
    {
      return self()->mToolsViewerOverlayColor;
    }

    /**
      Set Contains the last opened kphotobook file.
    */
    static
    void setFileSystemLastOpenedFile( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "FileSystemLastOpenedFile" ) ))
        self()->mFileSystemLastOpenedFile = v;
    }

    /**
      Get Contains the last opened kphotobook file.
    */
    static
    QString fileSystemLastOpenedFile()
    {
      return self()->mFileSystemLastOpenedFile;
    }

    /**
      Set Contains the last added source directory.
    */
    static
    void setFileSystemLastAddedSourcedir( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "FileSystemLastAddedSourcedir" ) ))
        self()->mFileSystemLastAddedSourcedir = v;
    }

    /**
      Get Contains the last added source directory.
    */
    static
    QString fileSystemLastAddedSourcedir()
    {
      return self()->mFileSystemLastAddedSourcedir;
    }

    /**
      Set Contains the last directory images were exported to.
    */
    static
    void setFileSystemLastExportedToDirectory( const QString & v )
    {
      if (!self()->isImmutable( QString::fromLatin1( "FileSystemLastExportedToDirectory" ) ))
        self()->mFileSystemLastExportedToDirectory = v;
    }

    /**
      Get Contains the last directory images were exported to.
    */
    static
    QString fileSystemLastExportedToDirectory()
    {
      return self()->mFileSystemLastExportedToDirectory;
    }

    static
    void writeConfig()
    {
      static_cast<KConfigSkeleton*>(self())->writeConfig();
    }
  protected:
    Settings();
    static Settings *mSelf;


    // General
    int mGeneralViewMode;
    bool mGeneralRescanWhileStartup;
    bool mGeneralCheckUntaggedOnExit;

    // TagTree
    int mTagTreeToolBarIconSize;
    int mTagTreeIconSize;
    QFont mTagTreeFont;
    bool mTagTreeShowIcons;
    bool mTagTreeBoldMatches;
    bool mTagTreeBoldMatchesCollapsedOnly;
    int mTagTreeFilterOperator;
    bool mTagTreeRememberTree;
    bool mTagTreeRememberFilter;
    bool mTagTreeLocked;

    // SourceDirTree
    int mSourceDirTreeToolBarIconSize;
    int mSourceDirTreeIconSize;
    QFont mSourceDirTreeFont;
    bool mSourceDirTreeShowIcons;
    bool mSourceDirTreeRememberTree;
    bool mSourceDirTreeRememberFilter;

    // ImagePreview
    int mImagePreviewSize;
    QFont mImagePreviewFont;
    bool mImagePreviewShowName;
    bool mImagePreviewAutoRefresh;
    int mImagePreviewSelectionMode;

    // FileFilter
    QStringList mFileFilterFileToHandle;
    QStringList mFileFilterSubdirsToIgnore;

    // Tools
    bool mToolsUseInternalImageViewer;
    QString mToolsDefaultExternalTool;
    QStringList mToolsExternalTools;
    bool mToolsViewerUseSmoothScale;
    bool mToolsViewerShowImageCounter;
    bool mToolsViewerShowFileInfos;
    QString mToolsViewerOverlayColor;

    // FileSystem
    QString mFileSystemLastOpenedFile;
    QString mFileSystemLastAddedSourcedir;
    QString mFileSystemLastExportedToDirectory;

  private:
};

#endif

