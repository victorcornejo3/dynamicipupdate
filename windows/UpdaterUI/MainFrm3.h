// Copyright (c) 2009 OpenDNS Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MAIN_FRM2_H__
#define MAIN_FRM2_H__

#if MAIN_FRM == 3

#include "NTray.h"
#include "DnsQuery.h"
#include "UpdaterThread.h"
#include "Errors.h"
#include "Http.h"
#include "MiscUtil.h"
#include "LayoutSizer.h"
#include "Prefs.h"
#include "RtfBuilder.h"

#include "SelectNetworkDlg.h"
#include "SendIPUpdate.h"
#include "SignInDlg.h"

enum SimulatedError {
	SE_NO_ERROR = 0,
	SE_NO_INTERNET = 1,
	SE_NOT_LOGGED_IN = 2,
	SE_NOT_USING_OPENDNS = 3,
	SE_NO_NETWORKS_CONFIGURED = 4,
	SE_NO_DYNAMIC_IP_NETWORKS = 5,
	SE_NO_NETWORK_SELECTED = 6,
	SE_IP_NOT_YOURS = 7,
	SE_BAD_AUTH = 8,
};

enum UIState {
	UI_STATE_VISIBLE,
	UI_STATE_HIDDEN,
	UI_STATE_SYSTRAY // not used yet
};

enum {
	SupressOneNetworkMsgFlag   = 0x1,
	SuppressNoDynamicIpMsgFlag = 0x2,
	SupressNoNetworksMsgFlag   = 0x4,
	SupressAll = SupressOneNetworkMsgFlag | SuppressNoDynamicIpMsgFlag | SupressNoNetworksMsgFlag
};

static bool g_showDebug = false;

#define DEFAULT_FONT_SIZE 12

#define TOP_BAR_TXT _T("OpenDNS Updater")

enum {
	IDC_BUTTON_CHANGE_ACCOUNT = 3000,
	IDC_BUTTON_CHANGE_NETWORK,
	IDC_BUTTON_SEND_IP_UPDATE,
	IDC_LINK_ABOUT,
	IDC_LINK_SETTINGS,
	IDC_LINK_LEARN_SETUP_OPENDNS,
	IDC_EDIT_STATUS,
	IDC_EDIT_UPDATE,
};

// TODO: ensure those are unique by gathering them in a common file
enum {
	WMAPP_DO_LAYOUT = WM_APP + 33
	, WMAPP_UPDATE_STATUS
	, WMAPP_NEW_VERSION
	, WMAPP_NOTIFY_ABOUT_ERROR
	, WMAPP_NOTIFY_ICON
};

// why are we posting WMAPP_NOTIFY_ABOUT_ERROR message
enum NotifyErrorReason {
	NER_NOT_USING_OPENDNS = 0,
	NER_UPDATE_FAILED
};

static void CrashMe()
{
	char *p = (char*)0;
	*p = 0;
}

enum DividerLineType {
	ETCHED_LINE = 1,
	SOLID_LINE = 2
};

static const int TOP_BAR_DY = 32;
static const int LEFT_MARGIN = 8;
static const int RIGHT_MARGIN = LEFT_MARGIN;
static const int DIVIDER_TEXT_LEFT_MARGIN = 6;
static const int DIVIDER_TEXT_RIGHT_MARGIN = 6;
static const int Y_SPACING = 4;

#define UI_VARIANT 2

#if UI_VARIANT == 1
	static const COLORREF colWinBg = RGB(0xef, 0xeb, 0xde);
	static const COLORREF colEditBg = colWinBg;
	static const COLORREF colDivLine = RGB(0xde, 0xdb, 0xde);
	static const int Y_START = 4;
	static const int DIVIDER_Y_SPACING = 8;
	static const int DIV_LINE_OFF_Y = 0;
	static const int DIVIDER_TEXT_TOP_MARGIN = 4;
	static const int DIVIDER_TEXT_BOTTOM_MARGIN = 4;
	static const DividerLineType lineType = ETCHED_LINE;
	static const int DIVIDER_LINE_Y_OFF = 16;
	static const int EDIT_BOX_Y_OFF = 10;

	static const int EDIT_MARGIN_X = LEFT_MARGIN;
	static const TCHAR *EDIT_FONT_NAME = 0;
	static const int EDIT_FONT_SIZE = 10;
#else
	static const COLORREF colWinBg = RGB(0xf7, 0xfb, 0xff);

	// background and border color for edit box showing error message(s)
	static const COLORREF colEditBg    = RGB(0xff, 0xff, 0xe7);
	static const COLORREF colEditFrame = RGB(0xf7, 0xe3, 0x84);

	// background and border color for edit box showing 'update available' message
	static const COLORREF colEditUpdateBg    = RGB(0xe7, 0xfb, 0xe7);
	static const COLORREF colEditUpdateFrame = RGB(0x00, 0x9a, 0x00);

	static const int DIVIDER_LINE_Y_OFF = 12;

	//static const COLORREF colEditBg    = RGB(0xff, 0xdb, 0x18);
	//static const COLORREF colEditBg    = RGB(0xff, 0xfb, 0xbd);
	//static const COLORREF colEditFrame = RGB(0xef, 0xc3, 0x00);

	static const COLORREF colDivLine = RGB(0xde, 0xdb, 0xde);
	static const int Y_START = 6;
	static const int DIVIDER_Y_SPACING = 0;
	static const int DIVIDER_TEXT_TOP_MARGIN = 6;
	static const int DIVIDER_TEXT_BOTTOM_MARGIN = 0;
	static const int DIV_LINE_OFF_Y = 4;
	static const DividerLineType lineType = SOLID_LINE;
	static const int EDIT_BOX_Y_OFF = 13;

	static const int EDIT_MARGIN_X = LEFT_MARGIN + 4;
	static const TCHAR *EDIT_FONT_NAME = _T("Arial");
	static const int EDIT_FONT_SIZE = 8;

	static const int UPDATE_BITMAP_MARGIN_X_LEFT = 4;
	static const int UPDATE_BITMAP_MARGIN_X_RIGHT = 4;
	static const int UPDATE_BITMAP_DX = 15;
	static const int UPDATE_BITMAP_DX_TOTAL = UPDATE_BITMAP_MARGIN_X_LEFT + UPDATE_BITMAP_MARGIN_X_RIGHT + UPDATE_BITMAP_DX; 
#endif

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler, public UpdaterThreadObserver
{
public:
	DECLARE_FRAME_WND_CLASS(MAIN_WINDOWS_CLASS_NAME, IDR_MAINFRAME)

	UIState			m_uiState;
	CFont			m_defaultFont;
	CFont			m_defaultGuiFont;
	CFont			m_dividerTextFont;
	CFont			m_statusEditFont;
	CFont			m_topBarFont;
	CFont			m_buttonsFont;
	CFont			m_textFont;

	CRect			m_topBarRect;
	CRect			m_txtAccountRect;
	CRect			m_txtNetworkRect;
	CRect			m_txtIpAddressRect;
	CRect			m_txtStatusRect;
	CRect			m_txtUpdateRect;

	CTrayNotifyIcon	m_notifyIcon;

	CLinkCtrl		m_linkAbout;
	CLinkCtrl		m_linkLearnSetup;
	CLinkCtrl		m_linkSettings;
	CButton			m_buttonChangeAccount;
	CButton			m_buttonChangeConfigureNetwork;
	CButton			m_buttonUpdate;

	HICON			m_hIconOk;
	HICON			m_hIconErr;

	HBITMAP			m_updateBitmap;

	// info related to edit control for error message(s)
	RtfTextInfo		m_rtiError;
	CRichEditCtrl	m_editErrorMsg;
	bool			m_showStatusMsgEdit;
	int				m_editErrorMsgRequestedDy;
	int				m_editErrorMsgDx;
	int				m_minStatusEditDx;

	// info related to edit control for 'update available' message
	RtfTextInfo		m_rtiUpdate;
	CRichEditCtrl	m_editUpdateMsg;
	bool			m_showUpdateMsgEdit;
	int				m_editUpdateMsgRequestedDy;
	int				m_editUpdateMsgDx;
	int				m_minUpdateEditDx;

	int				m_topBarY;
	int				m_topBarX;

	int				m_btnDy;

	int				m_minutesSinceLastUpdate;

	TCHAR *			m_editFontName;
	TCHAR *			m_newVersionSetupFilepath;

	//static const COLORREF colWinBg = RGB(0xf9, 0xf9, 0xf9);
	//static const COLORREF colWinBg = RGB(0xff, 0xff, 0xff);
	//static const COLORREF colWinBg = RGB(0xd6, 0xdf, 0xf7);
	static const COLORREF colWhite	 = RGB(0xff,0xff,0xff);
	static const COLORREF colBlack	 = RGB(0x00, 0x00, 0x00);
	static const COLORREF colRed	 = RGB(0xff, 0x00, 0x00);
	static const COLORREF colRed2    = RGB(0xf7, 0x0c, 0x08);

	IP4_ADDRESS			m_ipFromDns;
	CString				m_ipFromDnsStr;
	TCHAR *				m_ipFromHttp;
	UpdaterThread *		m_updaterThread;
	int					m_minWinDx, m_minWinDy;
	IpUpdateResult		m_ipUpdateResult;
	SimulatedError		m_simulatedError;
	bool				m_forceExitOnClose;
	bool				m_hadFirstLayout;

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(IDM_RUN_HIDDEN, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(g_errorNotifMsg, OnErrorNotif)
		MESSAGE_HANDLER_EX(WMAPP_DO_LAYOUT, OnLayout)
		MESSAGE_HANDLER_EX(WMAPP_UPDATE_STATUS, OnUpdateStatus)
		MESSAGE_HANDLER_EX(WMAPP_NEW_VERSION, OnNewVersion)
		MESSAGE_HANDLER_EX(WMAPP_NOTIFY_ABOUT_ERROR, OnNotifyAboutError)
		MESSAGE_HANDLER_EX(WMAPP_NOTIFY_ICON, OnNotifyIcon)
		COMMAND_HANDLER_EX(IDC_BUTTON_CHANGE_ACCOUNT, BN_CLICKED, OnChangeAccount)
		COMMAND_HANDLER_EX(IDC_BUTTON_CHANGE_NETWORK, BN_CLICKED, OnChangeNetwork)
		COMMAND_HANDLER_EX(IDC_BUTTON_SEND_IP_UPDATE, BN_CLICKED, OnSendUpdate)

		COMMAND_ID_HANDLER_EX(IDM_EXIT, OnExit)
		COMMAND_ID_HANDLER_EX(IDM_TOGGLE_WINDOW, OnToggleWindow)
		COMMAND_ID_HANDLER_EX(IDM_RUN_HIDDEN, OnRunHidden)
		COMMAND_ID_HANDLER_EX(IDM_IP_UPDATES_HISTORY, OnIpUpdatesHistory)
		COMMAND_ID_HANDLER_EX(IDM_PREFERENCES, OnPreferences)

		NOTIFY_HANDLER_EX(IDC_EDIT_STATUS, EN_LINK, OnLinkStatusEdit)
		NOTIFY_HANDLER_EX(IDC_EDIT_STATUS, EN_REQUESTRESIZE, OnRequestResize)
		NOTIFY_HANDLER_EX(IDC_EDIT_STATUS, EN_SELCHANGE, OnSelChange)

		NOTIFY_HANDLER_EX(IDC_EDIT_UPDATE, EN_LINK, OnLinkStatusEdit)
		NOTIFY_HANDLER_EX(IDC_EDIT_UPDATE, EN_REQUESTRESIZE, OnRequestResize)
		NOTIFY_HANDLER_EX(IDC_EDIT_UPDATE, EN_SELCHANGE, OnSelChange)

		NOTIFY_HANDLER_EX(IDC_LINK_ABOUT, NM_CLICK, OnLinkAbout)
		NOTIFY_HANDLER_EX(IDC_LINK_SETTINGS, NM_CLICK, OnLinkSettings)
		NOTIFY_HANDLER_EX(IDC_LINK_LEARN_SETUP_OPENDNS, NM_CLICK, OnLinkLearnSetupOpenDns)

		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	CMainFrame();
	~CMainFrame();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	virtual void OnIpCheckResult(IP4_ADDRESS myIp);
	virtual void OnIpUpdateResult(char *ipUpdateRes);
	virtual void OnNewVersionAvailable(TCHAR *setupFilePath);

	void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnNewVersion(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnNotifyAboutError(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/);
	LRESULT OnNotifyIcon(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);

	void OnExit(UINT /*uCode*/, int /*nID*/, HWND /*hWndCtl*/);
	void OnToggleWindow(UINT /*uCode*/, int /*nID*/, HWND /*hWndCtl*/);
	void OnRunHidden(UINT /*uCode*/, int /*nID*/, HWND /*hWndCtl*/);
	void OnIpUpdatesHistory(UINT /*uCode*/, int /*nID*/, HWND /*hWndCtl*/);
	void OnPreferences(UINT /*uCode*/, int /*nID*/, HWND /*hWndCtl*/);

	void OnClose();
	void OnDestroy();
	BOOL OnEraseBkgnd(CDCHandle dc);

	bool IsUsingOpenDns();
	bool NoNetworksConfigured();
	bool IsLoggedIn();
	bool NoInternetConnectivity();
	bool NetworkNotSelected();
	bool NoDynamicNetworks();
	bool ShowLastUpdated();

	HBRUSH OnCtlColorStatic(CDCHandle dc, CWindow wnd);

	void SetRtfLinks(CRichEditCtrl *edit, RtfTextInfo *rti);

	void BuildStatusEditRtf(RtfTextInfo& ti);
	void UpdateErrorEdit(bool doLayout=true);

	void BuildUpdateEditRtf(RtfTextInfo& ti);
	void UpdateUpdateEdit(bool doLayout=true);

	TCHAR *LastUpdateTxt();
	TCHAR *IpAddress();
	LRESULT OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void ChangeNetwork(int supressFlags);
	LRESULT OnRequestResize(LPNMHDR pnmh);
	LRESULT OnSelChange(LPNMHDR /*pnmh*/);
	void ChangeAccount();

	LRESULT OnLinkStatusEdit(LPNMHDR pnmh);
	LRESULT OnLinkAbout(LPNMHDR /*pnmh*/);
	LRESULT OnLinkSettings(LPNMHDR /*pnmh*/);
	LRESULT OnLinkLearnSetupOpenDns(LPNMHDR /*pnmh*/);

	void DrawErrorText(CDCHandle *dc, int x, int y, const TCHAR *txt);
	void DrawDivider(CDCHandle dc, const TCHAR *txt, CRect& rect);
	void OnChangeAccount(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	void OnChangeNetwork(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	void OnSendUpdate(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
	int SizeDividerLineText(TCHAR *txt, int y, int clientDx, CRect& rectOut);
	void DoLayout();
	LRESULT OnLayout(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnErrorNotif(UINT /*uMsg*/, WPARAM specialCmd, LPARAM /*lParam*/);
	LRESULT OnUpdateStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	void UpdateLastUpdateText();
	void OnSize(UINT nType, CSize /*size*/);

	void StartDownloadNetworks(char *token, int supressFlags = 0);
	LRESULT OnDownloadNetworks(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/);
	NetworkInfo *SelectNetwork(NetworkInfo *ni);

	bool GetLastIpUpdateTime();
	bool DnsVsHttpIpMismatch();
	void SizeButtons(int& dxOut, int& dyOut);
	void SwitchToVisibleState();
	void SwitchToHiddenState();

	void ToggleNagging();

	int OnCreate(LPCREATESTRUCT /* lpCreateStruct */);
};

NetworkInfo *MakeFirstNetworkDynamic(NetworkInfo *ni);

#endif

#endif
