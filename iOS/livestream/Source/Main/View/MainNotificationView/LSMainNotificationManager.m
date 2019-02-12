//
//  LSMainNotificationManager.m
//  livestream
//
//  Created by Calvin on 2019/1/17.
//  Copyright © 2019年 net.qdating. All rights reserved.
//

#import "LSMainNotificationManager.h"
#import "QNLiveChatLocalPushManager.h"
#import "LiveModule.h"
#import "LSMainNotificaitonModel.h"
#import "LSImManager.h"
#import "LSGetHangoutFriendsRequest.h"
#import "LSAutoInvitationHangoutLiveDisplayRequest.h"
//显示冒泡最大数
#define SHOW_MAX_NUM 8
//显示聊天消息的最大数
#define SHOW_CHAT_MAX_NUM 4
//显示直播消息的最大数
#define SHOW_LIVE_MAX_NUM SHOW_MAX_NUM - SHOW_CHAT_MAX_NUM
//缓存最大数
#define CACHE_MAX_NUM 4
//超时消失时间
#define TIMEOUT 45.0
//延迟显示i气泡时间
#define DELAY_TIME 5.0

static LSMainNotificationManager* mainNotificationManager = nil;

@interface LSMainNotificationManager ()<QNLiveChatLocalPushManagerDelegate,IMManagerDelegate>
@property (nonatomic, strong) NSMutableArray * cacheArray;
@property (nonatomic, strong) NSMutableArray * showArray;
@property (nonatomic, strong) NSTimer * timer;
@property (nonatomic, strong) NSMutableArray * hangoutArray;
@end

@implementation LSMainNotificationManager


+ (instancetype)manager {
    if( mainNotificationManager == nil ) {
        mainNotificationManager = [[[self class] alloc] init];
    }
    return mainNotificationManager;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self.cacheArray = [NSMutableArray array];
        self.showArray = [NSMutableArray array];
        self.hangoutArray = [NSMutableArray array];
       [QNLiveChatLocalPushManager sharedInstance].delegate = self;
       [[LSImManager manager] addDelegate:self];
    }
    return self;
}

- (void)newCountdown {
    if (!self.timer) {
        self.timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(countdown) userInfo:nil repeats:YES];
    }
}

- (void)stopCountdown {
    [self.timer invalidate];
    self.timer = nil;
}

- (void)countdown {
    
    if (self.showArray.count == 0 && self.cacheArray.count == 0) {
        [self stopCountdown];
        if ([self.delegate respondsToSelector:@selector(mainNotificationManagerRemoveNotificaitonView)]) {
            [self.delegate mainNotificationManagerRemoveNotificaitonView];
            return;
        }
    }
    else if (self.showArray.count == 0 && self.cacheArray.count > 0) {
        [self showNotificaitonView];
    }
    else {
        @synchronized (self.showArray) {
            BOOL isTimeOut = NO;
            NSInteger row = 0;
            for (int i = 0; i < self.showArray.count; i++) {
                LSMainNotificaitonModel * model = self.showArray[i];
                if ([[NSDate date]timeIntervalSince1970] - [self timeOutNum] > model.createTime) {
                    isTimeOut = YES;
                    row = i;
                    break;
                }
            }
            if (isTimeOut) {
                [self.showArray removeObjectAtIndex:row];
                
                if (self.showArray.count > 0) {
                    //刷新UI
                    [self hideTimeoutNotificaitonView:row];
                }
            }
            
            if (self.showArray.count < SHOW_MAX_NUM) {
                 [self showNotificaitonView];
            }
        }
    }
}


- (CGFloat)timeOutNum {
    return self.showArray.count == 1?TIMEOUT:TIMEOUT+DELAY_TIME;
}

- (NSArray *)items {
    return  [[self.showArray reverseObjectEnumerator] allObjects];
}

//插入缓存通知数据
- (void)insertCacheNotificaiton:(LSMainNotificaitonModel *)model {
    @synchronized (self) {
        if (self.cacheArray.count > CACHE_MAX_NUM) {
            [self.cacheArray removeObjectAtIndex:0];
        }
        [self.cacheArray addObject:model];
    }
    //开始倒计时
    [self newCountdown];
}

//获取显示数组类型个数
- (NSInteger)getShowMsgTypeNumIsChat:(BOOL)isChat {
    NSInteger chatNum = 0;
    NSInteger liveNum = 0;
     @synchronized (self.showArray) {
        for (LSMainNotificaitonModel * model in self.showArray) {
            if (model.msgType == MsgStatus_Chat) {
                chatNum++;
            }else {
                liveNum++;
            }
        }
     }
    return isChat?chatNum:liveNum;
}

//添加到显示数组并回调显示UI
- (void)addDataToShowArrayAndCallbackDelegate:(LSMainNotificaitonModel *)model {
    //可显示
    @synchronized (self) {
        model.createTime = [[NSDate date] timeIntervalSince1970];
        [self.showArray addObject:model];
        [self.cacheArray removeObject:model];
        if (model.msgType == MsgStatus_Hangout) {
            [self.hangoutArray addObject:model];
        }
    }
    [self delayCallbackDelegate];
}

//删除没用的缓存消息
- (void)removeCacheFirstObject {
    @synchronized (self) {
        if (self.cacheArray.count > 0) {
            [self.cacheArray removeObjectAtIndex:0];
        }
    }
}

//延时通知
- (void)delayCallbackDelegate {
    CGFloat delay = self.showArray.count == 1?0:DELAY_TIME;
    __weak typeof(self)weakSelf = self;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        //刷新UI
        if ([weakSelf.delegate respondsToSelector:@selector(mainNotificationManagerShowNotificaitonView)]) {
            [weakSelf.delegate mainNotificationManagerShowNotificaitonView];
        }
    });
}

//显示气泡逻辑
- (void)showNotificaitonView {
    if (self.cacheArray.count > 0) {
        LSMainNotificaitonModel * model = [self.cacheArray objectAtIndex:0];
        if (model.msgType == MsgStatus_Chat) {
            if ([self getShowMsgTypeNumIsChat:YES] < SHOW_CHAT_MAX_NUM) {
                [self addDataToShowArrayAndCallbackDelegate:model];
            }else {
                //删除没用的缓存消息
                [self removeCacheFirstObject];
            }
        }else {
            if ([self getShowMsgTypeNumIsChat:NO] < SHOW_LIVE_MAX_NUM) {
                
                for (LSMainNotificaitonModel * item in self.hangoutArray) {
                    if ([item.userId isEqualToString:model.userId] && item.createTime + 600 < [[NSDate date] timeIntervalSince1970]) {
                        //10分鐘内顯示过該主播的邀请
                        NSLog(@"LSMainNotificationManager::showNotificaitonView  10分鐘内顯示过該主播的邀请，不插入气泡");
                        //删除没用的缓存消息
                        [self removeCacheFirstObject];
                        return;
                    }
                }
                
                LSAutoInvitationHangoutLiveDisplayRequest * request = [[LSAutoInvitationHangoutLiveDisplayRequest alloc]init];
                request.anchorId = model.userId;
                request.finishHandler = ^(BOOL success, HTTP_LCC_ERR_TYPE errnum, NSString *errmsg) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                         NSLog(@"LSMainNotificationManager::showNotificaitonView LSAutoInvitationHangoutLiveDisplayRequest hangout消息是否显示 %@",BOOL2SUCCESS(success));
                        if (success) {
                            [self addDataToShowArrayAndCallbackDelegate:model];
                        }else {
                            //删除没用的缓存消息
                            [self removeCacheFirstObject];
                        }
                    });
                };
                [[LSSessionRequestManager manager]sendRequest:request];
            }
            else {
                //删除没用的缓存消息
                [self removeCacheFirstObject];
            }
        }
    }
}

//删除过期气泡
- (void)hideTimeoutNotificaitonView:(NSInteger)row {
    
    if ([self.delegate respondsToSelector:@selector(mainNotificationManagerHideNotificaitonView:)]) {
        [self.delegate mainNotificationManagerHideNotificaitonView:row];
    }
}

//点击选中气泡并移除
- (void)selectedShowArrayRow:(NSInteger)row {
    @synchronized (self.showArray) {
        [self.showArray removeObjectAtIndex:row];
    }
    [self hideTimeoutNotificaitonView:row];
}

#pragma mark - 收到liveChat消息
- (void)liveChatPushManager:(NSString *)msg andMsgItem:(LSLCLiveChatMsgItemObject *)msgObj formLady:(LSLCLiveChatUserItemObject *)userItem {
    //在聊消息不显示
    if ([[LSLiveChatManagerOC manager] isChatingUserInChatState:userItem.userId]) {
        return;
    }
    
    if (userItem.chatType == LC_CHATTYPE_INVITE) {
        [[LiveModule module].analyticsManager reportActionEvent:ShowLiveChatInvitation eventCategory:EventCategoryLiveChat];
    }else if (userItem.chatType == LC_CHATTYPE_IN_CHAT_USE_TRY_TICKET || LC_CHATTYPE_IN_CHAT_CHARGE) {
        [[LiveModule module].analyticsManager reportActionEvent:ShowLiveChatMessage eventCategory:EventCategoryLiveChat];
    }
    
    LSMainNotificaitonModel * model = [[LSMainNotificaitonModel alloc]init];
    model.userName = userItem.userName;
    model.userId = userItem.userId;
    model.photoUrl = userItem.imageUrl;
    model.contentStr = msg;
    model.msgType = MsgStatus_Chat;
    [self insertCacheNotificaiton:model];
}

#pragma mark - 收到hangout邀请通知
- (void)onRecvHandoutInviteNotice:(IMHangoutInviteItemObject *)item {
    
    NSLog(@"LSMainNotificationManager::onRecvHandoutInviteNotice anchorNickName: %@,anchorId : %@ InviteMessage: %@",item.anchorNickName,item.anchorId,item.InviteMessage);
    dispatch_async(dispatch_get_main_queue(), ^{
        LSGetHangoutFriendsRequest * request = [[LSGetHangoutFriendsRequest alloc]init];
        request.anchorId = item.anchorId;
        request.finishHandler = ^(BOOL success, HTTP_LCC_ERR_TYPE errnum, NSString *errmsg, NSString *anchorId, NSArray<LSHangoutAnchorItemObject *> *array) {
            dispatch_async(dispatch_get_main_queue(), ^{
                NSLog(@"LSMainNotificationManager::LSGetHangoutFriendsRequest:%@,errnum : %d, errmsg : %@",BOOL2SUCCESS(success),errnum,errmsg);
                if (array.count > 0) {
                    int r = arc4random() % [array count];
                    LSHangoutAnchorItemObject * ojb = [array objectAtIndex:r];
                    LSMainNotificaitonModel * model = [[LSMainNotificaitonModel alloc]init];
                    model.userName = item.anchorNickName;
                    model.userId = item.anchorId;
                    model.photoUrl = item.avatarImg;
                    model.contentStr = item.InviteMessage;
                    model.msgType = MsgStatus_Hangout;
                    model.friendUrl = ojb.photoUrl;
                    [self insertCacheNotificaiton:model];
                }
            });
        };
        [[LSSessionRequestManager manager] sendRequest:request];
    });
}

@end