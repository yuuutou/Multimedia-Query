#ifndef UNICODE
#define UNICODE
#endif 


#include <stdio.h>
#include <windowsx.h>
#include <Shlwapi.h>
#include "Extractor.h"
#include "Comparator.h"


HINSTANCE g_hInst;  // The handle to the instance of the current module
HWND hProgress1;	// The handle to the query track bar
HWND hProgress2;	// The handle to the result track bar
HWND hWait;			// The handle to the wait progress bar
char dir[MAX_PATH_LENGTH];
string resultPath;
double sim[FILE_NUM][SIM_NUM];
Player *pl;
Player *pl2;
Extractor *ext;
Comparator *comp;
bool canPlayQuery;
bool canPlayResult;

using namespace cv;

BOOL OnInitWaitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
	SetWindowPos(hWnd,NULL, 380, 200, 0, 0, SWP_NOSIZE);

	// Load and register Progress control class
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_PROGRESS_CLASS;
    if (!InitCommonControlsEx(&iccx))
        return -1;

    // Create the progress bar control
    RECT rc = { 40, 30, 400, 40 };
    hWait = CreateWindowEx(0, PROGRESS_CLASS, 0, 
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, (HMENU)0, g_hInst, 0);

	SendMessage(hWait, PBM_SETRANGE, 0, MAKELPARAM(0, QUERY_LENGTH));
    SendMessage(hWait, PBM_SETSTEP, (WPARAM) 1, 0);

	SetDlgItemText(hWnd,IDC_EDIT_LINE1,L"Start...");
	UpdateWindow(hWnd);

    return TRUE;
}

void OnCloseProgress(HWND hWnd)
{
    EndDialog(hWnd, 0);
}

INT_PTR CALLBACK waitDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDateTimePickDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitWaitDialog);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnCloseProgress);

    default:
        return FALSE;	// Let system deal with msg
    }
    return 0;
}

BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
	// Load and register track bar control class
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_BAR_CLASSES;
    if (!InitCommonControlsEx(&iccx))
        return -1;

    // Create the track bar control for Query
    RECT rc = { 70, 480, 370, 40 };
    hProgress1 = CreateWindowEx(0, TRACKBAR_CLASS, 0, 
        WS_CHILD | WS_VISIBLE | TBS_ENABLESELRANGE, rc.left, rc.top, rc.right, rc.bottom, 
        hWnd, (HMENU)0, g_hInst, 0);

	// Create the track bar control for Result
    RECT rc2 = { 530, 480, 370, 40 };
    hProgress2 = CreateWindowEx(0, TRACKBAR_CLASS, 0, 
        WS_CHILD | WS_VISIBLE | TBS_ENABLESELRANGE, rc2.left, rc2.top, rc2.right, rc2.bottom, 
        hWnd, (HMENU)0, g_hInst, 0);


	pl->init(hWnd,"Query",IDC_PIC);
	pl2->init(hWnd,"Result",IDC_PIC2);

	//init the sim pic area
	Mat image;
    image = imread("simPicStarter.bmp", IMREAD_COLOR);
	namedWindow( "Sim",WINDOW_AUTOSIZE);
	HWND hWnd2 = (HWND) cvGetWindowHandle("Sim");
	HWND hParent = ::GetParent(hWnd2); 
	::SetParent(hWnd2, GetDlgItem(hWnd, IDC_SIM)); 
	::ShowWindow(hParent, SW_HIDE);
	imshow( "Sim", image );

    return TRUE;
}

void OnClose(HWND hWnd)
{
	pl->setPause();
	pl->stopPlay();
	pl2->setPause();
	pl2->stopPlay();
    EndDialog(hWnd, 0);
}

void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_BUTTON_EXIT:
		{
			pl->setPause();
			pl->stopPlay();
			pl2->setPause();
			pl2->stopPlay();
			EndDialog(hWnd, 0);
		}
        break;
	
	case IDC_BUTTON_PLAY:
		{
			if(!canPlayQuery)
				return;
			if(pl->isPause()){
				pl->continuePlay();
				pl->playDir(hProgress1,string(dir),QUERY_LENGTH,"Query");
			}
		}
		break;

	case IDC_BUTTON_PLAY2:
		{
			if(!canPlayResult)
				return;
			if(pl2->isPause()){
				pl2->continuePlay();
				pl2->playDir(hProgress2,resultPath,DATABASE_LENGTH,"Result");
			}
		}
		break;

	case IDC_BUTTON_PAUSE:
		{
			pl->setPause();
		}
		break;

	case IDC_BUTTON_PAUSE2:
		{
			pl2->setPause();
		}
		break;

	case IDC_BUTTON1:
		{
			//clean the view box
			HWND hwndList = GetDlgItem(hWnd, IDC_LIST);
			if(canPlayQuery){
				canPlayResult = false;
				for(int i=0;i<FILE_NUM;i++)
					SendMessage(hwndList, LB_DELETESTRING, 0, NULL); 
			}

			wchar_t buf1[20];
			if (!GetDlgItemText(hWnd, IDC_QUERYPATH, buf1, 20)){
				MessageBox(NULL,L"Please indicate the query folder.",L"Hint",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			if(!PathFileExists(buf1)){
				MessageBox(NULL,L"This query folder does not exist.",L"Hint",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			WideCharToMultiByte(CP_ACP,0,buf1,-1,dir,sizeof(dir),NULL,NULL);

			//extract the query folder if there is no des
			string s = string(dir)+".des";
			if(!PathFileExists(wstring(s.begin(), s.end()).c_str())){
				HWND hDlg = CreateDialog(g_hInst, 
					MAKEINTRESOURCE(IDD_PROGRESS), 
					0, waitDlgProc);
				if (hDlg)
				{
					ShowWindow(hDlg, SW_SHOW);
				}
				ext->extractDir(dir,hDlg,hWait);
				EndDialog(hDlg, 0);
			}

			//compute the similarities
			string outFile;
			double max[FILE_NUM];			
			for(int i=0;i<FILE_NUM;i++){
				stringstream ss;
				outFile = comp->getFileName(i) + "-output.txt";
				comp->imgCompare(comp->getFileName(i), dir, GAP, outFile,sim[i],max[i]);
				ss<<comp->getFileName(i)<<" "<<max[i]<<"%";
				string s = ss.str();
				int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) wstring(s.begin(), s.end()).c_str()); 
				SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
				pl->createSimPic(i,sim[i]);
			}
			SetFocus(hwndList);

			//set the first frame of the query
			imshow( "Query", pl->getFirstQuery(dir));

			canPlayQuery = true;
			
		}
		break;

	case IDC_LIST:
		{
			if(canPlayQuery){
				HWND hwndList = GetDlgItem(hWnd, IDC_LIST); 
				int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
				int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);

				//show the visualized similarity
				imshow( "Sim", pl->getSimPic(i));

				//set the first frame of the result
				imshow( "Result", pl->getFirstResult(comp->getFileName(i)));

				//set the resultPath and reset the pl2
				resultPath = comp->getFileName(i);
				pl2->reset();

				canPlayResult = true;
			}	
		}
		break;

    case IDOK:
    case IDCANCEL:
        EndDialog(hWnd, 0);
        break;
    }
}


INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitDialog);

        // Handle the WM_COMMAND message in OnCommand
        HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	canPlayQuery = false;
	canPlayResult = false;
	pl = new Player("Query");
	pl2 = new Player("Result");
	ext = new Extractor();
	comp = new Comparator();
    g_hInst = hInstance;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
	delete pl;
	delete pl2;
	delete ext;
	delete comp;
	return GetLastError();
}