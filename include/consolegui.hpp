#ifndef MENU_HPP
#define MENU_HPP
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sys/time.h>
using namespace std;
// һЩ�����ĺ���
long getTimeDifference(const struct timeval &start,  const struct timeval &end) {
	return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
}

// �����̨�йص�����
// ���������
void hidecursor() {
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);
	CursorInfo.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);
}
// ��ʾ�����
void showcursor() {
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);
	CursorInfo.bVisible = true;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);
}
// �Ƴ�����̨����
void removeAttributes() {
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;	//�Ƴ����ٱ༭ģʽ
	mode &= ~ENABLE_INSERT_MODE;		//�Ƴ�����ģʽ
	mode |= ENABLE_MOUSE_INPUT;			//����������
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode);
}
// ��ӿ���̨����
void addAttributes() {
	DWORD mode;
	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	mode |= ENABLE_QUICK_EDIT_MODE;		//��ӿ��ٱ༭ģʽ
	mode |= ENABLE_INSERT_MODE;     	//��Ӳ���ģʽ
	mode |= ENABLE_MOUSE_INPUT;			//����������
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode);
}

// ������йصĺ���
// ��ȡ���λ��
COORD getcursor() {
	CONSOLE_SCREEN_BUFFER_INFO temp;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &temp);
	return temp.dwCursorPosition;
}
// �������λ��
void setcursor(const COORD &temp = {0, 0}) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), temp);
}
// �ȴ�����¼����¼���������ƶ�����������ֹ�����һ�������������¼�
MOUSE_EVENT_RECORD waitMouseEvent(bool move = true) {
	INPUT_RECORD record; //�����¼�
	DWORD reg;           //��ʱ�Ĵ�
	while (true) {
		Sleep(10);
		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &record, 1, &reg);                                  //�������¼�����record
		if (record.EventType == MOUSE_EVENT and (move | (record.Event.MouseEvent.dwEventFlags != MOUSE_MOVED))) //������¼� && �ƶ��¼���ģʽ��Ӧ
			return record.Event.MouseEvent;
	}
}
// ���Դ��룬�������COORD��������ӡ���λ��
ostream &operator<<(ostream &pout, const COORD &temp) {
	pout.setf(ios_base::fixed);
	pout << "[Cursor Position] X: " << std::setw(2) << temp.X << "  Y: " << std::setw(2) << temp.Y;
	return pout;
}

// �����̨�ı���ɫ�йص���
// һЩ������ɫ��ϵĶ���
#define BlackWhite 0x07
#define BlackCyan 0x0b
#define YellowBlue 0xe9
struct Color {
	short Highlight;	// �ı�������ɫ���
	short Default;		// Ĭ����ɫ���
	short lastcolor;	// ����ʵ���л���ɫ��ϣ����ڸ�������

	// ��ʼ��
	Color() : Highlight(BlackCyan), Default(BlackWhite), lastcolor(BlackWhite) {}
	Color(short Highlight = BlackCyan, short Default = BlackWhite) : Highlight(Highlight), Default(Default), lastcolor(BlackWhite) {}

	// ���ı���ɫ����ΪDefault���
	void setDefault() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Default);
		lastcolor = Default;
	}
	// ���ı���ɫ����ΪHighlight���
	void setHighlight() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Highlight);
		lastcolor = Highlight;
	}
};

// Menu���ǰ��������Parameter����ʹ��
class Menu;
// ����Parameter��Ա����״̬
// buttonState����ֵ�Ķ���
#define mouseLeftButton FROM_LEFT_1ST_BUTTON_PRESSED	// ���
#define mouseMiddleButton FROM_LEFT_2ND_BUTTON_PRESSED	// �м�
#define mouseRightButton RIGHTMOST_BUTTON_PRESSED		// �Ҽ�
// eventFlag����ֵ�Ķ���
#define mouseClick 0x0					// ��갴�����»��ɿ�ʱ����
#define mouseMove MOUSE_MOVED			// ����ƶ�ʱ����
#define mouseDoubleClick DOUBLE_CLICK	// ���ڶ��ΰ���ʱ�������������¼�ǰһ���ᴥ�� mouseClick �¼�
#define mouseWheel MOUSE_WHEELED		// �����ֹ���ʱ����
// *****Parameter�������Ϊ�ص�������������*****
struct Parameter {
	DWORD buttonState;		// ��갴����¼
	DWORD controlkeyState;	// ���ư���״̬
	DWORD eventFlag;		// ����¼���¼

	// ��ʼ��
	Parameter() : buttonState(mouseLeftButton), controlkeyState(0), eventFlag(0) {}
	Parameter(MOUSE_EVENT_RECORD mouseEvent) : buttonState(mouseEvent.dwButtonState), controlkeyState(mouseEvent.dwControlKeyState), eventFlag(mouseEvent.dwEventFlags) {}
};

// �˵����ı��࣬ÿ��Text�ͱ���������˵��е�һ��
struct Text {
	string text;		// �ı�����
	Color color;		// ��ɫ����
	COORD position;		// �ı�λ��

	// ��ʼ��
	Text() : text(""), color(Color(0, 0)), position({0, 0}) {}
	Text(string text, Color color) : text(text), color(color), position({0, 0}) {}

	// �ж���������Ƿ����ı�λ���غ�
	bool operator== (const COORD &mousePosition) const {
		if (position.Y == mousePosition.Y and position.X <= mousePosition.X and mousePosition.X <= position.X + (short)text.length() - 1) {
			return true;
		}
		return false;
	}
	// �ж���������Ƿ����ı�λ���غ�
	bool operator!= (const COORD &mousePosition) const {
		return !operator==(mousePosition);
	}
};
COORD operator+ (const COORD x, const int y) {
	return {x.X, (short)(x.Y + y)};
}

// *****�˵���*****
class Menu {
	private:
		struct timeval timeStart, timeNow;
	protected:
		// ���
		void write(string text, bool isEndl = false) {
			printf("%s", text.c_str());
			if (isEndl) {
				printf("\n");
			}
		}
		// ���и������
		void rewrite(Text data) {
			COORD lastposition = getcursor();
			setcursor({0, data.position.Y});
			// ��վ�����
			for (short j = 0; j < data.position.X; ++j) {
				write(" ");
			}
			setcursor({0, data.position.Y});
			// ���������
			write(data.text);
			setcursor(lastposition);
		}
		// ����˵�������linedata��position��Ա������ʼ��
		void initializePosition() {
			for (size_t i = 1; i < linedata.size(); ++i) {
				linedata[i].position = getcursor();
				linedata[i].color.setDefault();
				write(linedata[i].text, true);
			}
		}
		// ˢ�²˵�
		void refresh(COORD hangPosition) {
			for (size_t i = 1; i < linedata.size(); ++i) {
				// ������λ��%data%��ť�� ���� %data%��ť�����ڸ���
				if (linedata[i] == hangPosition and linedata[i].color.lastcolor != linedata[i].color.Highlight) {
					linedata[i].color.setHighlight();	// ����Ϊ������ɫ
					rewrite(linedata[i]);
				}
				// �����겻λ��%data%��ť�� ���� %data%��ť���ڸ���
				if (linedata[i] != hangPosition and linedata[i].color.lastcolor != linedata[i].color.Default) {
					linedata[i].color.setDefault();	// ����ΪĬ����ɫ
					rewrite(linedata[i]);
				}
			}
		}
	public:
		long timeLimit;					// �˵���ʾʱ������, ��λms
		vector<Text> linedata = {Text()};	// ��Ų˵�ÿ�е���Ϣ
		string keyState;

		// ���캯��
		Menu() : timeLimit(-1), keyState("") {}
		Menu(long timeLimit) : timeLimit(timeLimit), keyState("") {}
		Menu(string keyState) : timeLimit(-1), keyState(keyState) {}
		~Menu() {}

		bool loadfromConfig(const vector<string> &config) {
			linedata = {Text()};
			for (size_t i = 0; i < config.size(); ++i) {
				if (config[i] == keyState + "{") {
					string text, color;
					size_t pos;
					while (config[i + 1] != "}") {
						++i;
						pos = config[i].find("\":\"");
						if (pos == string::npos) continue;
						if (config[i][0] != '\"' or config[i][config[i].length() - 1] != '\"') continue;
						text = config[i].substr(1, pos - 1);
						color = config[i].substr(pos + 3, config[i].size() - pos - 4);
						linedata.push_back(Text(text, stoi(color)));
					}
				}
			}
			return false;
		}
		// ��˵�β�����һ��
		Menu &push_back(string text = "", short colorHighlight = BlackCyan, short colorDefault = BlackWhite) {
			linedata.push_back(Text(text, Color(colorHighlight, colorDefault)));
			return *this;
		}
		// �Ӳ˵�β������һ��
		Menu &pop_back() {
			linedata.pop_back();
			return *this;
		}
		// ��ղ˵�
		Menu &clear() {
			linedata.clear();
			return *this;
		}
		// ���ò˵���ʾʱ��
		void set_timeLimit(long _timeLimit) {
			timeLimit = _timeLimit;
		}
		void clearScreen() {
			if (linedata.size() == 1) return;
			for (size_t i = 1; i < linedata.size(); ++i) {
				setcursor(linedata[i].position);
				for (size_t j = 0; j < linedata[i].text.length(); ++j) {
					write(" ");
				}
			}
			setcursor(linedata[1].position);
		}
		void MenuStart() {
			if (linedata.size() == 1) return;
			gettimeofday(&timeStart, nullptr);
			removeAttributes();
			hidecursor();
			initializePosition();
		}
		long MenuStop() {
			if (linedata.size() == 1) return 0;
			setcursor(linedata[linedata.size() - 1].position + 1);
			for (size_t i = 1; i < linedata.size(); ++i) {
				if (linedata[i].color.lastcolor == linedata[i].color.Highlight) {
					linedata[i].color.setDefault();
					rewrite(linedata[i]);
				}
			}
			addAttributes();
			showcursor();
			return getTimeDifference(timeStart, timeNow);
			Sleep(100);
		}
		// ��ʾ�˵�, ����ֵ��0������1~n������inf��Ч���
		size_t choose() {
			if (linedata.size() == 1) return 0;
			removeAttributes();
			hidecursor();
			MOUSE_EVENT_RECORD mouseEvent;
			while (timeLimit == -1 or getTimeDifference(timeStart, timeNow) < timeLimit) {
				Sleep(50);
				mouseEvent = waitMouseEvent();
				switch (mouseEvent.dwEventFlags) {
					case mouseMove: {
						refresh(mouseEvent.dwMousePosition);
						break;
					}
					case mouseClick: {
						if (mouseEvent.dwButtonState and mouseEvent.dwButtonState != mouseWheel) {
							setcursor(linedata[linedata.size() - 1].position + 1);
							for (size_t i = 1; i < linedata.size(); ++i) {
								if (linedata[i] == mouseEvent.dwMousePosition) {
									linedata[i].color.setDefault();
									rewrite(linedata[i]);
									addAttributes();
									showcursor();
									return i;
								}
							}
							addAttributes();
							showcursor();
							return -1;
						}
						break;
					}
				}
				gettimeofday(&timeNow, nullptr);
			}
			return 0;
		}
};
void loadfromConfig(vector<string> &data, const vector<string> &config, string keyState) {
	for (size_t i = 0; i < config.size(); ++i) {
		if (config[i] == keyState + "{") {
			string text;
			while (config[i + 1] != "}") {
				++i;
				if (config[i][0] != '\"' or config[i][config[i].length() - 1] != '\"') continue;
				text = config[i].substr(1, config[i].length() - 2);
				data.push_back(text);
			}
		}
	}
}
#endif
