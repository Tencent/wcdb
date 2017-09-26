//
//  WCTSampleORMColumnConstraint.h
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface WCTSampleORMColumnConstraint : NSObject

@property(nonatomic, assign) int primaryProperty;
@property(nonatomic, retain) NSString *uniqueProperty;
@property(nonatomic, retain) NSData *notNullProperty;

@end
