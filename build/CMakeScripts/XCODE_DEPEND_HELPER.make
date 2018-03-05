# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.common_lib.Debug:
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Debug/libcommon_lib.a:
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Debug/libcommon_lib.a


PostBuild.yangtze.Debug:
PostBuild.common_lib.Debug: /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Debug/yangtze
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Debug/yangtze:\
	/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Debug/libcommon_lib.a
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Debug/yangtze


PostBuild.common_lib.Release:
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Release/libcommon_lib.a:
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Release/libcommon_lib.a


PostBuild.yangtze.Release:
PostBuild.common_lib.Release: /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Release/yangtze
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Release/yangtze:\
	/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Release/libcommon_lib.a
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Release/yangtze


PostBuild.common_lib.MinSizeRel:
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/MinSizeRel/libcommon_lib.a:
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/MinSizeRel/libcommon_lib.a


PostBuild.yangtze.MinSizeRel:
PostBuild.common_lib.MinSizeRel: /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/MinSizeRel/yangtze
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/MinSizeRel/yangtze:\
	/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/MinSizeRel/libcommon_lib.a
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/MinSizeRel/yangtze


PostBuild.common_lib.RelWithDebInfo:
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/RelWithDebInfo/libcommon_lib.a:
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/RelWithDebInfo/libcommon_lib.a


PostBuild.yangtze.RelWithDebInfo:
PostBuild.common_lib.RelWithDebInfo: /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/RelWithDebInfo/yangtze
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/RelWithDebInfo/yangtze:\
	/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/RelWithDebInfo/libcommon_lib.a
	/bin/rm -f /Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/RelWithDebInfo/yangtze




# For each target create a dummy ruleso the target does not have to exist
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Debug/libcommon_lib.a:
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/MinSizeRel/libcommon_lib.a:
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/RelWithDebInfo/libcommon_lib.a:
/Users/dengxi/Desktop/cp/Yangtze_AutoImageStitch/build/Release/libcommon_lib.a:
