//
//  MMDBMainData.h
//  MicroMessenger
//
//  Created by alantao on 3/18/15.
//  Copyright (c) 2015 Tencent. All rights reserved.
//

#import <WCDB/WCDB.h>

enum enMMDataType {
    MM_DATA_TEXT = 1, // 文本类型
    //MM_DATA_HTML=2, // 未使用,已废弃
    MM_DATA_IMG = 3,              // 图片类型
    MM_DATA_PRIVATEMSG_TEXT = 11, // 私信文本
    //MM_DATA_PRIVATEMSG_HTML=12, // 未使用,已废弃
    MM_DATA_PRIVATEMSG_IMG = 13,   // 私信图片
    MM_DATA_CHATROOMMSG_TEXT = 21, // 被客户端误用，兼容
    //MM_DATA_CHATROOMMSG_HTML=22, // 未使用,已废弃
    MM_DATA_CHATROOMMSG_IMG = 23, // 被客户端误用，兼容
    //MM_DATA_EMAILMSG_TEXT=31,    // 未使用,已废弃
    //MM_DATA_EMAILMSG_HTML=32,    // 未使用,已废弃
    //MM_DATA_EMAILMSG_IMG=33,     // 未使用,已废弃
    MM_DATA_VOICEMSG = 34,            // 语音类型
    MM_DATA_PUSHMAIL = 35,            // pushmail类型
    MM_DATA_QMSG = 36,                // QQ离线消息文本
    MM_DATA_VERIFYMSG = 37,           // 好友验证类型
    MM_DATA_PUSHSYSTEMMSG = 38,       // 广告消息类型
    MM_DATA_QQLIXIANMSG_IMG = 39,     // QQ离线消息图片
    MM_DATA_POSSIBLEFRIEND_MSG = 40,  // 好友推荐类型
    MM_DATA_PUSHSOFTWARE = 41,        // 精品软件推荐类型
    MM_DATA_SHARECARD = 42,           // 名片分享类型
    MM_DATA_VIDEO = 43,               // 视频类型
    MM_DATA_VIDEO_IPHONE_EXPORT = 44, // 转发视频类型
    MM_DATA_GMAIL_PUSHMAIL = 45,      // Gmail pushmail类型
    MM_DATA_EMPTY = 46,               //客户端要求，占坑
    MM_DATA_EMOJI = 47,               // 自定义表情类型
    MM_DATA_LOCATION = 48,            // 位置消息类型
    MM_DATA_APPMSG = 49,              // AppMsg
    MM_DATA_VOIP_INVITE = 50,         //voipinvite
    MM_DATA_STATUSNOTIFY = 51,        //web和手机状态同步消息
    MM_DATA_WEIBOPUSH = 52,           // Weibo Push( MMReader )
    MM_DATA_WEBWXVOIPNOTIFY = 53,     // webwx voip notify
    MM_DATA_CHATROOMVERIFYMSG = 54,   //申请加入群验证消息
    MM_DATA_BRAND_QA_ASK = 55,        //公众平台问答 提问
    MM_DATA_TALK_SYSMSG = 56,         //对讲模式的系统消息
    MM_DATA_BRAND_QA_MSG = 57,        //公众平台问答 普通消息
    MM_DATA_OPEN_SUBSCRIBE = 58,      //开放平台 订阅消息
    MM_DATA_OPEN_REPORT = 59,         //开放平台 报告消息
    MM_DATA_OPEN_LINK = 60,           //开放平台 报告消息
    MM_DATA_KF = 61,                  //客服消息
    //MM_DATA_TRACK_SYSMSG=61, //追踪模式的系统消息
    MM_DATA_SHORT_VIDEO = 62, //短视频自动播放消息类型
    MM_DATA_YO = 63,          //watch 戳一下找人
    MM_DATA_MULTI_TALK = 64,  //多人通话消息
    MM_DATA_THIRD_PARTY_VERIFYMSG = 65,
    MM_DATA_TP_SHARECARD = 66,
    MM_DATA_TP_KEFU_SHARE_CARD = 67,
    MM_DATA_RING_BACK_DISPLAY = 9997,
    MM_DATA_SYSCMD_IPXX = 9998,    // 用于查询问题通知客户端上传日志
    MM_DATA_SYSNOTICE = 9999,      // 系统通知类型
    MM_DATA_SYS = 10000,           // 系统消息，出现在会话中间
    MM_DATA_SYSCMD_XML = 10001,    //系统命令XML消息，客户端只处理，不显示
    MM_DATA_SYSCMD_NEWXML = 10002, //系统命令XML消息，客户端只处理，不显示
};

typedef NS_ENUM(NSInteger, enMMAppMsgInnerType) {
    MM_APP_TEXT = 1,
    MM_APP_IMG = 2,
    MM_APP_MUSIC = 3,
    MM_APP_VIDEO = 4,
    MM_APP_URL = 5,
    MM_APP_FILE = 6,
    MM_APP_OPEN = 7,
    MM_APP_EMOJI = 8,
    MM_APP_VOICE_REMIND = 9,
    MM_APP_PRODUCT = 10,
    MM_APP_EMOTICON_GIFT = 11,
    MM_APP_DEVICE_ACCESS = 12,
    MM_APP_GENERAL_PRODUCT = 13,
    MM_APP_OLD_TV = 14, // 摇一摇电视旧版、废弃
    MM_APP_EMOTION = 15,
    MM_APP_CARD = 16,
    MM_APP_TRACKROOM = 17,
    MM_APP_QQMSG = 18,
    MM_APP_RECORD = 19,
    MM_APP_TV = 20,
    MM_APP_HARDWARE = 21,
    MM_APP_WCSTORE_PRODUCT = 22,
    MM_APP_IWATCHMSG = 23,
    MM_APP_NOTE = 24,
    MM_APP_EMOTION_DESIGNER = 25,
    MM_APP_EMOTION_EMOTIONLIST = 26,
    MM_APP_EMOTION_EMOJILIST = 27,
    MM_APP_NOTE_SHARE = 31, //弃用
    MM_APP_APPBRAND = 33,
    MM_APP_GIFTCARD = 34,
    MM_APP_SYNC_RECORD = 35, // 聊天记录同步
    MM_APP_OPENSDK_APPBRAND = 36,
    MM_APP_MESSAGE_OBJECT_VIDEO_VILE = 38, // opensdk分享video视频文件消息类型
    MM_APP_DOWNLOAD_BIG_APPMSG = 40,       // 超过64k的appmsg用这种类型来下载
    MM_APP_MMSCHEDULE = 41,
    MM_APP_APPBRAND_NATIVE = 44,      // 仿原生小程序分享出的消息
    MM_APP_APPBRAND_WEISHIVIDEO = 46, // 微视互动视频
    MM_APP_LINK_TEXT = 47,            //可带链接的文本app消息
    MM_APP_APPBRAND_RECORD_VIDEO = 48,
    MM_APP_SOLITAIRE_TEXT = 53, // 接龙
    MM_APP_BRAND_MP_VIDEO = 54,
    MM_APP_MSG_REFER = 57,          // 消息引用
    MM_APP_FINDER_NAME_CARD = 50,   ///< 圈外: 个人名片
    MM_APP_FINDER_FEED = 51,        ///< 圈外: Feed
    MM_APP_Finder_Endorsement = 52, ///< 圈外: 认证
    MM_APP_FINDER_TOPIC = 59,       ///< 圈外: 话题
    MM_APP_MMLIVE_SHARE = 60,       // 群直播分享
    MM_APP_APPBRAND_FAV_MSG = 61,   //用于我的小程序消息盒子
    MM_APP_PAT = 62,                // 拍一拍聚合消息
    MM_APP_FINDER_LIVE = 63,        ///<  圈外: 直播Feed
    MM_APP_FINDER_BOOKING = 64,     ///< 圈外: 直播预约
    MM_APP_FINDER_INVITE = 65,      ///< 圈外: 直播连麦邀请
    MM_APP_GAMELIFE_GIFT = 66,      // 游戏人生赠礼消息
    MM_APP_LITEAPP = 68,            // liteapp分享
    MM_APP_GAMELIFE_NAME_CARD = 69, //游戏名片消息
    MM_APP_FINDER_VIDEO_SHARE = 71, //圈外/视频号: 长视频分享
    MM_APP_FINDER_NEW_TOPIC = 72,   //圈外/视频号: 音乐聚合页分享
    MM_APP_FINDER_COLUMN = 73,      //圈外/视频号: 专栏分享
    MM_APP_FILE_UPLOADING = 74,     // 文件消息壳子（对方正在上传）
    MM_APP_FINDER_AGGREGATE = 75,
    MM_APP_MUSIC_MV = 76,
    MM_APP_FESTIVALWISH = 77,                        // 2021新年许愿活动
    MM_APP_NotifyMsg = 80,                           // 服务通知消息
    MM_APP_FINDER_GUARANTEE_CERTIFICATION = 81,      // 视频号担保认证
    MM_APP_FINDER_PRODUCT_SHARE = 82,                //视频号商品分享
    MM_APP_TEENAGER_MODE_REQUEST_BIND_GUARDIAN = 83, // 青少年模式监护人申请
    MM_APP_TEENAGER_MODE_REQUEST_AUTHORIZATION = 84, // 青少年向监护人申请临时授权
    MM_APP_GROUP_NOTICE = 87,                        //新群公告
    MM_APP_FINDER_PURCHASE_LIVE = 88,                // 付费直播

    MM_APP_MUSIC_MV_PURCHASE = 89,            // 付费音乐MV
    MM_APP_FINDER_PURCHASE_LIVE_LOTTERY = 89, // 直播抽奖 同样是 89
    /* MM_APP_MUSIC_MV_PURCHASE 仅用于听一听业务后台拦截逻辑
     客户端收到的 89 的 appMsg 表示 MM_APP_FINDER_PURCHASE_LIVE_LOTTERY，展示直播抽奖相关逻辑 */

    MM_APP_MINI_GAME = 91,

    MM_APP_TING_AUDIO = 92,   // 听一听音频
    MM_APP_TING_LIST = 93,    // 听一听列表
    MM_APP_TING_LYRICS = 104, // 听一听列表

    MM_APP_FINDER_SHOP_PROFILE = 94, ///<（旧版本, 已废弃, 仅用于兼容旧版本）视频号商品橱窗卡片
    MM_APP_FINDER_LINK_TEXT = 95,    // 视频号带h5富文本跳转私信消息
    MM_APP_FINDER_ORDER = 96,        //视频号电商订单

    MM_APP_EMOTION_EMOTIONSET = 97, //表情合集分享
    MM_APP_ASSET_UNION = 98,        //资源合集
    MM_APP_EXTERNAL_GAME_SHARE = 101,
    MM_APP_TEENAGER_MODE_REQUEST_CHANGE_GUARDIAN = 102,   // 青少年模式更换监护人
    MM_APP_TEENAGER_MODE_REQUEST_AUTHORIZATION_NEW = 103, // 青少年向监护人申请授权同意（通用wording）
    MM_APP_FINDER_MEMBER_NAME_CARD = 105,                 ///< 视频号会员专区
    MM_APP_FINDER_MEMBER_FEED = 106,                      ///< 视频号会员视频（直播回放）

    MM_APP_ILINK_MULTI_TALK = 107,       ///< ilink 双人切成多人房间后的消息
    MM_APP_FINDER_SHOP_PROFILE_V2 = 111, ///< 视频号商品橱窗卡片
    MM_APP_PAY_POCKET_MONEY = 110,       // 支付零花钱气泡消息需求， https://tapd.woa.com/allowance_weixin/prong/stories/view/1070089380886228395

    MM_APP_FINDER_SERVICE_NOTIFY_LIVE_SUBSCRIPTION = 112, // 服务通知 视频号直播业务订阅

    //    MM_APP_ASSET_UNION = MM_APP_RECORD,
    // ⚠️ 新增type插在这上面
    /* !!!: 本枚举值请确保先在后台对应文件定义 */
    /* !!!: 仅客户端本地使用的也需要在后台定义 */

    MM_APP_UNKNOWN_TYPE_BEGIN, // 从这个值开始的是当前版本不认识的

    MM_APP_RING_BACK_DISPLAY = 996,     //彩铃
    MM_APP_READER_MESSAGE_FOLDER = 997, //模板消息聚合
    MM_APP_SCANCODE_RETRY = 998,        //扫一扫稍后重试本地消息，只在本地生成与推送
    MM_APP_POI = 999,

    /* define of MM_APP_ is [ 1, 999 ]*/

    // 2022年T3 微信键盘互通需求第二期占用100000-199999
};

#pragma mark - contact

@interface DBContact : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 version;
@property (nonatomic, retain) NSString* userName;
@property (nonatomic, retain) NSString* nickName;
@property (nonatomic, assign) UInt32 uin;
@property (nonatomic, retain) NSString* email;
@property (nonatomic, retain) NSString* mobile;
@property (nonatomic, assign) UInt32 sex;
@property (nonatomic, retain) NSString* fullPinYin;
@property (nonatomic, retain) NSData* extent;
@property (nonatomic, retain) NSString* image;
@property (nonatomic, assign) UInt32 type;
@property (nonatomic, retain) NSString* draft;

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

@interface DBContactExt : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString* userName;
@property (nonatomic, assign) UInt32 type;
@property (nonatomic, retain) NSString* remark;
@property (nonatomic, retain) NSString* remarkShortPinYin;
@property (nonatomic, retain) NSString* remarkFullPinYin;
@property (nonatomic, retain) NSString* QQMBlog;
@property (nonatomic, retain) NSString* SMBlog;
@property (nonatomic, retain) NSString* chatRoomMember;
@property (nonatomic, assign) UInt32 chatStatus;
@property (nonatomic, retain) NSString* domainList;
@property (nonatomic, retain) NSString* extent;

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

@interface DBContactMeta : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString* username;
@property (nonatomic, assign) UInt32 lastUpdate;
@property (nonatomic, assign) UInt32 flag;

WCDB_PROPERTY(username)
WCDB_PROPERTY(lastUpdate)
WCDB_PROPERTY(flag)

@end

#pragma mark - message

@interface DBMessage : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 version;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) SInt64 msgServerId;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, retain) NSString* message;
@property (nonatomic, assign) UInt32 status;
@property (nonatomic, assign) UInt32 imageStatus;
@property (nonatomic, assign) UInt32 type;
@property (nonatomic, assign) UInt32 des;

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

@interface DBMessageBlob : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 version;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) SInt64 msgServerId;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, retain) NSData* message;
@property (nonatomic, assign) UInt32 status;
@property (nonatomic, assign) UInt32 imageStatus;
@property (nonatomic, assign) UInt32 type;
@property (nonatomic, assign) UInt32 des;

- (instancetype)initFromDBMessage:(DBMessage*)msg;

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

@interface DBMessageExt : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 msgFlag;
@property (nonatomic, retain) NSString* msgSource;
@property (nonatomic, retain) NSString* msgIdentify;

WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgFlag)
WCDB_PROPERTY(msgSource)
WCDB_PROPERTY(msgIdentify)

@end

@interface DBMessageExtBlob : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 msgFlag;
@property (nonatomic, retain) NSData* msgSource;
@property (nonatomic, retain) NSString* msgIdentify;

- (instancetype)initFromDBMessageExt:(DBMessageExt*)msg;

WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgFlag)
WCDB_PROPERTY(msgSource)
WCDB_PROPERTY(msgIdentify)

@end

@interface DBMessageBizExt : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString* chatUsername;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 msgType;
@property (nonatomic, assign) UInt32 msgInnerType;
@property (nonatomic, retain) NSString* bizId;

WCDB_PROPERTY(chatUsername)
WCDB_PROPERTY(msgLocalId)
WCDB_PROPERTY(msgType)
WCDB_PROPERTY(msgInnerType)
WCDB_PROPERTY(bizId)

@end

@interface DBMessageNewBizExt : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString* chatUsername;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) UInt32 msgType;
@property (nonatomic, assign) UInt32 msgInnerType;
@property (nonatomic, retain) NSString* bizId;
@property (nonatomic, assign) UInt32 status;
@property (nonatomic, assign) UInt32 invalidTime;

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

@interface DBHello : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 version;
@property (nonatomic, assign) UInt32 msgLocalId;
@property (nonatomic, assign) SInt64 msgServerId;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, retain) NSString* message;
@property (nonatomic, assign) UInt32 status;
@property (nonatomic, assign) UInt32 imageStatus;
@property (nonatomic, assign) UInt32 type;
@property (nonatomic, assign) UInt32 des;
@property (nonatomic, retain) NSString* userName;
@property (nonatomic, assign) UInt32 opCode;
@property (nonatomic, assign) UInt32 sayHelloStatus;

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

@interface DBEmoticon : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString* md5;
@property (nonatomic, retain) NSString* message;
@property (nonatomic, assign) UInt32 type;
@property (nonatomic, assign) UInt32 createTime;
@property (nonatomic, assign) UInt32 length;
@property (nonatomic, assign) UInt32 status;
@property (nonatomic, retain) NSString* catalog;
@property (nonatomic, assign) UInt32 catalogId;
@property (nonatomic, retain) NSString* draft;

@property (nonatomic, assign) UInt32 lastUsedTime;
@property (nonatomic, assign) UInt32 extFlag;
@property (nonatomic, assign) UInt32 indexInPack;

@property (nonatomic, retain) NSString* extInfo;
@property (nonatomic, retain) NSString* packageId;

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

@interface DBBottle : NSObject <WCTTableCoding>

@property (nonatomic, assign) UInt32 localId;
@property (nonatomic, assign) UInt32 serverId;
@property (nonatomic, retain) NSString* encryptUserName;
@property (nonatomic, retain) NSString* bottleId;
@property (nonatomic, retain) NSString* ext;

WCDB_PROPERTY(localId)
WCDB_PROPERTY(serverId)
WCDB_PROPERTY(encryptUserName)
WCDB_PROPERTY(bottleId)
WCDB_PROPERTY(ext)

@end

@interface DBBottleContact : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString* userName;
@property (nonatomic, retain) NSString* nickName;
@property (nonatomic, assign) UInt32 sex;
@property (nonatomic, retain) NSString* imageStatus;
@property (nonatomic, retain) NSString* hdImageStatus;
@property (nonatomic, retain) NSString* province;
@property (nonatomic, retain) NSString* city;
@property (nonatomic, retain) NSString* sign;
@property (nonatomic, assign) UInt32 imageKey;
@property (nonatomic, assign) UInt32 imageKeyLast;
@property (nonatomic, assign) UInt32 extKey;
@property (nonatomic, assign) UInt32 extKeyLast;

@property (nonatomic, retain) NSString* ext;

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
