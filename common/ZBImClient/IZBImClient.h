/*
 * author: Alex
 *   date: 2018-03-02
 *   file: IZBImClient.h
 *   desc: 主播IM客户端接口类
 *   注意不能在[客户端主动请求]回调函数里面再次调用接口
 */

#pragma once

#include "IZBImClientDef.h"

#include <common/CommonFunc.h>

#include <string>
#include <list>
#include <vector>
#include "ZBitem/ZBRoomInfoItem.h"
#include "ZBitem/ZBLoginReturnItem.h"
#include "ZBitem/ZBBookingReplyItem.h"
#include "ZBitem/ZBTalentReplyItem.h"
#include "ZBitem/ZBTalentRequestItem.h"
#include "ZBitem/ZBControlPushItem.h"
#include "ZBitem/AnchorHangoutRoomItem.h"
#include "ZBitem/AnchorHangoutInviteItem.h"
#include "ZBitem/IMAnchorRecommendHangoutItem.h"
#include "ZBitem/IMAnchorKnockRequestItem.h"
#include "ZBitem/IMAnchorRecvOtherInviteItem.h"
#include "ZBitem/IMAnchorRecvDealInviteItem.h"
#include "ZBitem/IMAnchorRecvEnterRoomItem.h"
#include "ZBitem/IMAnchorRecvLeaveRoomItem.h"
#include "ZBitem/IMAnchorRecvGiftItem.h"
#include "ZBitem/IMAnchorProgramInfoItem.h"
#include "ZBitem/IMAnchorRecvHangoutChatItem.h"

using namespace std;

// 直播间观众结构体
typedef struct _tZBRoomTopFan {
    _tZBRoomTopFan() {
        userId = "";
        nickName = "";
        photoUrl = "";
    };
    
    _tZBRoomTopFan(const _tZBRoomTopFan& item) {
        userId = item.userId;
        nickName = item.nickName;
        photoUrl = item.photoUrl;
    }
    
    string userId;
    string nickName;
    string photoUrl;
} ZBRoomTopFan;
// 直播间观众列表
typedef list<ZBRoomTopFan> ZBRoomTopFanList;

typedef struct _tZBGiftInfo {
    _tZBGiftInfo() {
        giftId = "";
        name = "";
        num = 0;
    };
    _tZBGiftInfo(const _tZBGiftInfo& item) {
        giftId = item.giftId;
        name = item.name;
        num = item.num;
    }
    
    string giftId;    // 礼物ID
    string name;      // 礼物名称
    int num;          // 礼物数量
} ZBGiftInfo;

typedef struct _tZBVoucherInfo {
    _tZBVoucherInfo() {
        voucherId = "";
        photoUrl = "";
        desc = "";
    };
    _tZBVoucherInfo(const _tZBVoucherInfo& item) {
        voucherId = item.voucherId;
        photoUrl = item.photoUrl;
        desc = item.desc;
    }
    
    string voucherId;    // 试用劵ID
    string photoUrl;      // 试用劵图标url
    string desc;          // 试用劵描述
} ZBVoucherInfo;

typedef struct _tZBRideInfo {
    _tZBRideInfo() {
        rideId = "";
        photoUrl = "";
        name = "";
    };
    _tZBRideInfo(const _tZBRideInfo& item) {
        rideId = item.rideId;
        photoUrl = item.photoUrl;
        name = item.name;
    }
    
    string rideId;        // 座驾ID
    string photoUrl;      // 座驾图片url
    string name;          // 座驾名称
} ZBRideInfo;

typedef struct _tZBBackpackInfo {
    _tZBBackpackInfo() {
        
    };
    _tZBBackpackInfo(const _tZBBackpackInfo& item) {
        gift = item.gift;
        voucher = item.voucher;
        ride = item.ride;
    }
    
    ZBGiftInfo     gift;        // 新增的背包礼物
    ZBVoucherInfo voucher;      // 新增的试用劵
    ZBRideInfo    ride;         // 新增的座驾
} ZBBackpackInfo;

// 主播IM客户端监听接口类
class IZBImClientListener
{
public:
	IZBImClientListener() {};
	virtual ~IZBImClientListener() {}

public:
	// ------------- 登录/注销 -------------
    /**
     *  2.1.登录回调
     *
     *  @param err          结果类型
     *  @param errmsg       结果描述
     *  @param item         登录返回结构体
     */
    virtual void OnZBLogin(ZBLCC_ERR_TYPE err, const string& errmsg, const ZBLoginReturnItem& item) {}
    virtual void OnZBLogout(ZBLCC_ERR_TYPE err, const string& errmsg) {}
    /**
     *  2.4.用户被挤掉线回调
     *
     */
    virtual void OnZBKickOff(ZBLCC_ERR_TYPE err, const string& errmsg) {}

    // ------------- 直播间处理(非消息) -------------
    /**
     *  3.1.主播进入公开直播间接口 回调
     *
     *  @param success      操作是否成功
     *  @param reqId        请求序列号
     *  @param errMsg      结果描述
     *  @param item         直播间信息
     *
     */
    virtual void OnZBPublicRoomIn(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg, const ZBRoomInfoItem& item) {};
    /**
     *  3.2.主播进入指定直播间回调
     *
     *  @param success     操作是否成功
     *  @param reqId       请求序列号
     *  @param err         结果类型
     *  @param errMsg      结果描述
     *  @param item        直播间信息
     *
     */
    virtual void OnZBRoomIn(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg, const ZBRoomInfoItem& item) {};

    /**
     *  3.3.主播退出直播间回调
     *
     *  @param success     操作是否成功
     *  @param reqId       请求序列号
     *  @param err     结果类型
     *  @param errMsg      结果描述
     *
     */
    virtual void OnZBRoomOut(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg) {};

    /**
     *  3.4.接收直播间关闭通知(观众)回调
     *
     *  @param roomId      直播间ID
     *
     */
    virtual void OnZBRecvRoomCloseNotice(const string& roomId, ZBLCC_ERR_TYPE err, const string& errMsg) {};
    
    /**
     *  3.5.接收踢出直播间通知
     *
     *  @param roomId      直播间ID
     *  @param err     踢出原因错误码
     *  @param errMsg      踢出原因描述
     *
     */
    virtual void OnZBRecvRoomKickoffNotice(const string& roomId, ZBLCC_ERR_TYPE err, const string& errMsg) {};

    /**
     *  3.6.接收观众进入直播间通知回调
     *
     *  @param roomId      直播间ID
     *  @param userId      观众ID
     *  @param nickName    观众昵称
     *  @param photoUrl    观众头像url
     *  @param riderId     座驾ID
     *  @param riderName   座驾名称
     *  @param riderUrl    座驾图片url
     *  @param fansNum     观众人数
     *  @param isHasTicket 是否已购票
     *
     */
    virtual void OnZBRecvEnterRoomNotice(const string& roomId, const string& userId, const string& nickName, const string& photoUrl, const string& riderId, const string& riderName, const string& riderUrl, int fansNum, bool isHasTicket) {};

    /**
     *  3.7.接收观众退出直播间通知回调
     *
     *  @param roomId      直播间ID
     *  @param userId      观众ID
     *  @param nickName    观众昵称
     *  @param photoUrl    观众头像url
     *  @param fansNum     观众人数
     *
     */
    virtual void OnZBRecvLeaveRoomNotice(const string& roomId, const string& userId, const string& nickName, const string& photoUrl, int fansNum) {};

    /**
     *  3.8.接收关闭直播间倒数通知回调
     *
     *  @param roomId      直播间ID
     *  @param leftSeconds 关闭直播间倒数秒数（整型）（可无，无或0表示立即关闭）
     *  @param err         错误码
     *  @param errMsg      错误描述
     *
     */
    virtual void OnZBRecvLeavingPublicRoomNotice(const string& roomId, int leftSeconds, ZBLCC_ERR_TYPE err, const string& errMsg) {};
    
    /**
     *  3.9.接收主播退出直播间通知回调
     *
     *  @param roomId       直播间ID
     *  @param anchorId     退出直播间的主播ID
     *
     */
    virtual void OnRecvAnchorLeaveRoomNotice(const string& roomId, const string& anchorId) {};

    // ------------- 直播间处理(非消息) -------------
    /**
     *  4.1.发送直播间文本消息回调
     *
     *  @param reqId       请求序列号
     *  @param err         结果类型
     *  @param errMsg      结果描述
     *
     */
    virtual void OnZBSendLiveChat(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg) {};

    /**
     *  4.2.接收直播间文本消息通知回调
     *
     *  @param roomId      直播间ID
     *  @param level       发送方级别
     *  @param fromId      发送方的用户ID
     *  @param nickName    发送方的昵称
     *  @param msg         文本消息内容
     *  @param honorUrl    勋章图片url
     *
     */
    virtual void OnZBRecvSendChatNotice(const string& roomId, int level, const string& fromId, const string& nickName, const string& msg, const string& honorUrl) {};

    /**
     *  4.3.接收直播间公告消息回调
     *
     *  @param roomId      直播间ID
     *  @param msg         公告消息内容
     *  @param link        公告链接（可无，无则表示不是带链接的公告消息） （仅当type=0有效）
     *  @param type        公告类型（0：普通，1：警告）
     *
     */
    virtual void OnZBRecvSendSystemNotice(const string& roomId, const string& msg, const string& link, ZBIMSystemType type) {};

    // ------------- 直播间礼物消息 -------------
    /**
     *  5.1.发送直播间礼物消息（观众端发送直播间礼物消息，包括连击礼物）回调
     *
     *  @param success       操作是否成功
     *  @param reqId         请求序列号
     *  @param err           结果类型
     *  @param errMsg        结果描述
     *  @param credit        信用点
     *  @param rebateCredit  返点
     *
     */
    virtual void OnZBSendGift(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg) {};

    /**
     * 5.2.接收直播间礼物通知（观众端／主播端接收直播间礼物消息，包括连击礼物）回调
     *
     *  @param roomId               直播间ID
     *  @param fromId               发送方的用户ID
     *  @param nickName             发送方的昵称
     *  @param giftId               礼物ID
     *  @param giftNum              本次发送礼物的数量
     *  @param multi_click          是否连击礼物
     *  @param multi_click_start    连击起始数
     *  @param multi_click_end      连击结束数
     *  @param multi_click_id       连击ID，相同则表示是同一次连击
     *  @param honorUrl             勋章图片url
     *
     */
    virtual void OnZBRecvSendGiftNotice(const string& roomId, const string& fromId, const string& nickName, const string& giftId, const string& giftName, int giftNum, bool multi_click, int multi_click_start, int multi_click_end, int multi_click_id, const string& honorUrl, int totalCredit) {};

    // ------------- 直播间弹幕消息 -------------
    /**
     *  6.1.接收直播间弹幕通知（观众端／主播端接收直播间弹幕消息）回调
     *
     *  @param roomId               直播间ID
     *  @param fromId               发送方的用户ID
     *  @param nickName             发送方的昵称
     *  @param msg                  消息内容
     *  @param honorUrl             勋章图片url
     *
     */
    virtual void OnZBRecvSendToastNotice(const string& roomId, const string& fromId, const string& nickName, const string& msg, const string& honorUrl) {};

    // ------------- 直播间才艺点播邀请 -------------
    /**
     *  7.1.接收直播间才艺点播邀请通知回调
     *
     *  @param talentRequestItem             才艺点播请求
     *
     */
    virtual void OnZBRecvTalentRequestNotice(const ZBTalentRequestItem talentRequestItem) {};
    
    // ------------- 直播间视频互动 -------------
    /**
     *  8.1.接收观众启动/关闭视频互动通知回调
     *
     *  @param Item            互动切换
     *
     */
    virtual void OnZBRecvControlManPushNotice(const ZBControlPushItem item) {};
    
    // ------------- 邀请私密直播 -------------
    /**
     *  9.1.主播发送立即私密邀请
     *
     *  @param success           操作是否成功
     *  @param reqId             请求序列号
     *  @param err               结果类型
     *  @param errMsg            结果描述
     *  @param invitationId      邀请ID
     *  @param timeOut           邀请的剩余有效时间
     *  @param roomId            直播间ID
     *
     */
    virtual void OnZBSendPrivateLiveInvite(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg, const string& invitationId, int timeOut, const string& roomId) {};

    /**
     *  9.2.接收立即私密邀请回复通知 回调
     *
     *  @param inviteId      邀请ID
     *  @param replyType     主播回复 （0:拒绝 1:同意）
     *  @param roomId        直播间ID （可无，m_replyType ＝ 1存在）
     *  @param roomType      直播间类型
     *  @param userId        观众ID
     *  @param nickName      主播昵称
     *  @param avatarImg     主播头像
     *
     */
    virtual void OnZBRecvInstantInviteReplyNotice(const string& inviteId, ZBReplyType replyType ,const string& roomId, ZBRoomType roomType, const string& userId, const string& nickName, const string& avatarImg) {};

    /**
     *  9.3.接收立即私密邀请通知 回调
     *
     *  @param userId           观众ID
     *  @param nickName         观众昵称
     *  @param photoUrl         观众头像url
     *  @param invitationId     邀请ID

     *
     */
    virtual void OnZBRecvInstantInviteUserNotice(const string& userId, const string& nickName, const string& photoUrl ,const string& invitationId) {};


    /**
     *  9.4.接收预约开始倒数通知 回调
     *
     *  @param roomId       直播间ID
     *  @param userId       对端ID
     *  @param nickName     对端昵称
     *  @param avatarImg    对端头像url
     *  @param leftSeconds  倒数时间（秒）
     *
     */
    virtual void OnZBRecvBookingNotice(const string& roomId, const string& userId, const string& nickName, const string& avatarImg, int leftSeconds) {};
    
    /**
     *  9.5.获取指定立即私密邀请信息接口 回调
     *
     *  @param success          操作是否成功
     *  @param reqId            请求序列号
     *  @param errMsg           结果描述
     *  @param item             立即私密邀请
     *
     */
    virtual void OnZBGetInviteInfo(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg, const ZBPrivateInviteItem& item) {};


    /**
     *  9.6.接收观众接受预约通知接口 回调
     *
     *  @param userId           观众ID
     *  @param nickName         观众昵称
     *  @param photoUrl         观众头像url
     *  @param invitationId     预约ID
     *  @param bookTime         预约时间（1970年起的秒数）
     */
    virtual void OnZBRecvInvitationAcceptNotice(const string& userId, const string& nickName, const string& photoUrl, const string& invitationId, long bookTime) {};

    
    // ------------- 多人互动直播间 -------------
    /**
     *  10.1.进入多人互动直播间接口 回调
     *
     *  @param success      操作是否成功
     *  @param reqId        请求序列号
     *  @param errMsg      结果描述
     *  @param item        进入多人互动直播间信息
     *  @param expire      倒数进入秒数，倒数完成后再调用本接口重新进入
     *
     */
    virtual void OnAnchorEnterHangoutRoom(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg, const AnchorHangoutRoomItem& item, int expire) {};

    /**
     *  10.2.退出多人互动直播间接口 回调
     *
     *  @param success      操作是否成功
     *  @param reqId        请求序列号
     *  @param errMsg      结果描述
     *
     */
    virtual void OnAnchorLeaveHangoutRoom(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg) {};

    /**
     *  10.3.接收观众邀请多人互动通知接口 回调
     *
     *  @param item         观众邀请多人互动信息
     *
     */
    virtual void OnRecvAnchorInvitationHangoutNotice(const AnchorHangoutInviteItem& item) {};

    /**
     *  10.4.接收推荐好友通知接口 回调
     *
     *  @param item         主播端接收自己推荐好友给观众的信息
     *
     */
    virtual void OnRecvAnchorRecommendHangoutNotice(const IMAnchorRecommendHangoutItem& item) {};

    /**
     *  10.5.接收敲门回复通知接口 回调
     *
     *  @param item         接收敲门回复信息
     *
     */
    virtual void OnRecvAnchorDealKnockRequestNotice(const IMAnchorKnockRequestItem& item) {};

    /**
     *  10.6.接收观众邀请其它主播加入多人互动通知接口 回调
     *
     *  @param item         接收观众邀请其它主播加入多人互动信息
     *
     */
    virtual void OnRecvAnchorOtherInviteNotice(const IMAnchorRecvOtherInviteItem& item) {};

    /**
     *  10.7.接收主播回复观众多人互动邀请通知接口 回调
     *
     *  @param item         接收主播回复观众多人互动邀请信息
     *
     */
    virtual void OnRecvAnchorDealInviteNotice(const IMAnchorRecvDealInviteItem& item) {};

    /**
     *  10.8.观众端/主播端接收观众/主播进入多人互动直播间通知接口 回调
     *
     *  @param item         接收主播回复观众多人互动邀请信息
     *
     */
    virtual void OnRecvAnchorEnterRoomNotice(const IMAnchorRecvEnterRoomItem& item) {};

    /**
     *  10.9.接收观众/主播退出多人互动直播间通知接口 回调
     *
     *  @param item         接收观众/主播退出多人互动直播间信息
     *
     */
    virtual void OnRecvAnchorLeaveRoomNotice(const IMAnchorRecvLeaveRoomItem& item) {};

    /**
     *  10.10.接收观众/主播多人互动直播间视频切换通知接口 回调
     *
     *  @param roomId         直播间ID
     *  @param isAnchor       是否主播（0：否，1：是）
     *  @param userId         观众/主播ID
     *  @param playUrl        视频流url（字符串数组）（访问视频URL的协议参考《 “视频URL”协议描述》）
     *
     */
    virtual void OnRecvAnchorChangeVideoUrl(const string& roomId, bool isAnchor, const string& userId, const list<string>& playUrl) {};

    /**
     *  10.11.发送多人互动直播间礼物消息接口 回调
     *
     *  @param success          操作是否成功
     *  @param reqId            请求序列号
     *  @param errMsg           结果描述
     *
     */
    virtual void OnSendAnchorHangoutGift(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg) {};

    /**
     *  10.12.接收多人互动直播间礼物通知接口 回调
     *
     *  @param item         接收多人互动直播间礼物信息
     *
     */
    virtual void OnRecvAnchorGiftNotice(const IMAnchorRecvGiftItem& item) {};
    
    /**
     *  10.13.接收多人互动直播间观众启动/关闭视频互动通知回调
     *
     *  @param item            互动切换
     *
     */
    virtual void OnRecvAnchorControlManPushHangoutNotice(const ZBControlPushItem item) {};
    
    /**
     *  10.14.发送多人互动直播间文本消息接口 回调
     *
     *  @param success          操作是否成功
     *  @param reqId            请求序列号
     *  @param errMsg           结果描述
     *
     */
    virtual void OnSendAnchorHangoutLiveChat(SEQ_T reqId, bool success, ZBLCC_ERR_TYPE err, const string& errMsg) {};
    
    /**
     *  10.15.接收直播间文本消息回调
     *
     *  @param item            接收直播间的文本消息
     *
     */
    virtual void OnRecvAnchorHangoutChatNotice(const IMAnchorRecvHangoutChatItem& item) {};
    
    /**
     *  10.16.接收进入多人互动直播间倒数通知回调
     *
     *  @param roomId            待进入的直播间ID
     *  @param anchorId          主播ID
     *  @param leftSecond       进入直播间的剩余秒数
     *
     */
    virtual void OnRecvAnchorCountDownEnterRoomNotice(const string& roomId, const string& anchorId, int leftSecond) {};

    
    // ------------- 节目 -------------
    /**
     *  11.1.接收节目开播通知接口 回调
     *
     *  @param item         节目信息
     *  @param msg          消息提示文字
     *
     */
    virtual void OnRecvAnchorProgramPlayNotice(const IMAnchorProgramInfoItem& item, const string& msg) {};
    
    /**
     *  11.2.接收节目状态改变通知接口 回调
     *
     *  @param item         节目信息
     *
     */
    virtual void OnRecvAnchorChangeStatusNotice(const IMAnchorProgramInfoItem& item) {};
    
    
    /**
     *  11.3.接收无操作的提示通知接口 回调
     *
     *  @param backgroundUrl 背景图url
     *  @param msg           描述
     *
     */
    virtual void OnRecvAnchorShowMsgNotice(const string& backgroundUrl, const string& msg) {};

    
};

// 主播IM客户端接口类
class IZBImClient
{
public:
    typedef enum _tZBLoginStatus{
        ZBLOGINING,   // login中
        ZBLOGINED,    // 已login
        ZBLOGOUT,     // logout
    } ZBLoginStatus;
    
public:
	static IZBImClient* CreateClient();
	static void ReleaseClient(IZBImClient* client);

public:
	IZBImClient(void) {};
	virtual ~IZBImClient(void) {};

public:
	// 调用所有接口函数前需要先调用Init
	virtual bool Init(const list<string>& urls) = 0;
    // 增加监听器
    virtual void AddListener(IZBImClientListener* listener) = 0;
    // 移除监听器
    virtual void RemoveListener(IZBImClientListener* listener) = 0;
	// 判断是否无效seq
	virtual bool IsInvalidReqId(SEQ_T reqId) = 0;
	// 获取reqId
	virtual SEQ_T GetReqId() = 0;
    
    // --------- 登录/注销 ---------
	// 2.1.登录
	virtual bool ZBLogin(const string& token,   ZBPageNameType pageName) = 0;
    // 2.2.注销
    virtual bool ZBLogout() = 0;
    // 获取login状态
    virtual ZBLoginStatus ZBGetLoginStatus() = 0;
//    
//    // --------- 直播间 ---------
    /**
     *  3.1.新建/进入公开直播间
     *
     *  @param reqId         请求序列号
     *
     */
    virtual bool ZBPublicRoomIn(SEQ_T reqId) = 0;
    
    /**
     *  3.2.主播进入指定直播间
     *
     *  @param reqId         请求序列号
     *  @param roomId        直播间ID
     *
     */
    virtual bool ZBRoomIn(SEQ_T reqId, const string& roomId) = 0;

    /**
     *  3.3.主播退出直播间
     *
     *  @param reqId         请求序列号
     *  @param roomId        直播间ID
     *
     */
    virtual bool ZBRoomOut(SEQ_T reqId, const string& roomId) = 0;


    
    // --------- 直播间文本消息 ---------
    /**
     *  4.1.发送直播间文本消息
     *
     *  @param reqId         请求序列号
     *  @param roomId        直播间ID
     *  @param nickName      发送者昵称
     *  @param msg           发送的信息
     *  @param at           用户ID，用于指定接收者
     *
     */
    virtual bool ZBSendLiveChat(SEQ_T reqId, const string& roomId, const string& nickName, const string& msg, const list<string> at) = 0;

    // ------------- 直播间点赞 -------------
    /**
     *  5.1.发送直播间礼物消息（观众端发送直播间礼物消息，包括连击礼物）
     *
     *  @param reqId                    请求序列号
     *  @param roomId                   直播间ID
     *  @param nickName                 发送人昵称
     *  @param giftId                   礼物ID
     *  @param giftName                 礼物名称
     *  @param isBackPack               是否背包礼物
     *  @param giftNum                  本次发送礼物的数量
     *  @param multi_click              是否连击礼物
     *  @param multi_click_start        连击起始数
     *  @param multi_click_end          连击结束数
     *  @param multi_click_id           连击ID，相同则表示是同一次连击（生成方式：timestamp秒％10000）
     *
     */
    virtual bool ZBSendGift(SEQ_T reqId, const string& roomId, const string& nickName, const string& giftId, const string& giftName, bool isBackPack, int giftNum, bool multi_click, int multi_click_start, int multi_click_end, int multi_click_id) = 0;
   
    // ------------- 邀请私密直播 -------------
    /**
     *  9.1.主播发送立即私密邀请
     *
     *  @param reqId                 请求序列号
     *  @param userId                主播ID
     *
     */
    virtual bool ZBSendPrivateLiveInvite(SEQ_T reqId, const string& userId) = 0;
    
    
    /**
     *  9.5.获取指定立即私密邀请信息
     *
     *  @param reqId            请求序列号
     *  @param invitationId     邀请ID
     *
     */
    virtual bool ZBGetInviteInfo(SEQ_T reqId, const string& invitationId) = 0;

    // ------------- 多人互动直播间 -------------
    /**
     *  10.1.进入多人互动直播间
     *
     *  @param reqId            请求序列号
     *  @param roomId           直播间ID
     *
     */
    virtual bool AnchorEnterHangoutRoom(SEQ_T reqId, const string& roomId) = 0;

    /**
     *  10.2.退出多人互动直播间
     *
     *  @param reqId            请求序列号
     *  @param roomId           直播间ID
     *
     */
    virtual bool AnchorLeaveHangoutRoom(SEQ_T reqId, const string& roomId) = 0;

    /**
     *  10.11.发送多人互动直播间礼物消息接口
     *
     * @param reqId         请求序列号
     * @roomId              直播间ID
     * @nickName            发送人昵称
     * @toUid               接收者ID
     * @giftId              礼物ID
     * @giftName            礼物名称
     * @isBackPack          是否背包礼物（1：是，0：否）
     * @giftNum             本次发送礼物的数量
     * @isMultiClick        是否连击礼物（1：是，0：否）
     * @multiClickStart     连击起始数（整型）（可无，multi_click=0则无）
     * @multiClickEnd       连击结束数（整型）（可无，multi_click=0则无）
     * @multiClickId        连击ID，相同则表示是同一次连击（整型）（可无，multi_click=0则无）
     * @isPrivate           是否私密发送（1：是，0：否）
     *
     */
    virtual bool SendAnchorHangoutGift(SEQ_T reqId, const string& roomId, const string& nickName, const string& toUid, const string& giftId, const string& giftName, bool isBackPack, int giftNum, bool isMultiClick, int multiClickStart, int multiClickEnd, int multiClickId, bool isPrivate)  = 0;
    
    /**
     *  10.14.发送多人互动直播间文本消息
     *
     *  @param reqId         请求序列号
     *  @param roomId        直播间ID
     *  @param nickName      发送者昵称
     *  @param msg           发送的信息
     *  @param at           用户ID，用于指定接收者（字符串数组）
     *
     */
    virtual bool SendAnchorHangoutLiveChat(SEQ_T reqId, const string& roomId, const string& nickName, const string& msg, const list<string> at) = 0;

    
public:
	// 获取用户账号
	virtual string GetUser() = 0;
};
