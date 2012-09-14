# PackageManager Code View#

## Content Summary ##

* PackageManager的靜態結構.
* 應用程序包詳細信息的獲取.

## Overview ##

PackageManager是用來獲取安裝在設備上的應用程序包的各種信息的模塊。在應用程式開發過程中很少用到，但是非常重要。在系統啟動的時候，PackageManager會讀取檢測手機上已有的所有應用程序包，將相關信息裝進內存，給應用程序提供信息，亦會將相關信息寫入配置文件。下次啟動的時候會直接讀取配置文件，如應用程序包有變化，再更改相關的配置文件。

## How to use PackageManager in Application ##

和其他服務一樣，我們可以從Context類來獲取PackageManager對象：

	PackageManager pm = context.getPackageManager();

通過pm來調用PackageManager里的接口獲取用戶所需要的信息。這種方法和通常的獲取系統服務的方法是一樣的，唯一的區別的是，獲取PackageManager服務的代碼被封裝在了ActivityThread的getPackageManager方法里：

	public static IPackageManager getPackageManager() {
        if (sPackageManager != null) {
            return sPackageManager;
        }
        IBinder b = ServiceManager.getService("package");
        sPackageManager = IPackageManager.Stub.asInterface(b);
        return sPackageManager;
    }

## The Architecture of PackageManager ##

先看看有關PackageManager的結構圖：

![alt text](pm-architecture.png "Title")

從上圖可知，PackageManager是個abstract interface類，裏面定義了所有PackageManager提供給用戶的接口，但是需要其他的類繼承才能實現抽象類裡的接口，直接繼承他的是ApplicationPackageManager。其實ApplicationPackageManager也僅僅是個wrapper,這個類中有一個IPackageManager變量，他也是一個接口類，但是她是通過IPackageManager.aidl來定義的。AIDL是Android設計的用來做進程間通信的接口。通過編譯，java編譯器會自動將IPackageManager.aidl轉換成IPackageManager.java,在IPackageManager類裏面內嵌了stub類，而stub 繼承自Binder，以及實現了AIDL interface里定義的方法，因此依靠binder來實進程間服務調用。PackageManagerService通過繼承IPackageManager.stub來實現具體PackageManager功能,並且實現了進程間的調用。我們之後重點研究PackageManagerService。

## The details of PackageManagerService ##

### How to launch the PackageManager Service ###

Framework中會運行一個ServerThread,用來啟動android所需要的所有的Service，調用關係如圖：

![alt text](pm-service-launch.png "Title")

如圖所示，在PackageManagerService中有靜態方法main來通過調用PackageManagerService的構造函數來初始化此service：

	public static final IPackageManager main(Context context, boolean factoryTest, boolean onlyCore) {
    	PackageManagerService m = new PackageManagerService(context, factoryTest, onlyCore);
    	ServiceManager.addService("package", m);
    	return m;
	}

### PackageManagerService構造函數 ###

在構造函數PackageManagerService中，會檢測所有的安裝在設備中的應用程序，將相關信息存入相關的數據結構中。我們先來看一下PackageManagerService的構造函數的運行流程概覽：

![alt text](PackageManagerConstructor.png "Title")

我們逐步分析各個子過程：

* Installer對象的創建
* 建立PackageHandler消息循环
* readPermissions流程
* Settings對象的創建與讀取
* java binary的DexOpt
* 用AppDirObserver來監視app目錄
* scanDirLI流程
* Other Operations

### 1. Installer對象的創建 ###

Installer是一個單獨的類，他主要通過socket與C語言編寫的installd程序交互，主要實現apk安裝卸載，dex的優化。Installer負責通過socket往installd發送消息，而具體操作都是在installd中完成。命令行程序installd執行的命令如下：

	struct cmdinfo cmds[] = {
    	{ "ping",             0, do_ping },
        { "install",          3, do_install },
        { "dexopt",           3, do_dexopt },
        { "movedex",          2, do_move_dex },
        { "rmdex",            1, do_rm_dex },
        { "remove",           1, do_remove },
        { "rename",           2, do_rename },
        { "freecache",        1, do_free_cache },
        { "rmcache",          1, do_rm_cache },
        { "protect",          2, do_protect },
        { "getsize",          3, do_get_size },
        { "rmuserdata",       1, do_rm_user_data },
        { "movefiles",        0, do_movefiles },
     };

PackageManagerService也不是直接使用Installer對象，而是通過UserManager間接使用，在UserManager調用構造函數的時候將Installer對象傳進去：

	mInstaller = new Installer();
	......
    mUserManager = new UserManager(mInstaller, mUserAppDataDir);

在PackageManagerService的其他部份將會通過mUserManager來調用Installer相關的功能。

### 2. 建立PackageHandler消息循环 ###

代碼如下：

	mHandlerThread.start();
    mHandler = new PackageHandler(mHandlerThread.getLooper());

mHandlerThread是HandlerThread的實例化，HandlerThread繼承自Thread，根據文檔介紹，HandlerThread對於run函數有自己特殊的實現，通過mHandlerThread.getLooper()獲得的Looper傳遞給Handler的構造函數，這樣的話，mHandler的消息處理實際就在HandlerThread里運行了。另外PackageManagerService的Handler所處理的消息如下：

	static final int SEND_PENDING_BROADCAST = 1;
    static final int MCS_BOUND = 3;
    static final int END_COPY = 4;
    static final int INIT_COPY = 5;
    static final int MCS_UNBIND = 6;
    static final int START_CLEANING_PACKAGE = 7;
    static final int FIND_INSTALL_LOC = 8;
    static final int POST_INSTALL = 9;
    static final int MCS_RECONNECT = 10;
    static final int MCS_GIVE_UP = 11;
    static final int UPDATED_MEDIA_STATUS = 12;
    static final int WRITE_SETTINGS = 13;
    static final int WRITE_STOPPED_PACKAGES = 14;
    static final int PACKAGE_VERIFIED = 15;
    static final int CHECK_PENDING_VERIFICATION = 16;

### 3. readPermissions流程 ###