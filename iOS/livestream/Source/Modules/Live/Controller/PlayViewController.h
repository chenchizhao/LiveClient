//
//  PlayViewController.h
//  livestream
//
//  Created by Max on 2017/5/25.
//  Copyright © 2017年 net.qdating. All rights reserved.
//

#import "KKViewController.h"

#import "IMManager.h"

#import "UITapImageView.h"
#import "PresentView.h"
#import "CountTimeButton.h"
#import "LiveRoomTextField.h"
#import "GiftPresentView.h"
#import "ChatEmotionKeyboardView.h"
#import "BackpackPresentView.h"
#import "LiveSendBarView.h"
#import "YMAudienceView.h"
#import "LiveViewController.h"
#import "LiveGiftDownloadManager.h"
#import "LiveRoom.h"
#import "CreditView.h"

#import "LiveRoomGiftModel.h"

@class PlayViewController;
@protocol PlayViewControllerDelegate <NSObject>
@optional
- (void)onGetLiveRoomGiftList:(NSArray<LiveRoomGiftModel *> *)array;
- (void)onReEnterRoom:(PlayViewController *)vc;
@end

@interface PlayViewController : KKViewController

@property (nonatomic, weak) id<PlayViewControllerDelegate> delegate;

#pragma mark - 直播间信息
@property (nonatomic, strong) LiveRoom *liveRoom;

/** 显示界面 **/
@property (strong) LiveViewController *liveVC;

#pragma mark - 文本输入控件

/** 登录管理器 **/
@property (nonatomic, strong) LoginManager *loginManager;

/** 喇叭按钮 **/
@property (nonatomic, weak) IBOutlet UITapImageView* chatBtn;

/** 礼物按钮 **/
@property (nonatomic, weak) IBOutlet UIButton* giftBtn;

/** 礼物按钮约束 **/
@property (nonatomic, weak) IBOutlet NSLayoutConstraint* giftBtnWidth;

@property (weak, nonatomic) IBOutlet LiveSendBarView *liveSendBarView;

/** 发送栏 **/
@property (nonatomic, weak) IBOutlet UIView* inputMessageView;

// 才艺点播按钮
@property (weak, nonatomic) IBOutlet UIButton *talentBtn;

// 才艺点播按钮宽度
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *talentBtnWidth;

// 才艺点播左边约束
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *talentBtnTailing;

// 随机礼物背景
@property (weak, nonatomic) IBOutlet UIImageView *randomGiftImageView;

// 随机礼物按钮
@property (weak, nonatomic) IBOutlet UIButton *randomGiftBtn;

// 随机礼物按钮宽度
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *randomGiftBtnBgWidth;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *randomGiftBtnWidth;

// 随机礼物左边约束
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *randomGiftBtnTailing;

/** 发送栏底部约束 **/
@property (nonatomic, weak) IBOutlet NSLayoutConstraint* inputMessageViewBottom;

/** 输入框高度约束 **/
@property (nonatomic, weak) IBOutlet NSLayoutConstraint* inputMessageViewHeight;

/** 单击收起输入控件手势 **/
@property (nonatomic, strong) UITapGestureRecognizer *singleTap;

/**  礼物列表界面 **/
@property (strong, nonatomic) PresentView *presentView;

/**  背包礼物界面 **/
@property (strong, nonatomic) BackpackPresentView *backpackView;

/**  选择礼物列表界面 **/
@property (strong, nonatomic) ChatEmotionKeyboardView *giftListView;

/**  选择礼物承载界面 **/
@property (weak, nonatomic) IBOutlet UIView *chooseGiftListView;

@property (weak, nonatomic) IBOutlet UIImageView *bottomView;

@property (weak, nonatomic) IBOutlet UIImageView *bottomPeopleView;

@property (weak, nonatomic) IBOutlet NSLayoutConstraint *chooseGiftListViewHeight;

@property (weak, nonatomic) IBOutlet NSLayoutConstraint *chooseGiftListViewWidth;

@property (weak, nonatomic) IBOutlet NSLayoutConstraint *chooseGiftListViewTop;

@property (weak, nonatomic) IBOutlet NSLayoutConstraint *talentOnDemandViewHeight;

@property (weak, nonatomic) IBOutlet NSLayoutConstraint *talentOnDemandViewWidth;

@property (weak, nonatomic) IBOutlet NSLayoutConstraint *talentOnDemandViewTop;

// balanceView
@property (nonatomic, strong) CreditView *creditView;

#pragma mark - 界面事件

/**
 点击礼物按钮
 
 @param sender <#sender description#>
 */
- (IBAction)giftAction:(id)sender;

/**
 关闭所有输入
 */
- (void)closeAllInputView;

- (void)hiddenBottomView;
- (void)showBottomView;

@end
