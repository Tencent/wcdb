//
//  WCTSampleORMTableConstraint.h
//  Sample
//
//  Created by sanhuazhang on 25/09/2017.
//  Copyright © 2017 sanhuazhang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface WCTSampleORMTableConstraint : NSObject

@property(nonatomic, assign) int primaryKeyPart1;
@property(nonatomic, retain) NSString *primaryKeyPart2;

@property(nonatomic, assign) int uniqueKeyPart1;
@property(nonatomic) float uniqueKeyPart2;

@end
