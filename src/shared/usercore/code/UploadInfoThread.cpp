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
#include "UploadInfoThread.h"
#include "UploadThread.h"

namespace UserCore
{
namespace Thread
{

UploadInfoThread::UploadInfoThread(DesuraId id, const char* key, const char* path, uint32 start) : BaseItem(key)
{
	m_bDeleted = false;
	m_bDelMcf = true;
	m_bStarted = false;
	m_bComplete = false;
	m_bErrored = false;

	m_uiProgress = 0;
	m_pThread = nullptr;

	m_pWebCore = nullptr;
	m_pUser = nullptr;
	m_pUploadManager = nullptr;

	m_pUploadThreadInfo = new UploadThreadInfo(id, path, key, start);

	onCompleteEvent += delegate(this, &UploadInfoThread::onComplete);
	onUploadProgressEvent += delegate(this, &UploadInfoThread::onProgress);
	onErrorEvent += delegate(this, &UploadInfoThread::onError);
}

UploadInfoThread::~UploadInfoThread()
{
	deleteThread();
	safe_delete(m_pUploadThreadInfo);
}

void UploadInfoThread::setDelMcf(bool state)
{
	onActionEvent();
	m_bDelMcf = state;
}

void UploadInfoThread::onComplete(uint32& status)
{
	m_bComplete = true;

	//remove the file once we are done uploading to keep every thing neat
	if (status == 999 && getFile() && m_bDelMcf)
	{
		UTIL::FS::delFile(UTIL::FS::PathWithFile(getFile()));
	}
	onActionEvent();
}

void UploadInfoThread::onPause()
{
	onActionEvent();
}

void UploadInfoThread::onUnpause()
{
	onActionEvent();
}

void UploadInfoThread::deleteThread()
{
	if (!m_pThread)
		return;

	m_pThread->onPauseEvent -= delegate(this, &UploadInfoThread::onPause);
	m_pThread->onUnpauseEvent -= delegate(this, &UploadInfoThread::onUnpause);

	m_pThread->onCompleteEvent -= delegate(&onCompleteEvent);
	m_pThread->onUploadProgressEvent -= delegate(&onUploadProgressEvent);
	m_pThread->onErrorEvent -= delegate(&onErrorEvent);

	m_pThread->stop();
	m_pThread->join();

	safe_delete(m_pThread);
	m_bStarted = false;
}

void UploadInfoThread::start()
{
	deleteThread();
	m_bStarted = true;

	m_pThread = new UploadThread(m_pUploadThreadInfo);

	m_pThread->onPauseEvent += delegate(this, &UploadInfoThread::onPause);
	m_pThread->onUnpauseEvent += delegate(this, &UploadInfoThread::onUnpause);

	m_pThread->onCompleteEvent += delegate(&onCompleteEvent);
	m_pThread->onUploadProgressEvent += delegate(&onUploadProgressEvent);
	m_pThread->onErrorEvent += delegate(&onErrorEvent);

	m_pThread->setWebCore(m_pWebCore);
	m_pThread->setUserCore(m_pUser);
	m_pThread->setUpLoadManager(m_pUploadManager);

	m_pThread->start();
}

void UploadInfoThread::onProgress(UserCore::Misc::UploadInfo& info)
{
	if (info.paused)
		onActionEvent();

	m_uiProgress = info.percent;
}

void UploadInfoThread::onError(gcException &e)
{
	m_bErrored = true;
}



DesuraId UploadInfoThread::getItemId()
{
	return m_pUploadThreadInfo->itemId;
}
	
//Overrides
void UploadInfoThread::setStart(uint32 start) 
{
	m_pUploadThreadInfo->uiStart = start; 
}

Event<uint32>* UploadInfoThread::getCompleteEvent() 
{
	return &onCompleteEvent;
}

Event<gcException>* UploadInfoThread::getErrorEvent()
{
	return &onErrorEvent; 
}

Event<UserCore::Misc::UploadInfo>* UploadInfoThread::getUploadProgressEvent()
{
	return &onUploadProgressEvent;
}

//BaseThread
void UploadInfoThread::stop()
{
	if (m_pThread)
		m_pThread->stop();
}

void UploadInfoThread::nonBlockStop()
{
	if (m_pThread)
		m_pThread->nonBlockStop(); 
}

void UploadInfoThread::unpause()
{
	if (m_pThread)
		m_pThread->unpause(); 
}

void UploadInfoThread::pause()
{
	if (m_pThread)
		m_pThread->pause(); 
}

bool UploadInfoThread::isPaused()
{
	if (m_pThread)
		return m_pThread->isPaused(); 

	return false;
}
	
const char* UploadInfoThread::getFile()
{
	return m_pUploadThreadInfo->szFile.c_str(); 
}

const char* UploadInfoThread::getKey()
{
	return m_pUploadThreadInfo->szKey.c_str();
}




void UploadInfoThread::setWebCore(WebCore::WebCoreI *wc)
{
	m_pWebCore = wc;
}

void UploadInfoThread::setUserCore(UserCore::UserI *uc)
{
	m_pUser = uc;
}

void UploadInfoThread::setUpLoadManager(UserCore::UploadManagerI *um)
{
	m_pUploadManager = um;
}









}
}