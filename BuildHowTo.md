# How to Build TabIMSwitch #
You can check out source code of TabIMSwitch and build it by yourself.

## Build Requirement ##
The project is developed with JavaScript and C++. You need the following tools to build it from source code:
  * Visual Studio 2005 Service Pack 1: to compile the C++ XPCOM component
  * [Cygwin](http://www.cygwin.com) environment: to package the XPI file.

If you want to test the extension after building, please refer to the document [How to Debug TabIMSwitch](DebugHowTo.md).

## Get Source Code ##
The project is being development, so you can choose stable source code or current (unstable or experimental) source code to build TabIMSwitch. We recommend you to use the stable source code, because the code have been tested and is stable enough for normal users.

To get stable source code, you can choose these two method:
  1. [Download](http://code.google.com/p/tabimswitch/downloads/list) the zipped source code for specified version named as `tabimswitch-[version]-src.zip`, or
  1. Check out source code from svn repositories, with specified tags.

### Download Zipped Source ###
Downloading zipped source code is much easier if you just want to compile the TabIMSwitch. On the [All Download](http://code.google.com/p/tabimswitch/downloads/list) page, the sources are named as `tabimswitch-[version]-src.zip`. Source code is labeled as _Unstable_ if it is not fully tested. Source code packages without this label is stable version.

Because of storage budget, only recent source code packages are provided. If you want to access very early versions of source code, use the other method.

### Check out Source from SVN ###
Checking out the source code allows you to get any historical version of source code or experimental branches. Also it allows you to join in the development or translation tasks to improve TabIMSwitch.

To check out source code, you must install [SubVersion](http://subversion.tigris.org) first.
Following the installation guide on that website.

At the command line, type the following command to check out the latest source code:
```
svn checkout http://tabimswitch.googlecode.com/svn/trunk/ tabimswitch
```

To check out the tagged source code, for example, the 1.0.0.5 source code, use the following command:
```
svn checkout http://tabimswitch.googlecode.com/svn/tags/release-1.0.0.5/ tabimswitch
```

To check out the experimental branched source code, for example, the experimental branch for Linux SCIM, use the following command:
```
svn checkout http://tabimswitch.googlecode.com/svn/branch/experimental-linux-scim/ tabimswitch
```

You can [browse the SVN repository online](http://tabimswitch.googlecode.com/svn/) for how the source code is organized and select the correct tag or branch to check out.

## Build the Source Code ##

TabIMSwitch includes a XPCOM component written in C++, and some JavaScript code. The first step is building the C++ XPCOM component. In the following guide, $(SRC\_ROOT) will be the root directory of source code.

### Build XPCOM component ###
Use VS 2005 open the tabimswitch.sln file under `$(SRC_ROOT)\src\tabimswitch\tabimswitch.sln`, change the current configuration to "Debug" or "Release", and build the project.

After build, you should see `tabimswitch.dll` and `tabimswitch.xpt` under `$(SRC_ROOT)\components`. Then you can package the XPI installer.

### Build the XPI Package ###
Open Cygwin shell, and change directory to `$(SRC_ROOT)`, type the following command:
```
./build.sh
```

If you can see the tabimswitch.xpi file at `$(SRC_ROOT)`, congratulations, your own version of TabIMSwitch is built!

For more information, please refer to [Mozilla Developer Center](http://developer.mozilla.org).