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

在構造函數PackageManagerService中，會檢測所有的安裝在設備中的應用程序，將相關信息存入相關的數據結構中。