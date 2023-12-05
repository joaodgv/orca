#include "main.h"
#include "simple.h"

IMPLEMENT_APP(OrcaHUD)

bool OrcaHUD::OnInit()
{
    Simple *simple = new Simple(wxT("Simple"));
    simple->Show(true);

    return true;
}