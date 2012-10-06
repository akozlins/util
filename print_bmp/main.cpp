
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <CommDlg.h>

void print(HWND hwnd)
{
  wchar_t file[1024] = { 0 };

  OPENFILENAME ofn;
  memset(&ofn, 0, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = L"BMP Windows Bitmap Format (*.bmp)\0*.bmp\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFile = file;
  ofn.nMaxFile = 1024;
  ofn.lpstrInitialDir = L"C:\\";
  ofn.lpstrTitle = L"Select File";
  ofn.Flags = OFN_PATHMUSTEXIST;
  ofn.lpstrDefExt = L"bmp";

  if(GetSaveFileName(&ofn) == 0) return;

  HBITMAP hbitmap = (HBITMAP)LoadImage(0, file, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
  if(hbitmap == 0) return;

  PRINTDLG printDlg;
  memset(&printDlg, 0, sizeof(printDlg));
  printDlg.lStructSize = sizeof(printDlg);
  printDlg.hwndOwner = hwnd;
  printDlg.Flags = PD_RETURNDC;

  if(PrintDlg(&printDlg) == 0) return;

  try
  {
    if(!(GetDeviceCaps(printDlg.hDC, RASTERCAPS) & RC_BITBLT)) throw L"";

    DOCINFO docInfo;
    memset(&docInfo, 0, sizeof(docInfo));
    docInfo.cbSize = sizeof(docInfo);
    docInfo.lpszDocName = L""; // name of the document

    StartDoc(printDlg.hDC, &docInfo);
    StartPage(printDlg.hDC);

    HDC hdc = CreateCompatibleDC(printDlg.hDC);
    if(hdc == 0) throw L"";

    HGDIOBJ obj = SelectObject(hdc, hbitmap);
    if(obj == 0)
    {
      DeleteDC(hdc);
      throw L"";
    }

    BITMAP bm;
    GetObject(hbitmap, sizeof(bm), &bm);
    BitBlt(printDlg.hDC, 0, 0, bm.bmWidth, bm.bmHeight, hdc, 0, 0, SRCCOPY);
    SelectObject(hdc, obj);

    DeleteDC(hdc);

    EndPage(printDlg.hDC);
    EndDoc(printDlg.hDC);
  }
  catch(...)
  {
  }

  if(hbitmap != 0) DeleteObject(hbitmap);

  DeleteDC(printDlg.hDC);
  if(printDlg.hDevMode != 0) GlobalFree(printDlg.hDevMode);
  if(printDlg.hDevNames != 0) GlobalFree(printDlg.hDevNames);
}

void main(void)
{
  print(0);
}
