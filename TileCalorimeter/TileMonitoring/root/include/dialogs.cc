/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//
// This file contains the class InputDialog.
// An InputDialog object prompts for an input string using a simple
// dialog box. The InputDialog class is also a good example of how
// to use the ROOT GUI classes via the interpreter. Since interpreted
// classes can not call virtual functions via base class pointers, all
// GUI objects are used by composition instead of by inheritance.
//
// This file contains also some utility functions that use
// the InputDialog class to either get a string, integer or
// floating point number. There are also two functions showing
// how to use the file open and save dialogs. The utility functions are:
//
// const char *OpenFileDialog()
// const char *SaveFileDialog()
// const char *GetStringDialog(const char *prompt, const char *defval)
// Int_t GetIntegerDialog(const char *prompt, Int_t defval)
// Float_t GetFloatDialog(const char *prompt, Float_t defval)
//
// To use the InputDialog class and the utility functions you just
// have to load the Dialogs.C file as follows:
// .L Dialogs.C
//
// Now you can use them like:
// {
//    const char *file = OpenFileDialog();
//    Int_t run   = GetIntegerDialog("Give run number:", 0);
//    Int_t event = GetIntegerDialog("Give event number:", 0);
//    printf("analyse run %d, event %d from file %s\n", run ,event, file);
// }
//

///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Input Dialog Widget                                                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

class InputDialog {

private:
   TGTransientFrame *fDialog;  // transient frame, main dialog window
   TGTextEntry      *fTE;      // text entry widget containing
   TList            *fWidgets; // keep track of widgets to be deleted in dtor
   char             *fRetStr;  // address to store return string

public:
   InputDialog(const char *prompt, const char *defval, char *retstr);
   InputDialog (const InputDialog&) = delete;
   InputDialog& operator= (const InputDialog&) = delete;
   ~InputDialog();
   void ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void Cancel(void);
};

InputDialog::~InputDialog()
{
   // Cleanup dialog.

   fWidgets->Delete();
   delete fWidgets;

   delete fTE;
   delete fDialog;
}

InputDialog::InputDialog(const char *prompt, const char *defval, char *retstr)
{
   // Create simple input dialog.

   fWidgets = new TList;

   TGWindow *main = gClient->GetRoot();
   fDialog = new TGTransientFrame(main, main, 10, 10);

   // command to be executed by buttons and text entry widget
   char cmd[128];
   sprintf(cmd, "{long r__ptr=0x%lx; ((InputDialog*)r__ptr)->ProcessMessage($MSG,$PARM1,$PARM2);}", (Long_t)this);
   //sprintf(cmd,"");
   // create prompt label and textentry widget
   TGLabel *label = new TGLabel(fDialog, prompt);
   fWidgets->Add(label);

   TGTextBuffer *tbuf = new TGTextBuffer(256);  //will be deleted by TGtextEntry
   tbuf->AddText(0, defval);

   fTE = new TGTextEntry(fDialog, tbuf);
   fTE->Resize(260, fTE->GetDefaultHeight());
   fTE->SetCommand(cmd);

   TGLayoutHints *l1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 0);
   TGLayoutHints *l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
   fWidgets->Add(l1);
   fWidgets->Add(l2);

   fDialog->AddFrame(label, l1);
   fDialog->AddFrame(fTE, l2);

   // create frame and layout hints for Ok and Cancel buttons
   TGHorizontalFrame *hf = new TGHorizontalFrame(fDialog, 60, 20, kFixedWidth);
   TGLayoutHints     *l3 = new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0);

   // put hf as last in list to be deleted
   fWidgets->Add(l3);

   // create OK and Cancel buttons in their own frame (hf)
   UInt_t  nb = 0, width = 0, height = 0;
   TGTextButton *b;

   b = new TGTextButton(hf, "&Ok", cmd, 1);
   fWidgets->Add(b);
   b->Associate(fDialog);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

   sprintf(cmd,"{ long r__ptr=0x%lx;((InputDialog*)r__ptr)->Cancel();}",(Long_t)this);
   b = new TGTextButton(hf, "&Cancel", cmd, 2);
   fWidgets->Add(b);
   b->Associate(fDialog);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

   // place button frame (hf) at the bottom
   TGLayoutHints *l4 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
   fWidgets->Add(l4);
   fWidgets->Add(hf);

   fDialog->AddFrame(hf, l4);

   // keep buttons centered and with the same width
   hf->Resize((width + 20) * nb, height);

   // set dialog title
   fDialog->SetWindowName("Get Input");

   // map all widgets and calculate size of dialog
   fDialog->MapSubwindows();

   width  = fDialog->GetDefaultWidth();
   height = fDialog->GetDefaultHeight();

   fDialog->Resize(width, height);

   // position relative to the parent window (which is the root window)
   Window_t wdum;
   int      ax, ay;

   gVirtualX->TranslateCoordinates(main->GetId(), main->GetId(),
                          (((TGFrame *) main)->GetWidth() - width) >> 1,
                          (((TGFrame *) main)->GetHeight() - height) >> 1,
                          ax, ay, wdum);
   fDialog->Move(ax, ay);
   fDialog->SetWMPosition(ax, ay);

   // make the message box non-resizable
   fDialog->SetWMSize(width, height);
   fDialog->SetWMSizeHints(width, height, width, height, 0, 0);

   fDialog->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                                       kMWMDecorMinimize | kMWMDecorMenu,
                        kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
                                       kMWMFuncMinimize,
                        kMWMInputModeless);

   // popup dialog and wait till user replies
   fDialog->MapWindow();

   fRetStr = retstr;

   gClient->WaitFor(fDialog);
}

void InputDialog::Cancel()
{
strcpy(fRetStr, "-999");
delete this;

}

void InputDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle button and text enter events

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
             case kCM_BUTTON:
                switch (parm1) {
                   case 1:
                      // here copy the string from text buffer to return variable
                      strcpy(fRetStr, fTE->GetBuffer()->GetString());
                      delete this;
                      continue;

                   case 2:
                      fRetStr[0] = 0;
                      delete this;
                      continue;
                 }
              default:
                 continue;
          }
          continue;

       case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
             case kTE_ENTER:
                // here copy the string from text buffer to return variable
                strcpy(fRetStr, fTE->GetBuffer()->GetString());
                delete this;
                continue;
             default:
                continue;
          }
          continue;

       default:
          continue;
   }
}


//--- Utility Functions --------------------------------------------------------

const char *OpenFileDialog()
{
   // Prompt for file to be opened. Depending on navigation in
   // dialog the current working directory can be changed.
   // The returned file name is always with respect to the
   // current directory.

   const char *gOpenAsTypes[] = {
      "ROOT files",   "*.root",
      "All files",    "*",
       0,              0
   };

   static TGFileInfo fi;
   fi.fFileTypes = gOpenAsTypes;
   new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &fi);

   return fi.fFilename;
}

const char *SaveFileDialog()
{
   // Prompt for file to be saved. Depending on navigation in
   // dialog the current working directory can be changed.
   // The returned file name is always with respect to the
   // current directory.

   const char *gSaveAsTypes[] = {
      "Macro files",  "*.C",
      "ROOT files",   "*.root",
      "PostScript",   "*.ps",
      "Encapsulated PostScript", "*.eps",
      "Gif files",    "*.gif",
      "All files",    "*",
       0,              0
   };

   static TGFileInfo fi;
   fi.fFileTypes = gSaveAsTypes;
   new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDSave, &fi);

   return fi.fFilename;
}

const char *GetStringDialog(const char *prompt, const char *defval)
{
   // Prompt for string. The typed in string is returned.

   static char answer[128];

   new InputDialog(prompt, defval, answer);

   return answer;
}

Int_t GetIntegerDialog(const char *prompt, Int_t defval)
{
   // Prompt for integer. The typed in integer is returned.

   static char answer[32];

   char defv[32];
   sprintf(defv, "%d", defval);

   new InputDialog(prompt, defv, answer);

   return atoi(answer);
}

Float_t GetFloatDialog(const char *prompt, Float_t defval)
{
   // Prompt for float. The typed in float is returned.

   static char answer[32];

   char defv[32];
   sprintf(defv, "%f", defval);

   new InputDialog(prompt, defv, answer);

   return atof(answer);
}
