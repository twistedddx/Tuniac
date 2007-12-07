#pragma once

class CTagEditor
{
public:
	CTagEditor(void);
	~CTagEditor(void);

	bool ShowEditor(HWND aParentWnd);

	bool AddListToEdit(EntryArray *aPlaylistEdit) {m_EditPlaylist = aPlaylistEdit;} //

protected:
	
	static LRESULT CALLBACK EditorProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	EntryArray* m_EditPlaylist;

	bool UpdateControls(); // adds the data to respective controls
	bool SaveData(); //saves the data to the entries in the array
    	
};
