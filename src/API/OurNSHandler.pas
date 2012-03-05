unit OurNSHandler;

(* Simple demo for temporary pluggable NameSpacehandler
   To add more functionality to the namespacehandler
   take a look at the following link:

 http://msdn.microsoft.com/workshop/networking/pluggable/pluggable.asp

For discussions about APP, namespacehandlers, mimefilters and
other delphi-webbrowser topics use:

  http://www.egroups.com/group/delphi-webbrowser/info.html

   Go to http://www.euromind.com/iedelphi for more info about
   this sample and updated versions.
           Per Lindsø Larsen, Nov. 1999
*)

interface
uses
  Classes, Windows, Forms, Axctrls, dialogs, SysUtils, ComObj, ActiveX, UrlMon;

const
  Class_OurNSHandler: TGUID = '{0EB00680-8FA1-11D3-96C7-829E3EA50A30}';
  // Create your own GUID - In Delphi IDE: Ctrl-Shift-G

type
  TOurNSHandler = class(TComObject, IInternetProtocol)
  private
    Url: string;
    Written, TotalSize: Integer;
    ProtSink: IInternetProtocolSink;
  protected
// IInternetProtocol Methods
    function Start(szUrl: PWideChar; OIProtSink: IInternetProtocolSink;
      OIBindInfo: IInternetBindInfo; grfPI, dwReserved: DWORD): HResult; stdcall;
    function Continue(const ProtocolData: TProtocolData): HResult; stdcall;
    function Abort(hrReason: HResult; dwOptions: DWORD): HResult; stdcall;
    function Terminate(dwOptions: DWORD): HResult; stdcall;
    function Suspend: HResult; stdcall;
    function Resume: HResult; stdcall;
    function Read(pv: Pointer; cb: ULONG; out cbRead: ULONG): HResult; stdcall;
    function Seek(dlibMove: LARGE_INTEGER; dwOrigin: DWORD;
      out libNewPosition: ULARGE_INTEGER): HResult; stdcall;
    function LockRequest(dwOptions: DWORD): HResult; stdcall;
    function UnlockRequest: HResult; stdcall;
  end;
  
var
  slAuth: TStringList;

implementation

uses comserv;

function TOurNSHandler.Start(szUrl: PWideChar; OIProtSink: IInternetProtocolSink;
  OIBindInfo: IInternetBindInfo; grfPI, dwReserved: DWORD): HResult; stdcall;
 var
   grfBINDF: DWORD;
  bindData: TBindInfo;
  dwSize: DWORD;

  pData, FPostData: Pointer;
  dwPostDataSize: DWORD;
  pszMIMEType: POleStr;
  iStreamPos: Int64;
  stData: IStream;
  pcbRead: PLongInt;
  oStream: TOleStream;
begin
(* We receive all http://-URL's here and let the
default protocolhandler take over if we don't find
our namespace.
*)
  Url := SzUrl;
  Result := INET_E_USE_DEFAULT_PROTOCOLHANDLER;
  if (Pos('api.php',url) > 0) then
  begin
    written := 0;
    ProtSink := OIProtSink; //Get interface to Transaction handlers IInternetnetProtocolSink

  //check to see if this is a post request, and handle any post data
  bindData.cbSize := Sizeof(TBindInfo);
  if (Succeeded(OIBindInfo.GetBindInfo(grfBINDF, bindData))) then
  begin
    case (bindData.dwBindVerb) of
      BINDVERB_POST:
        begin
          dwPostDataSize := bindData.cbstgmedData;
          //SetLength(FPostData, dwPostDataSize);
          GetMem(FPostData, dwPostDataSize);

          case (bindData.stgmedData.tymed) of
            TYMED_HGLOBAL:
              begin
//              bindData.stgmedData.stm
                pData := GlobalLock(bindData.stgmedData.hGlobal);
                if (Assigned(pData)) then
                begin
                  //make a copy of the post data
                  Move(pData^, PChar(FPostData)^, dwPostDataSize);

                  //unlock the buffer now
                  GlobalUnlock(bindData.stgmedData.hGlobal);
                end;
              end;
            TYMED_ISTREAM:
              begin
                if (Assigned(bindData.stgmedData.stm)) then
                begin
                  oStream :=
                TOleStream.Create(IStream(bindData.stgmedData.stm));
                  try
                    oStream.Seek(0, soFromBeginning);
                    {if (oStream.Read(PChar(FPostData)^,
dwPostDataSize) <> dwPostDataSize) then
                      SetLength(FPostData, 0);  }
                  finally
                    FreeAndNil(oStream);
                  end;
                end;
              end;
          end;

          slAuth.Add(url + PChar(FPostData));

          FreeMem(FPostData);

          //get the mime type now
          //frmMain.memo1.lines.Add('Before BindInfo data');
          if
        (Succeeded(OIBindInfo.GetBindString(BINDSTRING_POST_DATA_MIME,
        @pszMIMEType, 1, dwSize))) then
          begin
            //FPostMimeType := String(pszMIMEType);
            CoTaskMemFree(pszMIMEType);
            pszMIMEType := nil;
          end else
          begin
            //FPostMimeType := 'application/x-www-form-urlencoded';
          end;

          Url := SzUrl;
          written := 0;
          ProtSink := OIProtSink;
          ProtSink.ReportData(BSCF_FIRSTDATANOTIFICATION or
                BSCF_LASTDATANOTIFICATION or BSCF_DATAFULLYAVAILABLE, TotalSize, TotalSize);
        (* -> Here our Read Method is called by transaction handler*)
          ProtSink.ReportResult(S_OK, S_OK, nil);
        (* Report result to transaction handler. Our Terminate method will be called *)
          Result := S_OK;
        end;
    end;
    end;
  end;
end;




function TOurNSHandler.Read(pv: Pointer; cb: ULONG; out cbRead: ULONG): HResult;
begin
(*Read Data from DataStream to Browser/URLMON *)
{  DataStream.Read(pv, cb, @cbRead);
  Inc(written, cbread);
  if (written = totalSize) then  result := S_FALSE else Result := E_PENDING;
}end;


function TOurNSHandler.Terminate(dwOptions: DWORD): HResult; stdcall;
begin
  Protsink._Release;
  result := S_OK;
end;

function TOurNSHandler.LockRequest(dwOptions: DWORD): HResult; stdcall;
begin
  result := S_OK;
end;

function TOurNSHandler.UnlockRequest: HResult;
begin
  result := S_OK;
end;

function TOurNSHandler.Continue(const ProtocolData: TProtocolData): HResult;
begin
  result := S_OK;
end;

function TOurNSHandler.Abort(hrReason: HResult; dwOptions: DWORD): HResult; stdcall;
begin
  result := E_NOTIMPL;
end;

function TOurNSHandler.Suspend: HResult; stdcall;
begin
  result := E_NOTIMPL;
end;

function TOurNSHandler.Resume: HResult; stdcall;
begin
  result := E_NOTIMPL;
end;

function TOurNSHandler.Seek(dlibMove: LARGE_INTEGER; dwOrigin: DWORD;
  out libNewPosition: ULARGE_INTEGER): HResult;
begin
  result := E_NOTIMPL;
end;

initialization
  begin
    TComObjectFactory.Create(ComServer, TOurNSHandler, Class_OurNSHandler,
      'OurNSHandler', 'OurNSHandler', ciMultiInstance, tmApartment);
    slAuth := TStringList.Create;
  end;

finalization
  slAuth.Free;
  
end.

