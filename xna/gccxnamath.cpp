#include "gccxnamath.h"

const XMVECTORF32 g_XMSinCoefficients0    = {1.0f, -0.166666667f, 8.333333333e-3f, -1.984126984e-4f};
const XMVECTORF32 g_XMSinCoefficients1    = {2.755731922e-6f, -2.505210839e-8f, 1.605904384e-10f, -7.647163732e-13f};
const XMVECTORF32 g_XMSinCoefficients2    = {2.811457254e-15f, -8.220635247e-18f, 1.957294106e-20f, -3.868170171e-23f};
const XMVECTORF32 g_XMCosCoefficients0    = {1.0f, -0.5f, 4.166666667e-2f, -1.388888889e-3f};
const XMVECTORF32 g_XMCosCoefficients1    = {2.480158730e-5f, -2.755731922e-7f, 2.087675699e-9f, -1.147074560e-11f};
const XMVECTORF32 g_XMCosCoefficients2    = {4.779477332e-14f, -1.561920697e-16f, 4.110317623e-19f, -8.896791392e-22f};
const XMVECTORF32 g_XMTanCoefficients0    = {1.0f, 0.333333333f, 0.133333333f, 5.396825397e-2f};
const XMVECTORF32 g_XMTanCoefficients1    = {2.186948854e-2f, 8.863235530e-3f, 3.592128167e-3f, 1.455834485e-3f};
const XMVECTORF32 g_XMTanCoefficients2    = {5.900274264e-4f, 2.391290764e-4f, 9.691537707e-5f, 3.927832950e-5f};
const XMVECTORF32 g_XMASinCoefficients0   = {-0.05806367563904f, -0.41861972469416f, 0.22480114791621f, 2.17337241360606f};
const XMVECTORF32 g_XMASinCoefficients1   = {0.61657275907170f, 4.29696498283455f, -1.18942822255452f, -6.53784832094831f};
const XMVECTORF32 g_XMASinCoefficients2   = {-1.36926553863413f, -4.48179294237210f, 1.41810672941833f, 5.48179257935713f};
const XMVECTORF32 g_XMATanCoefficients0   = {1.0f, 0.333333334f, 0.2f, 0.142857143f};
const XMVECTORF32 g_XMATanCoefficients1   = {1.111111111e-1f, 9.090909091e-2f, 7.692307692e-2f, 6.666666667e-2f};
const XMVECTORF32 g_XMATanCoefficients2   = {5.882352941e-2f, 5.263157895e-2f, 4.761904762e-2f, 4.347826087e-2f};
const XMVECTORF32 g_XMSinEstCoefficients  = {1.0f, -1.66521856991541e-1f, 8.199913018755e-3f, -1.61475937228e-4f};
const XMVECTORF32 g_XMCosEstCoefficients  = {1.0f, -4.95348008918096e-1f, 3.878259962881e-2f, -9.24587976263e-4f};
const XMVECTORF32 g_XMTanEstCoefficients  = {2.484f, -1.954923183e-1f, 2.467401101f, XM_1DIVPI};
const XMVECTORF32 g_XMATanEstCoefficients = {7.689891418951e-1f, 1.104742493348f, 8.661844266006e-1f, XM_PIDIV2};
const XMVECTORF32 g_XMASinEstCoefficients = {-1.36178272886711f, 2.37949493464538f, -8.08228565650486e-1f, 2.78440142746736e-1f};
const XMVECTORF32 g_XMASinEstConstants    = {1.00000011921f, XM_PIDIV2, 0.0f, 0.0f};
const XMVECTORF32 g_XMPiConstants0        = {XM_PI, XM_2PI, XM_1DIVPI, XM_1DIV2PI};
const XMVECTORF32 g_XMIdentityR0          = {1.0f, 0.0f, 0.0f, 0.0f};
const XMVECTORF32 g_XMIdentityR1          = {0.0f, 1.0f, 0.0f, 0.0f};
const XMVECTORF32 g_XMIdentityR2          = {0.0f, 0.0f, 1.0f, 0.0f};
const XMVECTORF32 g_XMIdentityR3          = {0.0f, 0.0f, 0.0f, 1.0f};
const XMVECTORF32 g_XMNegIdentityR0       = {-1.0f,0.0f, 0.0f, 0.0f};
const XMVECTORF32 g_XMNegIdentityR1       = {0.0f,-1.0f, 0.0f, 0.0f};
const XMVECTORF32 g_XMNegIdentityR2       = {0.0f, 0.0f,-1.0f, 0.0f};
const XMVECTORF32 g_XMNegIdentityR3       = {0.0f, 0.0f, 0.0f,-1.0f};
const XMVECTORI32 g_XMNegativeZero      = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
const XMVECTORI32 g_XMNegate3           = {0x80000000, 0x80000000, 0x80000000, 0x00000000};
const XMVECTORI32 g_XMMask3             = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000};
const XMVECTORI32 g_XMMaskX             = {0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000};
const XMVECTORI32 g_XMMaskY             = {0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000};
const XMVECTORI32 g_XMMaskZ             = {0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000};
const XMVECTORI32 g_XMMaskW             = {0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF};
const XMVECTORF32 g_XMOne               = { 1.0f, 1.0f, 1.0f, 1.0f};
const XMVECTORF32 g_XMOne3              = { 1.0f, 1.0f, 1.0f, 0.0f};
const XMVECTORF32 g_XMZero              = { 0.0f, 0.0f, 0.0f, 0.0f};
const XMVECTORF32 g_XMNegativeOne       = {-1.0f,-1.0f,-1.0f,-1.0f};
const XMVECTORF32 g_XMOneHalf           = { 0.5f, 0.5f, 0.5f, 0.5f};
const XMVECTORF32 g_XMNegativeOneHalf   = {-0.5f,-0.5f,-0.5f,-0.5f};
const XMVECTORF32 g_XMNegativeTwoPi     = {-XM_2PI, -XM_2PI, -XM_2PI, -XM_2PI};
const XMVECTORF32 g_XMNegativePi        = {-XM_PI, -XM_PI, -XM_PI, -XM_PI};
const XMVECTORF32 g_XMHalfPi            = {XM_PIDIV2, XM_PIDIV2, XM_PIDIV2, XM_PIDIV2};
const XMVECTORF32 g_XMPi                = {XM_PI, XM_PI, XM_PI, XM_PI};
const XMVECTORF32 g_XMReciprocalPi      = {XM_1DIVPI, XM_1DIVPI, XM_1DIVPI, XM_1DIVPI};
const XMVECTORF32 g_XMTwoPi             = {XM_2PI, XM_2PI, XM_2PI, XM_2PI};
const XMVECTORF32 g_XMReciprocalTwoPi   = {XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI};
const XMVECTORF32 g_XMEpsilon           = {1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f};
const XMVECTORI32 g_XMInfinity          = {0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000};
const XMVECTORI32 g_XMQNaN              = {0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000};
const XMVECTORI32 g_XMQNaNTest          = {0x007FFFFF, 0x007FFFFF, 0x007FFFFF, 0x007FFFFF};
const XMVECTORI32 g_XMAbsMask           = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
const XMVECTORI32 g_XMFltMin            = {0x00800000, 0x00800000, 0x00800000, 0x00800000};
const XMVECTORI32 g_XMFltMax            = {0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF};
const XMVECTORI32 g_XMNegOneMask        = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
const XMVECTORI32 g_XMMaskA8R8G8B8      = {0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000};
const XMVECTORI32 g_XMFlipA8R8G8B8      = {0x00000000, 0x00000000, 0x00000000, 0x80000000};
const XMVECTORF32 g_XMFixAA8R8G8B8      = {0.0f,0.0f,0.0f,(float)(0x80000000U)};
const XMVECTORF32 g_XMNormalizeA8R8G8B8 = {1.0f/(255.0f*(float)(0x10000)),1.0f/(255.0f*(float)(0x100)),1.0f/255.0f,1.0f/(255.0f*(float)(0x1000000))};
const XMVECTORI32 g_XMMaskA2B10G10R10   = {0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000};
const XMVECTORI32 g_XMFlipA2B10G10R10   = {0x00000200, 0x00080000, 0x20000000, 0x80000000};
const XMVECTORF32 g_XMFixAA2B10G10R10   = {-512.0f,-512.0f*(float)(0x400),-512.0f*(float)(0x100000),(float)(0x80000000U)};
const XMVECTORF32 g_XMNormalizeA2B10G10R10 = {1.0f/511.0f,1.0f/(511.0f*(float)(0x400)),1.0f/(511.0f*(float)(0x100000)),1.0f/(3.0f*(float)(0x40000000))};
const XMVECTORI32 g_XMMaskX16Y16        = {0x0000FFFF, 0xFFFF0000, 0x00000000, 0x00000000};
const XMVECTORI32 g_XMFlipX16Y16        = {0x00008000, 0x00000000, 0x00000000, 0x00000000};
const XMVECTORF32 g_XMFixX16Y16         = {-32768.0f,0.0f,0.0f,0.0f};
const XMVECTORF32 g_XMNormalizeX16Y16   = {1.0f/32767.0f,1.0f/(32767.0f*65536.0f),0.0f,0.0f};
const XMVECTORI32 g_XMMaskX16Y16Z16W16  = {0x0000FFFF, 0x0000FFFF, 0xFFFF0000, 0xFFFF0000};
const XMVECTORI32 g_XMFlipX16Y16Z16W16  = {0x00008000, 0x00008000, 0x00000000, 0x00000000};
const XMVECTORF32 g_XMFixX16Y16Z16W16   = {-32768.0f,-32768.0f,0.0f,0.0f};
const XMVECTORF32 g_XMNormalizeX16Y16Z16W16 = {1.0f/32767.0f,1.0f/32767.0f,1.0f/(32767.0f*65536.0f),1.0f/(32767.0f*65536.0f)};
const XMVECTORF32 g_XMNoFraction        = {8388608.0f,8388608.0f,8388608.0f,8388608.0f};
const XMVECTORI32 g_XMMaskByte          = {0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};
const XMVECTORF32 g_XMNegateX           = {-1.0f, 1.0f, 1.0f, 1.0f};
const XMVECTORF32 g_XMNegateY           = { 1.0f,-1.0f, 1.0f, 1.0f};
const XMVECTORF32 g_XMNegateZ           = { 1.0f, 1.0f,-1.0f, 1.0f};
const XMVECTORF32 g_XMNegateW           = { 1.0f, 1.0f, 1.0f,-1.0f};
const XMVECTORI32 g_XMSelect0101        = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1};
const XMVECTORI32 g_XMSelect1010        = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};
const XMVECTORI32 g_XMOneHalfMinusEpsilon = { 0x3EFFFFFD, 0x3EFFFFFD, 0x3EFFFFFD, 0x3EFFFFFD};
const XMVECTORI32 g_XMSelect1000        = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_0, XM_SELECT_0};
const XMVECTORI32 g_XMSelect1100        = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
const XMVECTORI32 g_XMSelect1110        = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_1, XM_SELECT_0};
const XMVECTORI32 g_XMSwizzleXYXY       = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0Y};
const XMVECTORI32 g_XMSwizzleXYZX       = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X};
const XMVECTORI32 g_XMSwizzleYXZW       = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_0W};
const XMVECTORI32 g_XMSwizzleYZXW       = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
const XMVECTORI32 g_XMSwizzleZXYW       = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
const XMVECTORI32 g_XMPermute0X0Y1X1Y   = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y};
const XMVECTORI32 g_XMPermute0Z0W1Z1W   = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_1W};
const XMVECTORF32 g_XMFixupY16          = {1.0f,1.0f/65536.0f,0.0f,0.0f};
const XMVECTORF32 g_XMFixupY16W16       = {1.0f,1.0f,1.0f/65536.0f,1.0f/65536.0f};
const XMVECTORI32 g_XMFlipY             = {0,0x80000000,0,0};
const XMVECTORI32 g_XMFlipZ             = {0,0,0x80000000,0};
const XMVECTORI32 g_XMFlipW             = {0,0,0,0x80000000};
const XMVECTORI32 g_XMFlipYZ            = {0,0x80000000,0x80000000,0};
const XMVECTORI32 g_XMFlipZW            = {0,0,0x80000000,0x80000000};
const XMVECTORI32 g_XMFlipYW            = {0,0x80000000,0,0x80000000};
const XMVECTORI32 g_XMMaskHenD3         = {0x7FF,0x7ff<<11,0x3FF<<22,0};
const XMVECTORI32 g_XMMaskDHen3         = {0x3FF,0x7ff<<10,0x7FF<<21,0};
const XMVECTORF32 g_XMAddUHenD3         = {0,0,32768.0f*65536.0f,0};
const XMVECTORF32 g_XMAddHenD3          = {-1024.0f,-1024.0f*2048.0f,0,0};
const XMVECTORF32 g_XMAddDHen3          = {-512.0f,-1024.0f*1024.0f,0,0};
const XMVECTORF32 g_XMMulHenD3          = {1.0f,1.0f/2048.0f,1.0f/(2048.0f*2048.0f),0};
const XMVECTORF32 g_XMMulDHen3          = {1.0f,1.0f/1024.0f,1.0f/(1024.0f*2048.0f),0};
const XMVECTORI32 g_XMXorHenD3          = {0x400,0x400<<11,0,0};
const XMVECTORI32 g_XMXorDHen3          = {0x200,0x400<<10,0,0};
const XMVECTORI32 g_XMMaskIco4          = {0xFFFFF,0xFFFFF000,0xFFFFF,0xF0000000};
const XMVECTORI32 g_XMXorXIco4          = {0x80000,0,0x80000,0x80000000};
const XMVECTORI32 g_XMXorIco4           = {0x80000,0,0x80000,0};
const XMVECTORF32 g_XMAddXIco4          = {-8.0f*65536.0f,0,-8.0f*65536.0f,32768.0f*65536.0f};
const XMVECTORF32 g_XMAddUIco4          = {0,32768.0f*65536.0f,0,32768.0f*65536.0f};
const XMVECTORF32 g_XMAddIco4           = {-8.0f*65536.0f,0,-8.0f*65536.0f,0};
const XMVECTORF32 g_XMMulIco4           = {1.0f,1.0f/4096.0f,1.0f,1.0f/(4096.0f*65536.0f)};
const XMVECTORI32 g_XMMaskDec4          = {0x3FF,0x3FF<<10,0x3FF<<20,0x3<<30};
const XMVECTORI32 g_XMXorDec4           = {0x200,0x200<<10,0x200<<20,0};
const XMVECTORF32 g_XMAddUDec4          = {0,0,0,32768.0f*65536.0f};
const XMVECTORF32 g_XMAddDec4           = {-512.0f,-512.0f*1024.0f,-512.0f*1024.0f*1024.0f,0};
const XMVECTORF32 g_XMMulDec4           = {1.0f,1.0f/1024.0f,1.0f/(1024.0f*1024.0f),1.0f/(1024.0f*1024.0f*1024.0f)};
const XMVECTORI32 g_XMMaskByte4         = {0xFF,0xFF00,0xFF0000,0xFF000000};
const XMVECTORI32 g_XMXorByte4          = {0x80,0x8000,0x800000,0x00000000};
const XMVECTORF32 g_XMAddByte4          = {-128.0f,-128.0f*256.0f,-128.0f*65536.0f,0};
