#pragma once

#include <iostream>
#include <fstream>
#include <windows.h>
#include <cstring>

#include <unistd.h>
#include <dirent.h>
#include <stdexcept>
#include <sys/types.h>
#include <shlobj.h>

using namespace std;

const bool creat_dir(const string pDir)
{
	string pszDir = pDir;
	if (access(pszDir.c_str(), 0) == -1)//Find Dir Failed
	{
		return false;
	}
	for (size_t i = 0; i != pDir.length(); i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\0';
			if (access(pszDir.c_str(), 0) != 0)
			{
				if (mkdir(pszDir.c_str()) != 0)
				{
					return false;
				}
			}
			pszDir[i] = '/';
		}
	}
	if (mkdir(pszDir.c_str()))
		return true;
	else
		return false;
}

bool file_empty(const string path)
{
	ofstream fout(path.c_str(), ios::out);
	if (fout.good())
		return true;
	else if (fout.fail())
		throw logic_error("Logical error on i/o operation");
	else if (fout.bad())
		throw runtime_error("Read/writing error on i/o operation");
	else
		return false;//Already made it EOF
	fout << EOF;
	fout.close();
}

const string GetLocalAppDataPath()
{
	char lpszDefaultDir[MAX_PATH];
	memset(lpszDefaultDir, 0, MAX_PATH);
	char szDocument[_MAX_PATH] = { 0 };

	LPITEMIDLIST pidl = NULL;

	SHGetSpecialFolderLocation(NULL, CSIDL_LOCAL_APPDATA, &pidl);
	if (pidl and SHGetPathFromIDList(pidl, szDocument))
	{
		GetShortPathName(szDocument, lpszDefaultDir, _MAX_PATH);
	}
	string sr(lpszDefaultDir);
	return sr;
}

const bool block(const string path, const string name)
{
	const string all_path = path + name;
	const string backups_path = GetLocalAppDataPath() + "Blocker/Backups/";

	system(((string) "taskkill -f -im " + name + (string) " -t").c_str());
	creat_dir(backups_path.c_str());
	CopyFile((LPCSTR)all_path.c_str(), (LPCSTR)backups_path.c_str(), FALSE);
	SetFileAttributes(all_path.c_str(), FILE_ATTRIBUTE_NORMAL);
	file_empty(all_path);
	if (not SetFileAttributes(all_path.c_str(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN))
		throw runtime_error("Error When Making File Read-only");
	return true;
}


void inline ad_blocks(const string path, const string name, ostream &os, ostream &er)
{
	try
	{
		block(path, name);
	}
	catch (runtime_error rex)
	{
		er << "Block " << "\"" << path + name << "\"" << " failed: " << rex.what() << "!" << endl;
		return;
	}
	catch (logic_error lex)
	{
		er << "Block " << "\"" << path + name << "\"" << " failed: " << lex.what() << "!" << endl;
		return;
	}

	os << "Block " << path + name << "succesed";
}
