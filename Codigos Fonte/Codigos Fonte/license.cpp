#include "stdafx.h"
#include "license.h"
#include "utils.h"

bool enableKey(int lic, unsigned OSArch) {
	char *guid = guid2char_WIN(OSArch);
	if(guid[0] < 0) {
		if(guid[0] == LICENSE_ERROR_E::INVALID_OSARCH) {
			delete[] guid;
			errorHandler(var2str(enableKey()),"Por favor, especifique uma arquitetura de sistema operacional valida.");
		}
		else if(guid[0] == LICENSE_ERROR_E::NO_ARCHIVE) {
			delete[] guid;
			errorHandler(var2str(enableKey()),"Nao foi possivel acessar informacoes de hardware/software.\n\t\tCaminho foi encontrado, mas arquivo nao.\n\t\tVerificar se a arquitetura de sistema operacional foi especificada corretamente.");
		}
		else if(guid[0] == LICENSE_ERROR_E::PATH_NOT_FOUND) {
			delete[] guid;
			errorHandler(var2str(enableKey()),"Nao foi possivel acessar informacoes de hardware/software.\n\t\tVerificar se a arquitetura de sistema operacional foi especificada corretamente.");
		}
		else if(guid[0] == LICENSE_ERROR_E::UNKNOWN) {
			delete[] guid;
			errorHandler(var2str(enableKey()),"Erro desconhecido.\n\t\tEntrar em contato com o suporte tecnico.");
		}
		else {
			delete[] guid;
			errorHandler(var2str(enableKey()),"Erro inesperado.\n\t\tEntrar em contato com o suporte tecnico.");
		}

		return false;
	}

	unsigned char *reg = new unsigned char[GUID_LENGTH];
	int trueSize = 0;

	for (int x = 0; x < GUID_LENGTH; x++) {
		if(guid[x] >= '0' && guid[x] <= '9') {
			reg[trueSize] = guid[x] - '0';
			trueSize++;
		}
		else if (guid[x] >= 'a' && guid[x] <= 'f') {
			reg[trueSize] = guid[x] - 'a' + 10;
			trueSize++;
		}
		else if (guid[x] >= 'A' && guid[x] <= 'F') {
			reg[trueSize] = guid[x] - 'A' + 10;
			trueSize++;
		}
	}

	int computerLic = transformLic(reg,trueSize);

	delete[] reg;

	if(computerLic != lic) {
		std::string error_info = "Esta licenca nao e valida para este computador. Por favor entre em contato com o suporte tecnico e peca uma nova licenca para este computador.\n"
								 "\t\tSera necessario enviar o codigo abaixo para poder criar uma nova licensa:\n\n\t\t\t";
		error_info.append(guid);
		delete[] guid;
		errorHandler(var2str(enableKey()),error_info);
		return false;
	}

	notificationHandler(var2str(enableKey()),"A licensa do programa foi validada com sucesso.");
	
	delete[] guid;
	return true;
}

int transformLic(unsigned char *reg, int size) {
	int block, lic = 0, blockSize = size/4;
	if (blockSize < 4) {
		errorHandler(var2str(transformLic()),"A informacao de hardware/software e pouco especifica.");
		return -1;
	}

	for(int x = 0; x < blockSize; x++) {
		block = (int)reg[4*x];
		for(int y = 0; y < 4; y++) {
			block += (int)reg[y + 4*x];
		}
		block = block%10;
		lic += block * (int) pow(10,blockSize-x-1);
	}
	return lic;
}

int generateKey(char *guid) {
	unsigned char *reg = new unsigned char[GUID_LENGTH];
	int trueSize = 0;

	for (int x = 0; guid[x] != '\0'; x++) {
		if(guid[x] >= '0' && guid[x] <= '9') {
			reg[trueSize] = guid[x] - '0';
			trueSize++;
		}
		else if (guid[x] >= 'a' && guid[x] <= 'f') {
			reg[trueSize] = guid[x] - 'a' + 10;
			trueSize++;
		}
		else if (guid[x] >= 'A' && guid[x] <= 'F') {
			reg[trueSize] = guid[x] - 'A' + 10;
			trueSize++;
		}
	}

	int computerLic = transformLic(reg,trueSize);

	delete[] reg;

	return computerLic;
}

int generateKey(unsigned OSArch) {
	char *guid = guid2char_WIN(OSArch);
	if(guid[0] < 0) {
		if(guid[0] == LICENSE_ERROR_E::INVALID_OSARCH) {
			delete[] guid;
			errorHandler(var2str(generateKey()),"Por favor, especifique uma arquitetura de sistema operacional valida.");
		}
		else if(guid[0] == LICENSE_ERROR_E::NO_ARCHIVE) {
			delete[] guid;
			errorHandler(var2str(generateKey()),"Nao foi possivel acessar informacoes de hardware/software.\n\t\tCaminho foi encontrado, mas arquivo nao.\n\t\tVerificar se a arquitetura de sistema operacional foi especificada corretamente.");
		}
		else if(guid[0] == LICENSE_ERROR_E::PATH_NOT_FOUND) {
			delete[] guid;
			errorHandler(var2str(generateKey()),"Nao foi possivel acessar informacoes de hardware/software.\n\t\tVerificar se a arquitetura de sistema operacional foi especificada corretamente.");
		}
		else if(guid[0] == LICENSE_ERROR_E::UNKNOWN) {
			delete[] guid;
			errorHandler(var2str(generateKey()),"Erro desconhecido.\n\t\tEntrar em contato com o suporte tecnico.");
		}
		else {
			delete[] guid;
			errorHandler(var2str(generateKey()),"Erro inesperado.\n\t\tEntrar em contato com o suporte tecnico.");
		}

		return -1;
	}

	unsigned char *reg = new unsigned char[GUID_LENGTH];
	int trueSize = 0;

	for (int x = 0; x < GUID_LENGTH; x++) {
		if(guid[x] >= '0' && guid[x] <= '9') {
			reg[trueSize] = guid[x] - '0';
			trueSize++;
		}
		else if (guid[x] >= 'a' && guid[x] <= 'f') {
			reg[trueSize] = guid[x] - 'a' + 10;
			trueSize++;
		}
		else if (guid[x] >= 'A' && guid[x] <= 'F') {
			reg[trueSize] = guid[x] - 'A' + 10;
			trueSize++;
		}
	}

	int computerLic = transformLic(reg,trueSize);

	delete[] reg;

	return computerLic;
}

char* guid2char_WIN(unsigned os) {
	/* Retrieve MachineGuid*/
	HKEY key = 0;
	char *buf = new char[GUID_LENGTH];
	DWORD dwBufSize = GUID_LENGTH*sizeof(char);
	const char* subkey = "Software\\Microsoft\\Cryptography";
	REGSAM desiredSam;

	//OS Architecture must be assigned to use proper flags.
	if(os == OS_E::OS_64BIT)
		desiredSam = KEY_WOW64_64KEY|KEY_READ;
	else if(os == OS_E::OS_32BIT)
		desiredSam = KEY_READ;
	else {
		delete[] buf;
		buf = new char;
		buf[0] = LICENSE_ERROR_E::INVALID_OSARCH;
		RegCloseKey(key);
		return buf;
	}

	//Open the registry folder
	if( RegOpenKeyExA(HKEY_LOCAL_MACHINE,subkey,0,desiredSam,&key) == ERROR_SUCCESS) {
		//Query the registry value
		if(RegQueryValueExA(key,"MachineGuid",0, 0, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS) {
			RegCloseKey(key);
			return buf;
		}
		else {
			delete[] buf;
			buf = new char;
			buf[0] = LICENSE_ERROR_E::NO_ARCHIVE;
			RegCloseKey(key);
			return buf;
		}
	}
	else {
		delete[] buf;
		buf = new char;
		buf[0] = LICENSE_ERROR_E::PATH_NOT_FOUND;
		RegCloseKey(key);
		return buf;
	}

	RegCloseKey(key);
	delete[] buf;
	buf = new char;
	buf[0] = LICENSE_ERROR_E::UNKNOWN;
	return buf;
}