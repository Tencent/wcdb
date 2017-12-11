/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "WCTBenchmarkDeviceInfo.h"
#import <sys/sysctl.h>

@implementation WCTBenchmarkDeviceInfo

+ (instancetype)currentDeviceInfo
{
    static WCTBenchmarkDeviceInfo *s_deviceInfo;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
      s_deviceInfo = [[WCTBenchmarkDeviceInfo alloc] init];
    });
    return s_deviceInfo;
}

- (instancetype)init
{
    if (self = [super init]) {
        _model = [self sysctlStringForKey:@"hw.model"];
        NSOperatingSystemVersion osVersion = [[NSProcessInfo processInfo] operatingSystemVersion];
        _osVersion = [NSString stringWithFormat:@"%lu.%lu.%lu", (long) osVersion.majorVersion, (long) osVersion.minorVersion, (long) osVersion.patchVersion];
        _cpuFrequency = [self sysctlCGFloatForKey:@"hw.cpufrequency"] / 1000000000.0f;
        _numberOfCores = [self sysctlCGFloatForKey:@"hw.ncpu"];
        _physicalMemory = [[NSProcessInfo processInfo] physicalMemory] / 1024 / 1024;
#ifdef DEBUG
        _isInDebugMode = YES;
#else
        _isInDebugMode = [self isBeingDebugged];
#endif
    }
    return self;
}

- (BOOL)isBeingDebugged
{
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()};
    struct kinfo_proc info;
    info.kp_proc.p_flag = 0;
    size_t size = sizeof(info);
    int junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
    assert(junk == 0);
    return ((info.kp_proc.p_flag & P_TRACED) != 0);
}

- (NSString *)sysctlStringForKey:(NSString *)key
{
    NSString *sysctl = nil;
    size_t length = 0;
    sysctlbyname(key.UTF8String, NULL, &length, NULL, 0);
    if (length) {
        char *result = malloc(length * sizeof(char));
        sysctlbyname(key.UTF8String, result, &length, NULL, 0);
        sysctl = @(result);
        free(result);
    }
    return sysctl;
}

- (float)sysctlCGFloatForKey:(NSString *)key
{
    float sysctl = 0;
    size_t length = 0;
    sysctlbyname(key.UTF8String, NULL, &length, NULL, 0);
    if (length) {
        char *result = malloc(length * sizeof(char));
        sysctlbyname(key.UTF8String, result, &length, NULL, 0);
        switch (length) {
            case 8:
                sysctl = (float) *(int64_t *) result;
                break;
            case 4:
                sysctl = (float) *(int32_t *) result;
                break;
            default:
                sysctl = 0.;
                break;
        }
        free(result);
    }
    return sysctl;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"Model: %@\n"
                                       "OS Version: %@\n"
                                       "CPU Frequency: %.2f GHz\n"
                                       "Number of cores: %d\n"
                                       "Phsical Memory: %.0f MB\n"
                                       "Is In Debug Mode: %@",
                                      self.model,
                                      self.osVersion,
                                      self.cpuFrequency,
                                      self.numberOfCores,
                                      self.physicalMemory,
                                      self.isInDebugMode ? @"YES" : @"NO"];
}

- (NSDictionary *)encodedResult
{
    NSMutableDictionary *dictionary = [[NSMutableDictionary alloc] init];
    [dictionary setObject:self.model forKey:@"Model"];
    [dictionary setObject:self.osVersion forKey:@"OS Version"];
    [dictionary setObject:[NSString stringWithFormat:@"%.2f GHz", self.cpuFrequency] forKey:@"CPU Frequency"];
    [dictionary setObject:@(self.numberOfCores) forKey:@"Number of Cores"];
    [dictionary setObject:[NSString stringWithFormat:@"%.0f MB", self.physicalMemory] forKey:@"Physical Memory"];
    [dictionary setObject:@(self.isInDebugMode) forKey:@"Is In Debug Mode"];
    return dictionary;
}

@end
