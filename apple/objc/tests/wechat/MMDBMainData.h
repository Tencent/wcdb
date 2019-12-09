//
//  MMDBMainData.h
//  MicroMessenger
//
//  Created by alantao on 3/18/15.
//  Copyright (c) 2015 Tencent. All rights reserved.
//

#import <WCDB/WCDB.h>

#pragma mark - contact

@interface DBContact : NSObject<WCTTableCoding>

@property(nonatomic, assign) UInt32 version;
@property(nonatomic, retain) NSString* userName;
@property(nonatomic, retain) NSString* nickName;
@property(nonatomic, assign) UInt32 uin;
@property(nonatomic, retain) NSString* email;
@property(nonatomic, retain) NSString* mobile;
@property(nonatomic, assign) UInt32 sex;
@property(nonatomic, retain) NSString* fullPinYin;
@property(nonatomic, retain) NSData* extent;
@property(nonatomic, retain) NSString* image;
@property(nonatomic, assign) UInt32 type;
@property(nonatomic, retain) NSString* draft;

WCDB_PROPERTY(version)
WCDB_PROPERTY(userName)
WCDB_PROPERTY(nickName)
WCDB_PROPERTY(uin)
WCDB_PROPERTY(email)
WCDB_PROPERTY(mobile)
WCDB_PROPERTY(sex)
WCDB_PROPERTY(fullPinYin)
WCDB_PROPERTY(extent)
WCDB_PROPERTY(image)
WCDB_PROPERTY(type)
WCDB_PROPERTY(draft)

@end


@interface DBContactExt : NSObject<WCTTableCoding>

@property(nonatomic, retain) NSString* userName;
@property(nonatomic, assign) UInt32 type;
@property(nonatomic, retain) NSString* remark;
@property(nonatomic, retain) NSString* remarkShortPinYin;
@property(nonatomic, retain) NSString* remarkFullPinYin;
@property(nonatomic, retain) NSString* QQMBlog;
@property(nonatomic, retain) NSString* SMBlog;
@property(nonatomic, retain) NSString* chatRoomMember;
@property(nonatomic, assign) UInt32 chatStatus;
@property(nonatomic, retain) NSString* domainList;
@property(nonatomic, retain) NSString* extent;

WCDB_PROPERTY(userName)
WCDB_PROPERTY(type)
WCDB_PROPERTY(remark)
WCDB_PROPERTY(remarkShortPinYin)
WCDB_PROPERTY(remarkFullPinYin)
WCDB_PROPERTY(QQMBlog)
WCDB_PROPERTY(SMBlog)
WCDB_PROPERTY(chatRoomMember)
WCDB_PROPERTY(chatStatus)
WCDB_PROPERTY(domainList)
WCDB_PROPERTY(extent)

@end


@interface DBContactMeta : NSObject<WCTTableCoding>

@property(nonatomic, retain) NSString* username;
@property(nonatomic, assign) UInt32 lastUpdate;
@property(nonatomic, assign) UInt32 flag;

WCDB_PROPERTY(username)
WCDB_PROPERTY(lastUpdate)
WCDB_PROPERTY(flag)

@end

#pragma mark - message


@interface DBMessage : NSObject<WCTTableCoding>

@property(nonatomic, assign) UInt32 version;
@property(nonatomic, assign) UInt32 msgLocalId;
@property(nonatomic, assign) SInt64 msgServerId;
@property(nonatomic, assign) UInt32 createTime;
@property(nonatomic, retain) NSString* message;
@property(nonatomic, assign) UInt32 status;
@property(nonatomic, assign) UInt32 imageStatus;
@property(nonatomic, assign) UInt32 type;
@property(nonatomic, assign) UInt32 des;

WCDB_PROPERTY(version)
WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgServerId)
WCDB_PROPERTY(createTime)
WCDB_PROPERTY(message)
WCDB_PROPERTY(status)
WCDB_PROPERTY(imageStatus)
WCDB_PROPERTY(type)
WCDB_PROPERTY(des)

@end


@interface DBMessageExt : NSObject<WCTTableCoding>

@property(nonatomic, assign) UInt32 msgLocalId;
@property(nonatomic, assign) UInt32 msgFlag;
@property(nonatomic, retain) NSString* msgSource;
@property(nonatomic, retain) NSString* msgIdentify;

WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgFlag)
WCDB_PROPERTY(msgSource)
WCDB_PROPERTY(msgIdentify)

@end

@interface DBMessageBizExt : NSObject<WCTTableCoding>

@property(nonatomic, retain) NSString* chatUsername;
@property(nonatomic, assign) UInt32 msgLocalId;
@property(nonatomic, assign) UInt32 msgType;
@property(nonatomic, assign) UInt32 msgInnerType;
@property(nonatomic, retain) NSString* bizId;

WCDB_PROPERTY(chatUsername)
WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgType)
WCDB_PROPERTY(msgInnerType)
WCDB_PROPERTY(bizId)

@end

@interface DBMessageNewBizExt : NSObject<WCTTableCoding>

@property(nonatomic, retain) NSString* chatUsername;
@property(nonatomic, assign) UInt32 msgLocalId;
@property(nonatomic, assign) UInt32 msgType;
@property(nonatomic, assign) UInt32 msgInnerType;
@property(nonatomic, retain) NSString* bizId;
@property(nonatomic, assign) UInt32 status;
@property(nonatomic, assign) UInt32 invalidTime;

WCDB_PROPERTY(chatUsername)
WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgType)
WCDB_PROPERTY(msgInnerType)
WCDB_PROPERTY(bizId)
WCDB_PROPERTY(status)
WCDB_PROPERTY(invalidTime)

@end

/*
@interface DBDuplicateMsg : NSObject<WCTTableCoding>

WCDB_PROPERTY(msgServerId)
WCDB_PROPERTY(createTime)

@end
*/

@interface DBHello : NSObject<WCTTableCoding>

@property(nonatomic, assign) UInt32 version;
@property(nonatomic, assign) UInt32 msgLocalId;
@property(nonatomic, assign) SInt64 msgServerId;
@property(nonatomic, assign) UInt32 createTime;
@property(nonatomic, retain) NSString* message;
@property(nonatomic, assign) UInt32 status;
@property(nonatomic, assign) UInt32 imageStatus;
@property(nonatomic, assign) UInt32 type;
@property(nonatomic, assign) UInt32 des;
@property(nonatomic, retain) NSString* userName;
@property(nonatomic, assign) UInt32 opCode;
@property(nonatomic, assign) UInt32 sayHelloStatus;

WCDB_PROPERTY(version)
WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgServerId)
WCDB_PROPERTY(createTime)
WCDB_PROPERTY(message)
WCDB_PROPERTY(status)
WCDB_PROPERTY(imageStatus)
WCDB_PROPERTY(type)
WCDB_PROPERTY(des)
WCDB_PROPERTY(userName)
WCDB_PROPERTY(opCode)

WCDB_PROPERTY(sayHelloStatus)

@end

#pragma mark - emoticon

@interface DBEmoticon : NSObject<WCTTableCoding>

@property(nonatomic, retain) NSString* md5;
@property(nonatomic, retain) NSString* message;
@property(nonatomic, assign) UInt32 type;
@property(nonatomic, assign) UInt32 createTime;
@property(nonatomic, assign) UInt32 length;
@property(nonatomic, assign) UInt32 status;
@property(nonatomic, retain) NSString* catalog;
@property(nonatomic, assign) UInt32 catalogId;
@property(nonatomic, retain) NSString* draft;

@property(nonatomic, assign) UInt32 lastUsedTime;
@property(nonatomic, assign) UInt32 extFlag;
@property(nonatomic, assign) UInt32 indexInPack;

@property(nonatomic, retain) NSString* extInfo;
@property(nonatomic, retain) NSString* packageId;

WCDB_PROPERTY(md5)
WCDB_PROPERTY(message)
WCDB_PROPERTY(type)
WCDB_PROPERTY(createTime)
WCDB_PROPERTY(length)
WCDB_PROPERTY(status)
WCDB_PROPERTY(catalog)
WCDB_PROPERTY(catalogId)
WCDB_PROPERTY(draft)

WCDB_PROPERTY(lastUsedTime)
WCDB_PROPERTY(extFlag)
WCDB_PROPERTY(indexInPack)

WCDB_PROPERTY(extInfo)
WCDB_PROPERTY(packageId)

@end

#pragma mark - bottle

@interface DBBottle : NSObject<WCTTableCoding>

@property(nonatomic, assign) UInt32 localId;
@property(nonatomic, assign) UInt32 serverId;
@property(nonatomic, retain) NSString* encryptUserName;
@property(nonatomic, retain) NSString* bottleId;
@property(nonatomic, retain) NSString* ext;

WCDB_PROPERTY(localId)
WCDB_PROPERTY(serverId)
WCDB_PROPERTY(encryptUserName)
WCDB_PROPERTY(bottleId)
WCDB_PROPERTY(ext)

@end

@interface DBBottleContact : NSObject<WCTTableCoding>

@property(nonatomic, retain) NSString* userName;
@property(nonatomic, retain) NSString* nickName;
@property(nonatomic, assign) UInt32 sex;
@property(nonatomic, retain) NSString* imageStatus;
@property(nonatomic, retain) NSString* hdImageStatus;
@property(nonatomic, retain) NSString* province;
@property(nonatomic, retain) NSString* city;
@property(nonatomic, retain) NSString* sign;
@property(nonatomic, assign) UInt32 imageKey;
@property(nonatomic, assign) UInt32 imageKeyLast;
@property(nonatomic, assign) UInt32 extKey;
@property(nonatomic, assign) UInt32 extKeyLast;

@property(nonatomic, retain) NSString* ext;

WCDB_PROPERTY(userName)
WCDB_PROPERTY(nickName)
WCDB_PROPERTY(sex)
WCDB_PROPERTY(imageStatus)
WCDB_PROPERTY(hdImageStatus)
WCDB_PROPERTY(province)
WCDB_PROPERTY(city)
WCDB_PROPERTY(sign)
WCDB_PROPERTY(imageKey)
WCDB_PROPERTY(imageKeyLast)
WCDB_PROPERTY(extKey)
WCDB_PROPERTY(extKeyLast)

WCDB_PROPERTY(ext)

@end

