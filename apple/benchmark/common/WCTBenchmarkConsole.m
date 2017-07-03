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

#import "WCTBenchmarkConsole.h"

@implementation WCTBenchmarkConsole {
    WCTBenchmarkConsoleNotify _notify;
}

+ (instancetype)console
{
    static WCTBenchmarkConsole *s_console;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
      s_console = [[WCTBenchmarkConsole alloc] init];
    });
    return s_console;
}

- (void)print:(NSString *)log
{
    log = [NSString stringWithFormat:@"%@\n", log];
    printf("%s", log.UTF8String);
    if (_notify) {
        _notify(log);
    }
}

- (void)registerNotification:(WCTBenchmarkConsoleNotify)notify
{
    _notify = notify;
}

@end
