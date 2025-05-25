
#include <notify-cpp/util.h>
#include <sys/utsname.h> //for utsname and uname
#include <cstdio>
#include <linux/version.h>


namespace notifycpp
{
    uint32_t __kernelVersion;
 

}
__attribute__((constructor)) extern "C" void __populateKernelVersion() 
{
    utsname kernelData;
    uint8_t major,minor,patch;
    uname(&kernelData);
    sscanf(kernelData.release, "%hhu.%hhu.%hhu",&major,&minor,&patch);
    notifycpp::__kernelVersion = KERNEL_VERSION(major,minor,patch);
}
