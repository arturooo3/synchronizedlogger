
class SynchronizedLogger {
	HANDLE m_Thread;
	DWORD m_ThreadID;
public:
	static HANDLE m_Mutex;
	static std::vector<std::string> m_Buffer;

	static DWORD WINAPI Worker( LPVOID ) {
		DWORD res;
		while(true) {
			res = WaitForSingleObject( m_Mutex, INFINITE );
			if(res == WAIT_OBJECT_0) {
				for(unsigned int i=0;i<m_Buffer.size();i++) {
					::printf(m_Buffer[i].c_str());
				}
				m_Buffer.clear();
			}
			ReleaseMutex(m_Mutex);
			Sleep(10);
		}
		return 0;
	}

	SynchronizedLogger() {
		m_Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) Worker, NULL, 0, &m_ThreadID);
		if( m_Thread  == NULL)
		{
			printf("CreateThread error: %d\n", GetLastError());
			return;
		}

		m_Mutex = CreateMutex(NULL, FALSE, NULL);
		if(m_Mutex == NULL)
		{
			printf("CreateMutex error: %d\n", GetLastError());
			return;
		}

	}

	~SynchronizedLogger() {
		CloseHandle(m_Thread);
		CloseHandle(m_Mutex);
	}

	void put(const char* buffer) {
		DWORD res;
		res = WaitForSingleObject( m_Mutex, INFINITE );
		if(res == WAIT_OBJECT_0) {
			m_Buffer.push_back(buffer);
		}
		ReleaseMutex(m_Mutex);
	}

};
HANDLE SynchronizedLogger::m_Mutex;
std::vector<std::string> SynchronizedLogger::m_Buffer;
SynchronizedLogger* g_Logger;

const int g_BufferSize = 500;
#define printf(format, ...) \
do { \
	if(g_Logger==NULL) { \
		::printf(format, ##__VA_ARGS__); \
	} else { \
		char buff[g_BufferSize]; \
		sprintf_s(buff, g_BufferSize, format, ##__VA_ARGS__ ); \
		g_Logger->put(buff); \
	} \
} while(0)
