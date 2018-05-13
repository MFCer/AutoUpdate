#pragma once

// 不需要手动调用提示
#define LID_DONT_NEED_MANUAL_CALL_TEXT				0
#define LID_DNMCT_DEFAULT							_T("This program do not need manual open!")
// 不需要手动调用标题
#define LID_DONT_NEED_MANUAL_CALL_CAPTION			1
#define LID_DNMCC_DEFAULT							_T("System hint")
// 托盘Tip
#define LID_TRAY_TIP								2
#define LID_TT_DEFAULT								_T("Auto Update Client is Running...")
// 正在下载气泡标题
#define LID_TRAY_DOWNLOADING_BALLOON_TITLE			3
#define LID_TDBT_DEFAULT							_T("Downloading")
// 正在下载气泡提示
#define LID_TRAY_DOWNLOADING_BALLOON_INFO			4
#define LID_TDBI_DEFAULT							_T("Current: %s %.2f%%\rTotal progress: %.2f%%")
// 新更新确认对话框标题
#define LID_UPDATE_QUESTION_DIALOG_CAPTION			5
#define LID_UQDC_DEFAULT							_T(" New update available!")
// 新更新确认对话框新特性
#define LID_UPDATE_QUESTION_DIALOG_LATEST_FEATURE	6
#define LID_UQDLF_DEFAULT							_T("Latest features:")
// 新更新确认对话框确认
#define LID_UPDATE_QUESTION_DIALOG_ENSURE			7
#define LID_UQDE_DEFAULT							_T("Are you sure to start downloading?")
// 新更新确认对话框忽略复选
#define LID_UPDATE_QUESTION_DIALOG_IGNORE_CHECK		8
#define LID_UQDIC_DEFAULT							_T("Do not show me this message until next update.")
// 新更新确认对话框“是”按钮
#define LID_UPDATE_QUESTION_DIALOG_YES_BTN_TEXT		9
#define LID_UQDYBT_DEFAULT							_T("YES")
// 新更新确认对话框“否”按钮
#define LID_UPDATE_QUESTION_DIALOG_NO_BTN_TEXT		10
#define LID_UQDNBT_DEFAULT							_T("NO")
// 下载完成托盘提示标题
#define LID_TRAY_DOWNLOAD_COMPLETE_BALLOON_TITLE	11
#define LID_TDCBT_DEFAULT							_T("Download complete")
// 下载完成托盘提示文本
#define LID_TRAY_DOWNLOAD_COMPLETE_BALLOON_INFO		12
#define LID_TDCBI_DEFAULT							_T("Update will take effect after restarting the program.")
// 确认更新消息框标题
#define LID_CONFIRM_UPDATE_MESSAEBOX_CAPTION		13
#define LID_CUMC_DEFAULT							_T("Ready to update files")
// 确认更新消息框内容
#define LID_CONFIRM_UPDATE_MESSAEBOX_TEXT			14
#define LID_CUMT_DEFAULT							_T("Are you sure to complete update?\r(Save your current work, so as not to lose.)")
// 更新完成托盘提示标题
#define LID_TRAY_UPDATE_COMPLETE_BALLOON_TITLE		15
#define LID_TUCBT_DEFAULT							_T("Congratulations")
// 更新完成托盘提示文本
#define LID_TRAY_UPDATE_COMPLETE_BALLOON_INFO		16
#define LID_TUCBI_DEFAULT							_T("Update complete.")
// 网络错误消息框标题
#define LID_PROMPT_NETWORK_ERROR_MESSAGEBOX_CAPTION	17
#define LID_PNEMC_DEFAULT							_T("Warning")
// 网络错误消息框内容
#define LID_PROMPT_NETWORK_ERROR_MESSAGEBOX_TEXT	18
#define LID_PNEMT_DEFAULT							_T("You haven't update your software for more than %d days.\rMaybe there's something wrong with your network environment.\rPlease update your software manually.")