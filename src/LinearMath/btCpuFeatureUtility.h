
#ifndef BT_CPU_UTILITY_H
#define BT_CPU_UTILITY_H

#include "LinearMath/btScalar.h"

#include <string.h>//memset
#ifdef  BT_USE_SSE
#if (_MSC_FULL_VER >= 160040219)
#include <intrin.h>
#endif
#endif

#if defined BT_USE_NEON
#define ARM_NEON_GCC_COMPATIBILITY  1
#include <arm_neon.h>
#include <sys/types.h>
#include <sys/sysctl.h> //for sysctlbyname
#endif //BT_USE_NEON

///Rudimentary btCpuFeatureUtility for CPU features: only report the features that Bullet actually uses (SSE4/FMA3, NEON_HPFP)
///We assume SSE2 in case BT_USE_SSE2 is defined in LinearMath/btScalar.h
class btCpuFeatureUtility
{
public:
	enum btCpuFeature
	{
		CPU_FEATURE_FMA3=1,
		CPU_FEATURE_SSE4_1=2,
		CPU_FEATURE_NEON_HPFP=4
	};

	static int getCpuFeatures(btCpuFeature inFeature)
	{

		static int capabilities = 0;
		static bool testedCapabilities = false;
		if (0 != testedCapabilities)
		{
			return capabilities;
		}

#ifdef BT_USE_NEON
		{
			uint32_t hasFeature = 0;
			size_t featureSize = sizeof(hasFeature);
			int err = sysctlbyname("hw.optional.neon_hpfp", &hasFeature, &featureSize, NULL, 0);
			if (0 == err && hasFeature)
				capabilities |= CPU_FEATURE_NEON_HPFP;
		}
#endif //BT_USE_NEON

#ifdef  BT_USE_SSE
#if (_MSC_FULL_VER >= 160040219)
		{
			int					cpuInfo[4];
			memset(cpuInfo, 0, sizeof(cpuInfo));
			unsigned long long	sseExt;
			__cpuid(mCpuInfo, 1);
			mExt = _xgetbv(0);

			const int OSXSAVEFlag = (1UL << 27);
			const int AVXFlag = ((1UL << 28) | OSXSAVEFlag);
			const int FMAFlag = ((1UL << 12) | AVXFlag | OSXSAVEFlag);
			if ((mCpuInfo[2] & FMAFlag) == FMAFlag && (mExt & 6) == 6)
			{
				capabilities |= btCpuFeatureUtility::CPU_FEATURE_FMA3;
			}

			const int SSE41Flag = (1 << 19);
			if (mCpuInfo[2] & SSE41Flag)
			{
				capabilities |= btCpuFeatureUtility::CPU_FEATURE_SSE4_1;
			}
		}
#endif//(_MSC_FULL_VER >= 160040219)
#endif//BT_USE_SSE

		testedCapabilities = true;
		return capabilities;
	}


};


#endif //BT_CPU_UTILITY_H
