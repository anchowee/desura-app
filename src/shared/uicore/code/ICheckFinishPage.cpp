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
#include "ICheckFinishPage.h"
#include "MainMenuButton.h"

#include "MainApp.h"

namespace UI
{
namespace Forms
{
namespace ItemFormPage
{


ICheckFinishPage::ICheckFinishPage(wxWindow* parent) : BaseInstallPage(parent)
{
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ICheckFinishPage::onButtonPressed, this);

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableRow( 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labInfo = new wxStaticText( this, wxID_ANY, Managers::GetString(L"#IF_CHECK_FOUND"), wxDefaultPosition, wxDefaultSize, 0 );

	m_butCIP	= new gcButton( this, wxID_ANY, Managers::GetString(L"#IF_CHECK_CIP"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butTryAgain = new gcButton( this, wxID_ANY, Managers::GetString(L"#TRYAGAIN"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butLaunch = new gcButton( this, wxID_ANY, Managers::GetString(L"#LAUNCH"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butClose = new gcButton( this, wxID_ANY, Managers::GetString(L"#CLOSE"), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer2->Add( m_butCIP, 0, wxLEFT|wxTOP|wxBOTTOM, 5 );
	bSizer2->Add( m_butTryAgain, 0, wxLEFT|wxTOP|wxBOTTOM, 5 );
	bSizer2->Add( m_butLaunch, 0, wxLEFT|wxTOP|wxBOTTOM, 5 );
	bSizer2->Add( m_butClose, 0, wxALL, 5 );

	fgSizer3->Add( m_labInfo, 0, wxALL|wxEXPAND, 5 );	
	fgSizer3->Add( bSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer3 );
	this->Layout();

	setParentSize(-1, 165);
}

ICheckFinishPage::~ICheckFinishPage()
{
}

void ICheckFinishPage::init()
{
	UserCore::Item::ItemInfoI *item = getItemHandle()->getItemInfo();

	if (item)
	{
		gcWString msg;

		if (item->isInstalled())
		{
			m_butTryAgain->Show(false);
			m_butCIP->Show(false);

			msg = gcWString(Managers::GetString(L"#IF_CHECK_FOUND"), item->getName());
		}
		else
		{
			m_butLaunch->Show(false);
			msg = gcWString(Managers::GetString(L"#IF_CHECK_NOTFOUND"), item->getName());
		}

		m_labInfo->SetLabel(msg);
		m_labInfo->Wrap(350);
		Layout();
	}
	else
	{
		gcException e(ERR_NULLHANDLE, "Null item for install check.");
		gcErrorBox(this, "#IF_CHECK_ERRTITLE", "#IF_CHECK_ERROR", e);
		GetParent()->Close();
	}
}

void ICheckFinishPage::onButtonPressed(wxCommandEvent& event)
{
	if (event.GetId() == m_butClose->GetId())
	{
		GetParent()->Close();
	}
	else if (event.GetId() == m_butLaunch->GetId())
	{
		g_pMainApp->handleInternalLink(getItemId(), ACTION_LAUNCH);
		GetParent()->Close();
	}
	else if (event.GetId() == m_butCIP->GetId())
	{
		wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, DESURA_wxCustomInstallPath);
		g_pMainApp->getMainWindow()->GetEventHandler()->AddPendingEvent(e);
	}
	else if (event.GetId() == m_butTryAgain->GetId())
	{
		GetParent()->Close();
		g_pMainApp->handleInternalLink(getItemInfo()->getId(), ACTION_INSCHECK);
	}
}

}
}
}
