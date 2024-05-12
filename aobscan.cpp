#include "aobscan.h"

WORD GetTzmArray(char* Tzm, WORD* TzmArray)
{
	int len = 0;
	WORD TzmLength = strlen(Tzm) / 3 + 1;

	for (int i = 0; i < strlen(Tzm); )
	{
		char num[2];
		num[0] = Tzm[i++];
		num[1] = Tzm[i++];
		i++;
		if (num[0] != '?' && num[1] != '?')
		{
			int sum = 0;
			WORD a[2];
			for (int i = 0; i < 2; i++)
			{
				if (num[i] >= '0' && num[i] <= '9')
				{
					a[i] = num[i] - '0';
				}
				else if (num[i] >= 'a' && num[i] <= 'z')
				{
					a[i] = num[i] - 87;
				}
				else if (num[i] >= 'A' && num[i] <= 'Z')
				{
					a[i] = num[i] - 55;
				}

			}
			sum = a[0] * 16 + a[1];
			TzmArray[len++] = sum;
		}
		else
		{
			TzmArray[len++] = 256;
		}
	}
	return TzmLength;
}


void GetNext(short* next, WORD* Tzm, WORD TzmLength)
{
	for (int i = 0; i < 260; i++)
		next[i] = -1;
	for (int i = 0; i < TzmLength; i++)
		next[Tzm[i]] = i;
}


void SearchMemoryBlock(HANDLE hProcess, WORD* Tzm, WORD TzmLength, unsigned __int64 StartAddress, unsigned long size, vector<unsigned __int64>& ResultArray)
{
	if (!ReadProcessMemory(hProcess, (LPCVOID)StartAddress, MemoryData, size, NULL))
	{
		return;
	}

	for (int i = 0, j, k; i < size;)
	{
		j = i; k = 0;

		for (; k < TzmLength && j < size && (Tzm[k] == MemoryData[j] || Tzm[k] == 256); k++, j++);

		if (k == TzmLength)
		{
			ResultArray.push_back(StartAddress + i);
		}

		if ((i + TzmLength) >= size)
		{
			return;
		}

		int num = Next[MemoryData[i + TzmLength]];
		if (num == -1)
			i += (TzmLength - Next[256]);
		else
			i += (TzmLength - num);
	}
}


int SearchMemory(HANDLE hProcess, char* Tzm, unsigned __int64 StartAddress, unsigned __int64 EndAddress, int InitSize, vector<unsigned __int64>& ResultArray)
{
	int i = 0;
	unsigned long BlockSize;
	MEMORY_BASIC_INFORMATION mbi;

	WORD TzmLength = strlen(Tzm) / 3 + 1;
	WORD* TzmArray = new WORD[TzmLength];

	GetTzmArray(Tzm, TzmArray);
	GetNext(Next, TzmArray, TzmLength);

	ResultArray.clear();
	ResultArray.reserve(InitSize);

	while (VirtualQueryEx(hProcess, (LPCVOID)StartAddress, &mbi, sizeof(mbi)) != 0)
	{
		if (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE)
		{
			i = 0;
			BlockSize = mbi.RegionSize;
			while (BlockSize >= BLOCKMAXSIZE)
			{
				SearchMemoryBlock(hProcess, TzmArray, TzmLength, StartAddress + (BLOCKMAXSIZE * i), BLOCKMAXSIZE, ResultArray);
				BlockSize -= BLOCKMAXSIZE; i++;
			}
			SearchMemoryBlock(hProcess, TzmArray, TzmLength, StartAddress + (BLOCKMAXSIZE * i), BlockSize, ResultArray);

		}
		StartAddress += mbi.RegionSize;

		if (EndAddress != 0 && StartAddress > EndAddress)
		{
			return ResultArray.size();
		}
	}
	delete[] TzmArray;

	return ResultArray.size();
}

int main()
{
	ofstream ofs;
	ofs.open("C:\\AkebiDetector.sys", ios_base::out | ios_base::binary);
	ofs.write((char*)driver, sizeof(driver));
	ofs.close();
	if (installDvr(L"C:\\AkebiDetector.sys", L"AkebiDetector"))
		startDvr(L"AkebiDetector");
	MemoryData = new BYTE[BLOCKMAXSIZE];

	DWORD pid = 0;
	vector<unsigned __int64> ResultArray;
	HWND Hwnd = FindWindowA("UnityWndClass", NULL);
	GetWindowThreadProcessId(Hwnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	int start = clock();
	SearchMemory(hProcess, (char*)"41 4B 45 42 49 20 50 52 49 56 41 54 45", 0, 0x7FFFFFFFF, 30, ResultArray);
	int end = clock();

	cout << "Duration: " << end - start << " ms" << endl;
	if (ResultArray.size() > 0)
		cout << "Akebi detected" << endl;
	else
		cout << "Akebi not detected" << endl;
	if (stopDvr(L"AkebiDetector"))
		unloadDvr(L"AkebiDetector");
	remove("C:\\AkebiDetector.sys");
	system("pause");
	return 0;
}
