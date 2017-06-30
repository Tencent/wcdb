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

#import "ViewController.h"
#import "WCTBenchmark.h"
#import "WCTBenchmarkConsole.h"

@interface ViewController ()
@property(weak, nonatomic) IBOutlet UIPickerView *benchmarksPicker;
- (IBAction)run:(id)sender;
@property(weak, nonatomic) IBOutlet UILabel *viewTitle;
@property(weak, nonatomic) IBOutlet UITextView *console;
@property(weak, nonatomic) IBOutlet UIButton *confirm;

@end

@implementation ViewController {
    WCTBenchmark *_entry;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    _entry = [WCTBenchmark benchmark];
    self.benchmarksPicker.dataSource = self;
    self.benchmarksPicker.delegate = self;
    [self.confirm setTitle:@"Running..." forState:UIControlStateDisabled];
    self.viewTitle.text = [NSProcessInfo processInfo].processName;
    self.console.text = nil;
    self.console.editable = NO;
    [[WCTBenchmarkConsole console] registerNotification:^(NSString *log) {
      self.console.text = [NSMutableString stringWithFormat:@"%@%@", self.console.text, log];
      NSRange bottom = NSMakeRange(self.console.text.length - 1, 1);
      [self.console scrollRangeToVisible:bottom];
    }];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return _entry.config.availableBenchmarks.count;
}

- (NSString *)pickerView:(UIPickerView *)pickerView
             titleForRow:(NSInteger)row
            forComponent:(NSInteger)component
{
    return _entry.config.availableBenchmarks[row];
}

- (IBAction)run:(id)sender
{
    self.confirm.enabled = NO;
    self.view.userInteractionEnabled = NO;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
      NSUInteger selected = [self.benchmarksPicker selectedRowInComponent:0];
      _entry.config.benchmark = _entry.config.availableBenchmarks[selected];
      [_entry run];
      self.confirm.enabled = YES;
      self.view.userInteractionEnabled = YES;
    });
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
