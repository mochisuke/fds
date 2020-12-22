﻿// ◇
// fds: fdx68 selector
// FDSConfig: コンフィグ設定
// Copyright: (C)2020 Hiroaki GOTO as GORRY.
// License: see readme.txt
// =====================================================================

#include "FDSConfig.h"

// =====================================================================
// FDS コンフィグ設定
// =====================================================================

// -------------------------------------------------------------
// INIファイル読み込み
// -------------------------------------------------------------
int
FDSConfig::load(const std::string& filename)
{
	int num;

	// コンフィグINIファイルを読む
	FDS_LOG("load %s\n", filename.c_str());
	int ret = mIniFile.load(filename);
	if (ret < 0) {
		FDS_ERROR("failed: load %s\n", filename.c_str());
		fprintf(stderr, "[%s] not found. Please copy [fds2_sample.ini] to [%s].\n", filename.c_str(), filename.c_str());
		return ret;
	}

	// SYSTEMセクション読み込み
	mCmdDir = mIniFile.getString("SYSTEM", "CMDDIR");
	mFddEmuCmd = mCmdDir + "/" + mIniFile.getString("SYSTEM", "FDDEMUCMD");
	mFdxToolCmd = mCmdDir + "/" + mIniFile.getString("SYSTEM", "FDXTOOLCMD");
	mFdDumpCmd = mCmdDir + "/" + mIniFile.getString("SYSTEM", "FDDUMPCMD");
	mFdRestoreCmd = mCmdDir + "/" + mIniFile.getString("SYSTEM", "FDRESTORECMD");

	// [DRIVES]セクション読み込み
	for (num=1; ; num++) {
		char key[16];
		sprintf(key, "DRIVE-%d", num);
		if (!mIniFile.hasKey("DRIVES", key)) {
			break;
		}
	}
	if (num < 2) {
		FDS_ERROR("failed: no DRIVE entry in [DRIVES] section in file %s\n", filename.c_str());
		fprintf(stderr, "No DRIVE entry in [DRIVES] section in file %s\n", filename.c_str());
		return -1;
	}
	mVecDrives.resize(num-1);
	mDrivesDir = mIniFile.getString("DRIVES", "DIR");
	for (int i=1; i<num; i++) {
		char key[16];
		sprintf(key, "DRIVE-%d", i);
		std::string vfilename = mIniFile.getString("DRIVES", key);
		std::string path = mDrivesDir + "/" + vfilename;
		FDSDrive& drive = mVecDrives[i-1];
		ret = drive.load(path);
		if (ret < 0) {
			return ret;
		}
	}

	// [MACHINES]セクション読み込み
	for (num=1; ; num++) {
		char key[16];
		sprintf(key, "MACHINE-%d", num);
		if (!mIniFile.hasKey("MACHINES", key)) {
			break;
		}
	}
	if (num < 2) {
		FDS_ERROR("failed: no MACHINE entry in [MACHINES] section in file %s", filename.c_str());
		fprintf(stderr, "No MACHINE entry in [MACHINES] section in file %s", filename.c_str());
		return -1;
	}
	mVecMachines.resize(num-1);
	mMachinesDir = mIniFile.getString("MACHINES", "DIR");
	for (int i=1; i<num; i++) {
		char key[16];
		sprintf(key, "MACHINE-%d", i);
		std::string vfilename = mIniFile.getString("MACHINES", key);
		std::string path = mMachinesDir + "/" + vfilename;
		FDSMachine& machine = mVecMachines[i-1];
		ret = machine.load(path);
		if (ret < 0) {
			return ret;
		}
	}

	return 0;
}

// -------------------------------------------------------------
// ダンプオプション文字列作成
// -------------------------------------------------------------
std::string
FDSConfig::makeDumpOpt(int machineno, int driveno, int dumpno) const
{
	char buf[FDX_STRING_MAX];

	const std::string& type = cfgMachine(machineno).dump(dumpno).type();
	int no = cfgDrive(driveno).findDumpNoByType(type);
	if (no < 0) {
		std::string str;
		return str;
	}

	sprintf(buf, "-i%d -r%d %s -f %s",
	  cfgDrive(driveno).id(),
	  cfgDrive(driveno).retry(),
	  cfgDrive(driveno).dump(no).fdDumpOpt().c_str(),
	  cfgMachine(machineno).dump(dumpno).format().c_str()
	);
	std::string str(buf);
	return str;
}


// -------------------------------------------------------------
// リストアオプション文字列作成
// -------------------------------------------------------------
std::string
FDSConfig::makeRestoreOpt(int machineno, int driveno) const
{
	char buf[FDX_STRING_MAX];

	const std::string& type = cfgMachine(machineno).restore().type();
	int no = cfgDrive(driveno).findRestoreNoByType(type);
	if (no < 0) {
		std::string str;
		return str;
	}

	sprintf(buf, "-i%d %s",
	  cfgDrive(driveno).id(),
	  cfgDrive(driveno).restore(no).fdRestoreOpt().c_str()
	);
	std::string str(buf);
	return str;
}


// [EOF]
