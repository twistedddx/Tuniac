#pragma once

#define FILEASSOC_TYPE_OPEN		0x00000001
#define FILEASSOC_TYPE_PLAY		0x00000002
#define FILEASSOC_TYPE_QUEUE	0x00000004

typedef struct {
	LPTSTR	szExt;
	LPTSTR	szDesc;
	bool	bAssociated;
} FileAssocDetail, *PFileAssocDetail;


class CFileAssoc
{
protected:
	Array<FileAssocDetail, 3>	m_ExtList;

	bool				IsAssocInReg(unsigned long ulIndex);

public:
	CFileAssoc(void);
	~CFileAssoc(void);

	bool				CleanAssociations();

	bool				IsFoldersAssociated(void);
	bool				AssociateFolders(int iTypes);

	bool				UpdateExtensionList(void);
	PFileAssocDetail	GetExtensionDetail(unsigned long iIndex);
	int					GetExtensionCount(void);

	bool				AssociateExtension(unsigned long ulIndex);

	int					GetDefaultType(void);
	bool				SetDefaultType(int iType);

	bool				ReAssociate(int iTypes, bool bAssocFolders);
};
