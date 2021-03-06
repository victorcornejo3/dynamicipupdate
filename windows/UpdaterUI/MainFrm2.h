// Copyright (c) 2009 OpenDNS, LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MAIN_FRM2_H__
#define MAIN_FRM2_H__

#if MAIN_FRM == 2

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
	SupressOneNetworkMsgFlag = 0x1,
	SuppressNoDynamicIpMsgFlag = 0x2
};

static bool g_showDebug = false;

#define DEFAULT_FONT_SIZE 12
#define EDIT_CTRL_FONT_SIZE 10

#define TOP_BAR_TXT _T("OpenDNS Updater")

enum {
	IDC_CHECK_SEND_UPDATES = 3000,
	IDC_BUTTON_CHANGE_ACCOUNT,
	IDC_BUTTON_CHANGE_NETWORK,
	IDC_BUTTON_SEND_IP_UPDATE,
	IDC_LINK_ABOUT,
	IDC_EDIT_STATUS,
};

// TODO: ensure those are unique by gathering them in a common file
enum {
	WMAPP_DO_LAYOUT = WM_APP + 33
	, WMAPP_UPDATE_STATUS
	, WMAPP_NEW_VERSION
};

static void CrashMe()
{
	char *p = (char*)0;
	*p = 0;
}

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

	CRect			m_topBarRect;
	CRect			m_txtAccountRect;
	CRect			m_txtNetworkRect;
	CRect			m_txtIpAddressRect;
	CRect			m_txtStatusRect;
	CRect			m_txtUpdateRect;

	CLinkCtrl		m_linkAbout;
	CButton			m_buttonSendIpUpdates;
	CButton			m_buttonChangeAccount;
	CButton			m_buttonChangeConfigureNetwork;
	CButton			m_buttonUpdate;

	RtfTextInfo		m_rti;

	CRichEditCtrl	m_editErrorMsg;
	int				m_editErrorMsgRequestedDy;
	int				m_editErrorMsgDx;
	int				m_minStatusEditDx;
	int				m_topBarY;
	int				m_topBarX;

	int				m_minutesSinceLastUpdate;

	TCHAR *			m_editFontName;

	static const COLORREF colDividerBg  = RGB(0xc6, 0xdf, 0xff);
	static const COLORREF colDividerTxt = RGB(0x52, 0x61, 0xb5);
	static const COLORREF colWhite	    = RGB(0xff,0xff,0xff);
	static const COLORREF colBlack	    = RGB(0x00, 0x00, 0x00);
	static const COLORREF colRed	    = RGB(0xff, 0x00, 0x00);

	HBRUSH				m_winBgColorBrush;

	static const int DIVIDER_Y_SPACING = 8;
	static const COLORREF colWinBg      = RGB(0xf7, 0xfb, 0xff);

	static const int TOP_BAR_DY = 32;
	static const int LEFT_MARGIN = 8;
	static const int RIGHT_MARGIN = LEFT_MARGIN;
	static const int DIVIDER_TEXT_LEFT_MARGIN = 6;
	static const int DIVIDER_TEXT_RIGHT_MARGIN = 6;
	static const int DIVIDER_TEXT_TOP_MARGIN = 4;
	static const int DIVIDER_TEXT_BOTTOM_MARGIN = 4;
	static const int Y_SPACING = 4;

	IP4_ADDRESS			m_ipFromDns;
	CString				m_ipFromDnsStr;
	TCHAR *				m_ipFromHttp;
	UpdaterThread *		m_updaterThread;
	int					m_minWinDx, m_minWinDy;
	IpUpdateResult		m_ipUpdateResult;
	SimulatedError		m_simulatedError;

	BEGIN_MSG_MAP(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_SIZE(OnSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_HANDLER_EX(g_errorNotifMsg, OnErrorNotif)
		MESSAGE_HANDLER_EX(WMAPP_DO_LAYOUT, OnLayout)
		MESSAGE_HANDLER_EX(WMAPP_UPDATE_STATUS, OnUpdateStatus)
		MESSAGE_HANDLER_EX(WMAPP_NEW_VERSION, OnNewVersion)
		COMMAND_HANDLER_EX(IDC_CHECK_SEND_UPDATES, BN_CLICKED, OnSendUpdatesButtonClicked)
		COMMAND_HANDLER_EX(IDC_BUTTON_CHANGE_ACCOUNT, BN_CLICKED, OnChangeAccount)
		COMMAND_HANDLER_EX(IDC_BUTTON_CHANGE_NETWORK, BN_CLICKED, OnChangeNetwork)
		COMMAND_HANDLER_EX(IDC_BUTTON_SEND_IP_UPDATE, BN_CLICKED, OnSendUpdate)
		NOTIFY_HANDLER_EX(IDC_EDIT_STATUS, EN_LINK, OnLinkStatusEdit)
		NOTIFY_HANDLER_EX(IDC_EDIT_STATUS, EN_REQUESTRESIZE, OnRequestResize)
		NOTIFY_HANDLER_EX(IDC_EDIT_STATUS, EN_SELCHANGE, OnSelChange)
		NOTIFY_HANDLER_EX(IDC_LINK_ABOUT, NM_CLICK, OnLinkAbout)

		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()

	CMainFrame();
	~CMainFrame();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	virtual void OnIpCheckResult(IP4_ADDRESS myIp);
	virtual void OnIpUpdateResult(char *ipUpdateRes);
	virtual void OnNewVersionAvailable(char *updateUrl);

	LRESULT OnNewVersion(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	void OnClose();
	BOOL OnEraseBkgnd(CDCHandle dc);
	bool IsLink(HWND hwnd);
	bool IsStatic(HWND /*hwnd*/);
	bool IsCheckBoxButton(HWND hwnd);
	HBRUSH OnCtlColorStatic(CDCHandle dc, CWindow wnd);
	void BuildStatusEditRtf(RtfTextInfo& ti);
	void UpdateStatusEdit(bool doLayout=true);
	TCHAR *LastUpdateTxt();
	TCHAR *IpAddress();
	void SetRtfLinks(RtfTextInfo *rti);
	LRESULT OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void ChangeNetwork(int supressFlags);
	LRESULT OnRequestResize(LPNMHDR pnmh);
	LRESULT OnSelChange(LPNMHDR /*pnmh*/);
	LRESULT OnLinkStatusEdit(LPNMHDR pnmh);
	void ChangeAccount();
	LRESULT OnLinkAbout(LPNMHDR /*pnmh*/);
	void DrawDivider(CDCHandle dc, const TCHAR *txt, CRect& rect);
	void OnSendUpdatesButtonClicked(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/);
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
	bool IsLoggedIn();
	bool GetLastIpUpdateTime();
	bool DnsVsHttpIpMismatch();
	void SizeButtons(int& dxOut, int& dyOut);
	void SwitchToVisibleState();
	void SwitchToHiddenState();

	int OnCreate(LPCREATESTRUCT /* lpCreateStruct */);
};

#endif

#endif
