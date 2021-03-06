/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

$LicenseInfo:firstyear=2014&license=lgpl$
Copyright (C) 2014, Linden Research, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
version 2.1 of the License only.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>
or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
$/LicenseInfo$
*/

#include "Common.h"
#include "InstallINToolPage.h"

#include "MainApp.h"
#include "usercore/UserCoreI.h"
#include "usercore/ToolManagerI.h"

using namespace UI::Forms::ItemFormPage;

InstallINToolPage::InstallINToolPage(wxWindow* parent, UserCore::ToolManagerI* pToolManager) 
	: BaseInstallPage(parent)
	, m_pToolManager(pToolManager)
{
	if (!m_pToolManager)
		m_pToolManager = GetUserCore()->getToolManager();

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &InstallINToolPage::onButtonPressed, this);

	m_labLabel = new gcStaticText( this, wxID_ANY, Managers::GetString(L"#IF_INTOOL_WAIT_OTHER"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pbProgress = new gcSpinnerProgBar( this, wxID_ANY, wxDefaultPosition, wxSize( -1,22 ) );
	m_butCancel = new gcButton( this, wxID_ANY, Managers::GetString(L"#CLOSE"), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer4->Add( m_butCancel, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	fgSizer1->Add( m_labLabel, 0, wxEXPAND|wxLEFT|wxTOP, 5 );
	fgSizer1->Add( m_pbProgress, 0, wxALL|wxEXPAND, 5 );
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	fgSizer1->Add( bSizer4, 0, wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	this->setParentSize(-1, 140);

	m_bCompleted = false;

	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_NORMAL);
}

InstallINToolPage::~InstallINToolPage()
{
}

void InstallINToolPage::init()
{
}

void InstallINToolPage::onButtonPressed(wxCommandEvent& event)
{
	GetParent()->Close();
}

//res will be 1 on error (i.e. hash fail) and 0 if no error
void InstallINToolPage::onComplete(uint32& res)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_NONE);

	m_pbProgress->setProgress(100);
}

void InstallINToolPage::onError(gcException& e)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_ERROR);

	gcErrorBox(GetParent(), "#IF_IERRTITLE", "#IF_IERROR", e);
}

void InstallINToolPage::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{
	if (info.flag == 1)
	{
		DesuraId id(info.totalAmmount);

		std::string name = m_pToolManager->getToolName(id);
		m_labLabel->SetLabel(gcWString(Managers::GetString(L"#IF_INTOOL_LABEL"), name));
	}
	else if (info.flag == 2)
	{
		m_labLabel->SetLabel(Managers::GetString(L"#IF_INTOOL_WAIT_HELPER"));
		m_pbProgress->setCaption("");
	}
	else
	{
		gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
		if (par)
			par->setProgress(info.percent);

		m_pbProgress->setProgress(info.percent);

		gcString progLabel(Managers::GetString("#PROGRESS_INFO"), info.doneAmmount, info.totalAmmount);
		m_pbProgress->setCaption(progLabel);
	}

	Update();
}
