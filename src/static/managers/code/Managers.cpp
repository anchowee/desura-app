/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Common.h"
#include "managers/Managers.h"

#include "ThemeManager.h"
#include "LanguageManager.h"
#include "WindowManager.h"

#include "CVarManager.h"
#include "ConCommandManager.h"

inline const char* LANG_DEF(void)
{
	std::wstring path = UTIL::OS::getDataPath(L"language/english.xml");
	
	return UTIL::STRING::toStr(path).c_str();
}

namespace Managers
{


const char* GetString( const char* str )
{
	if (GetLanguageManager())
		return GetLanguageManager()->getString( str );
	
	return str;
}

const wchar_t* GetString( const wchar_t* str )
{
	if (GetLanguageManager())
		return GetLanguageManager()->getString( str );
	
	return str;
}

}

/**
 * a simple thread-safe static storage SingletonHolder implementation
 */
template <typename Class>
class SingletonHolder
{
public:
	static Class &Instance();

private:
	class InstanceHolder
	{
	public:
		bool operator !();
		Class & operator *();
		Class & operator =(Class* ref);
		InstanceHolder();
		~InstanceHolder();
		MutexType & mutex();

	private:
		Class *_instance;
		MutexType _mutex;
	};

	static InstanceHolder instance;

	static void NewInstance();

	// should be never ever called
	SingletonHolder();
};

template <typename Class>
inline Class &SingletonHolder<Class>::Instance()
{
	// create instance only if instance is not created
	if (!instance)
		NewInstance();

	return *instance;
}

template <typename Class>
inline void SingletonHolder<Class>::NewInstance()
{
	// enter critical section
	EnterCriticalSection(&instance.mutex());

	// check again for creation (another thread could accessed the critical section before
	if (!instance)
		instance = new Class();

	LeaveCriticalSection(&instance.mutex());
}

template <typename Class>
typename SingletonHolder<Class>::InstanceHolder SingletonHolder<Class>::instance;

template <typename Class>
inline bool SingletonHolder<Class>::InstanceHolder::operator !()
{
	return this->_instance == nullptr;
}

template <typename Class>
inline Class & SingletonHolder<Class>::InstanceHolder::operator *()
{
	return *this->_instance;
}

template <typename Class>
inline Class & SingletonHolder<Class>::InstanceHolder::operator =(Class* ref)
{
	this->_instance = ref;
}

template <typename Class>
SingletonHolder<Class>::InstanceHolder::InstanceHolder()
:	_instance(nullptr){}

template <typename Class>
SingletonHolder<Class>::InstanceHolder::~InstanceHolder()
{
	EnterCriticalSection(&mutex());
	if (this->_instance != nullptr)
	{
		delete this->_instance;
		this->_instance = nullptr;
	}
	LeaveCriticalSection(&mutex());
}

template <typename Class>
MutexType & SingletonHolder<Class>::InstanceHolder::mutex()
{
	return this->_mutex;
}

class ManagersImpl
{
public:
	ManagersImpl()
	{
		m_pLanguageManager = NULL;
		m_pThemeManager = NULL;
		m_pWindowManager = NULL;
	}

	~ManagersImpl()
	{
		//destroy theme here as some image handles may still exist after main form is destroyed
		safe_delete(m_pThemeManager);
	}

	void DestroyManagers()
	{
		safe_delete(m_pLanguageManager);
		safe_delete(m_pWindowManager);

		DestroyCVarManager();
		DestroyConComManager();
	}

	void InitManagers()
	{
		//must load lang first as the cvar gc_lang needs it
		m_pLanguageManager = new LanguageManager(LANG_DEF());
		
		InitCVarManger();
		InitConComManger();

		m_pThemeManager = new ThemeManager();
		m_pWindowManager = new WindowManager();
	}

	LanguageManager	*m_pLanguageManager;
	ThemeManager	*m_pThemeManager;
	WindowManager	*m_pWindowManager;
};

ManagersImpl g_Managers;

void InitManagers()
{
	g_Managers.InitManagers();
}

void DestroyManagers()
{
	g_Managers.DestroyManagers();
}


LanguageManagerI* GetLanguageManager()
{
	return g_Managers.m_pLanguageManager;
}

ThemeManagerI* GetThemeManager()
{
	return g_Managers.m_pThemeManager;
}

WindowManagerI* GetWindowManager()
{
	return g_Managers.m_pWindowManager;
}

CVarManagerI* GetCVarManager()
{
	return g_pCVarMang;
}

CCommandManagerI* GetCCommandManager()
{
	return g_pConComMang;
}







