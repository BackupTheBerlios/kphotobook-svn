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
      enum { IDEAlMode, TabPageMode, COUNT };
    };
    class EnumTagTreeFilterOperator
    {
      public:
      enum { And, Or, COUNT };
    };
    class EnumImagePreviewSelectionMode
    {
      public:
      enum { Extended, Multi, COUNT };
    };

    static Settings *self();
    ~Settings();

    /**
      Set Defines the mdi view mode.
    */
    static
    void setGeneralViewMode( int v )
    {
      if (!self()->isImmutable( "GeneralViewMode" ))
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
        if (!self()->isImmutable( "GeneralRescanWhileStartup" ))
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
        Set if a check for untagged Images is performed on exit.
    */
    static
    void setGeneralCheckUntaggedOnExit( bool v )
    {
        if (!self()->isImmutable( "GeneralCheckUntaggedOnExit" ))
            self()->mGeneralCheckUntaggedOnExit = v;
    }

    /**
        Get if a check for untagged images is performed on exit.
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
      if (!self()->isImmutable( "TagTreeToolBarIconSize" ))
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
      if (!self()->isImmutable( "TagTreeIconSize" ))
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
      if (!self()->isImmutable( "TagTreeFont" ))
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
      if (!self()->isImmutable( "TagTreeShowIcons" ))
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
      Set Defines the operator when filtering images by the TagTree.
    */
    static
    void setTagTreeFilterOperator( int v )
    {
      if (!self()->isImmutable( "TagTreeFilterOperator" ))
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
      if (!self()->isImmutable( "TagTreeRememberTree" ))
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
      if (!self()->isImmutable( "TagTreeRememberFilter" ))
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
      if (!self()->isImmutable( "TagTreeLocked" ))
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
      if (!self()->isImmutable( "SourceDirTreeToolBarIconSize" ))
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
      if (!self()->isImmutable( "SourceDirTreeIconSize" ))
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
      if (!self()->isImmutable( "SourceDirTreeFont" ))
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
      if (!self()->isImmutable( "SourceDirTreeShowIcons" ))
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
      if (!self()->isImmutable( "SourceDirTreeRememberTree" ))
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
      if (!self()->isImmutable( "SourceDirTreeRememberFilter" ))
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
      if (!self()->isImmutable( "ImagePreviewSize" ))
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
      if (!self()->isImmutable( "ImagePreviewFont" ))
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
      if (!self()->isImmutable( "ImagePreviewShowName" ))
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
      if (!self()->isImmutable( "ImagePreviewAutoRefresh" ))
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
      if (!self()->isImmutable( "ImagePreviewSelectionMode" ))
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
      if (!self()->isImmutable( "FileFilterFileToHandle" ))
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
      if (!self()->isImmutable( "FileFilterSubdirsToIgnore" ))
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
      set use an external or an internal ImageViewer
    */
    static
    void setToolsUseInternalImageviewer( const bool b )
    {
      if (!self()->isImmutable( "ToolsUseInternalImageViewer" ))
        self()->mToolsUseInternalImageViewer = b;
    }

    /**
      returns, if the internal ImageViewer should be used
    */
    static
    bool toolsUseInternalImageViewer()
    {
      return self()->mToolsUseInternalImageViewer;
    }


    /**
     * @return Image viewer should use SmoothScaling
     */
    static
        bool toolsViewerUseSmoothScale()
    {
        return self()->mToolsViewerUseSmoothScale;
    }

    /**
     * set, if the imageViewer should use SmoothScaling
     */
    static
        void setToolsViewerUseSmoothScale( bool v )
    {
        if (!self()->isImmutable( "ToolsViewerUseSmoothScale" ))
            self()->mToolsViewerUseSmoothScale = v;
    }

    /**
     * @return if the Context Gauge should be shown in the ImageViewer
     */
    static
        bool toolsViewerShowImageCounter()
    {
        return self()->mToolsViewerShowImageCounter;
    }

    /**
     * set, if the ImageCounter should be shown in the image viewer
     */
    static
        void setToolsViewerShowImageCounter( bool v )
    {
        if (!self()->isImmutable( "ToolsViewerShowImageCounter" ))
            self()->mToolsViewerShowImageCounter = v;
    }

    /**
     * @return if the FileInfos should be shown in the ImageViewer
     */
    static
        bool toolsViewerShowFileInfos()
    {
        return self()->mToolsViewerShowFileInfos;
    }

    /**
     * set, if the FileInfos should be shown in the ImageViewer
     */
    static
        void setToolsViewerShowFileInfos( bool v )
    {
        if (!self()->isImmutable( "ToolsViewerShowFileInfos" ))
            self()->mToolsViewerShowFileInfos = v;
    }


    /**
      Set Contains the list of tools an image can be opened with.
    */
    static
    void setToolsExternalTools( const QStringList & v )
    {
      if (!self()->isImmutable( "ToolsExternalTools" ))
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
      Set Contains the external to open an image with when doubleclicked.
    */
    static
    void setToolsDefaultExternalTool( const QString & v )
    {
      if (!self()->isImmutable( "ToolsDefaultExternalTool" ))
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
      Set Contains the last opened kphotobook file.
    */
    static
    void setFileSystemLastOpenedFile( const QString & v )
    {
      if (!self()->isImmutable( "FileSystemLastOpenedFile" ))
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
      if (!self()->isImmutable( "FileSystemLastAddedSourcedir" ))
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
      if (!self()->isImmutable( "FileSystemLastExportedToDirectory" ))
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
    bool mToolsViewerUseSmoothScale;
    bool mToolsViewerShowImageCounter;
    bool mToolsViewerShowFileInfos;
    QString mToolsDefaultExternalTool;
    QStringList mToolsExternalTools;

    // FileSystem
    QString mFileSystemLastOpenedFile;
    QString mFileSystemLastAddedSourcedir;
    QString mFileSystemLastExportedToDirectory;

  private:
};

#endif

