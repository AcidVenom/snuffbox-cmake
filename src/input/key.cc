#include "../input/key.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Key::Keys Key::StringToKey(const char* name)
	{
		if (strcmp(name, "Backspace") == 0)			return Keys::kBackspace;
		if (strcmp(name, "Tab") == 0)						return Keys::kTab;
		if (strcmp(name, "Clear") == 0)					return Keys::kClear;
		if (strcmp(name, "Enter") == 0)					return Keys::kEnter;
		if (strcmp(name, "Shift") == 0)					return Keys::kShift;
		if (strcmp(name, "Control") == 0)				return Keys::kControl;
		if (strcmp(name, "Alt") == 0)						return Keys::kAlt;
		if (strcmp(name, "Pause") == 0)					return Keys::kPause;
		if (strcmp(name, "Caps Lock") == 0)			return Keys::kCapsLock;
		if (strcmp(name, "Escape") == 0)				return Keys::kEscape;
		if (strcmp(name, "Space") == 0)					return Keys::kSpace;
		if (strcmp(name, "Page Up") == 0)				return Keys::kPageUp;
		if (strcmp(name, "Page Down") == 0)			return Keys::kPageUp;
		if (strcmp(name, "End") == 0)						return Keys::kEnd;
		if (strcmp(name, "Home") == 0)					return Keys::kHome;
		if (strcmp(name, "Left") == 0)					return Keys::kLeft;
		if (strcmp(name, "Up") == 0)						return Keys::kUp;
		if (strcmp(name, "Right") == 0)					return Keys::kRight;
		if (strcmp(name, "Down") == 0)					return Keys::kDown;
		if (strcmp(name, "Select") == 0)				return Keys::kSelect;
		if (strcmp(name, "Print") == 0)					return Keys::kPrint;
		if (strcmp(name, "Execute") == 0)				return Keys::kExecute;
		if (strcmp(name, "Print Screen") == 0)	return Keys::kPrintScreen;
		if (strcmp(name, "Insert") == 0)				return Keys::kInsert;
		if (strcmp(name, "Delete") == 0)				return Keys::kDelete;
		if (strcmp(name, "Help") == 0)					return Keys::kHelp;
		if (strcmp(name, "0") == 0)					    return Keys::kZero;
		if (strcmp(name, "1") == 0)						  return Keys::kOne;
		if (strcmp(name, "2") == 0)						  return Keys::kTwo;
		if (strcmp(name, "3") == 0)					    return Keys::kThree;
		if (strcmp(name, "4") == 0)					    return Keys::kFour;
		if (strcmp(name, "5") == 0)					    return Keys::kFive;
		if (strcmp(name, "6") == 0)						  return Keys::kSix;
		if (strcmp(name, "7") == 0)					    return Keys::kSeven;
		if (strcmp(name, "8") == 0)					    return Keys::kEight;
		if (strcmp(name, "9") == 0)					    return Keys::kNine;
		if (strcmp(name, "A") == 0)							return Keys::kA;
		if (strcmp(name, "B") == 0)							return Keys::kB;
		if (strcmp(name, "C") == 0)							return Keys::kC;
		if (strcmp(name, "D") == 0)							return Keys::kD;
		if (strcmp(name, "E") == 0)							return Keys::kE;
		if (strcmp(name, "F") == 0)							return Keys::kF;
		if (strcmp(name, "G") == 0)							return Keys::kG;
		if (strcmp(name, "H") == 0)							return Keys::kH;
		if (strcmp(name, "I") == 0)							return Keys::kI;
		if (strcmp(name, "J") == 0)							return Keys::kJ;
		if (strcmp(name, "K") == 0)							return Keys::kK;
		if (strcmp(name, "L") == 0)							return Keys::kL;
		if (strcmp(name, "M") == 0)							return Keys::kM;
		if (strcmp(name, "N") == 0)							return Keys::kN;
		if (strcmp(name, "O") == 0)							return Keys::kO;
		if (strcmp(name, "P") == 0)							return Keys::kP;
		if (strcmp(name, "Q") == 0)							return Keys::kQ;
		if (strcmp(name, "R") == 0)							return Keys::kR;
		if (strcmp(name, "S") == 0)							return Keys::kS;
		if (strcmp(name, "T") == 0)							return Keys::kT;
		if (strcmp(name, "U") == 0)							return Keys::kU;
		if (strcmp(name, "V") == 0)							return Keys::kV;
		if (strcmp(name, "W") == 0)							return Keys::kW;
		if (strcmp(name, "X") == 0)							return Keys::kX;
		if (strcmp(name, "Y") == 0)							return Keys::kY;
		if (strcmp(name, "Z") == 0)							return Keys::kZ;
		if (strcmp(name, "Left Windows") == 0)	return Keys::kLeftWindowsKey;
		if (strcmp(name, "Right Windows") == 0)	return Keys::kRightWindowsKey;
		if (strcmp(name, "Applications") == 0)	return Keys::kApplicationsKey;
		if (strcmp(name, "Sleep") == 0)					return Keys::kSleep;
		if (strcmp(name, "Numpad0") == 0)				return Keys::kNumPad0;
		if (strcmp(name, "Numpad1") == 0)				return Keys::kNumPad1;
		if (strcmp(name, "Numpad2") == 0)				return Keys::kNumPad2;
		if (strcmp(name, "Numpad3") == 0)				return Keys::kNumPad3;
		if (strcmp(name, "Numpad4") == 0)				return Keys::kNumPad4;
		if (strcmp(name, "Numpad5") == 0)				return Keys::kNumPad5;
		if (strcmp(name, "Numpad6") == 0)				return Keys::kNumPad6;
		if (strcmp(name, "Numpad7") == 0)				return Keys::kNumPad7;
		if (strcmp(name, "Numpad8") == 0)				return Keys::kNumPad8;
		if (strcmp(name, "Numpad9") == 0)				return Keys::kNumPad9;
		if (strcmp(name, "Multiply") == 0)			return Keys::kMultiply;
		if (strcmp(name, "Plus") == 0)					return Keys::kPlus;
		if (strcmp(name, "Seperator") == 0)			return Keys::kSeperator;
		if (strcmp(name, "Minus") == 0)					return Keys::kMinus;
		if (strcmp(name, "Decimal") == 0)				return Keys::kDecimal;
		if (strcmp(name, "Divide") == 0)				return Keys::kDivide;
		if (strcmp(name, "F1") == 0)						return Keys::kF1;
		if (strcmp(name, "F2") == 0)						return Keys::kF2;
		if (strcmp(name, "F3") == 0)						return Keys::kF3;
		if (strcmp(name, "F4") == 0)						return Keys::kF4;
		if (strcmp(name, "F5") == 0)						return Keys::kF5;
		if (strcmp(name, "F6") == 0)						return Keys::kF6;
		if (strcmp(name, "F7") == 0)						return Keys::kF7;
		if (strcmp(name, "F8") == 0)						return Keys::kF8;
		if (strcmp(name, "F9") == 0)						return Keys::kF9;
		if (strcmp(name, "F10") == 0)						return Keys::kF10;
		if (strcmp(name, "F11") == 0)						return Keys::kF11;
		if (strcmp(name, "F12") == 0)						return Keys::kF12;
		if (strcmp(name, "F13") == 0)						return Keys::kF13;
		if (strcmp(name, "F14") == 0)						return Keys::kF14;
		if (strcmp(name, "F15") == 0)						return Keys::kF15;
		if (strcmp(name, "F16") == 0)						return Keys::kF16;
		if (strcmp(name, "F17") == 0)						return Keys::kF17;
		if (strcmp(name, "F18") == 0)						return Keys::kF18;
		if (strcmp(name, "F19") == 0)						return Keys::kF19;
		if (strcmp(name, "F20") == 0)						return Keys::kF20;
		if (strcmp(name, "F21") == 0)						return Keys::kF21;
		if (strcmp(name, "F22") == 0)						return Keys::kF22;
		if (strcmp(name, "F23") == 0)						return Keys::kF23;
		if (strcmp(name, "F24") == 0)						return Keys::kF24;
		if (strcmp(name, "Numlock") == 0)				return Keys::kNumlock;
		if (strcmp(name, "Scroll Lock") == 0)		return Keys::kScrollLock;
		if (strcmp(name, "Left Shift") == 0)		return Keys::kLeftShift;
		if (strcmp(name, "Right Shift") == 0)		return Keys::kRightShift;
		if (strcmp(name, "Left Control") == 0)	return Keys::kLeftControl;
		if (strcmp(name, "Right Control") == 0)	return Keys::kRightControl;
		if (strcmp(name, "Left Menu") == 0)			return Keys::kLeftMenu;
		if (strcmp(name, "Right Menu") == 0)		return Keys::kRightMenu;
		if (strcmp(name, "Browse Back") == 0)		return Keys::kBrowserBack;
		if (strcmp(name, "Browse Forward") == 0)return Keys::kBrowserForward;
		if (strcmp(name, "Browse Refresh") == 0)return Keys::kBrowserRefresh;
		if (strcmp(name, "Browse Stop") == 0)		return Keys::kBrowserStop;
		if (strcmp(name, "Browse Search") == 0)	return Keys::kBrowserSearch;
		if (strcmp(name, "Browse Fave") == 0)		return Keys::kBrowserFavorites;
		if (strcmp(name, "Browse Home") == 0)		return Keys::kBrowserHome;
		if (strcmp(name, "Volume Mute") == 0)		return Keys::kVolumeMute;
		if (strcmp(name, "Volume Down") == 0)		return Keys::kVolumeDown;
		if (strcmp(name, "Volume Up") == 0)			return Keys::kVolumeUp;
		if (strcmp(name, "Next Track") == 0)		return Keys::kNextTrack;
		if (strcmp(name, "Previous Track") == 0)return Keys::kPreviousTrack;
		if (strcmp(name, "Stop Media") == 0)		return Keys::kStopMedia;
		if (strcmp(name, "Play Pause") == 0)		return Keys::kPlayPause;
		if (strcmp(name, "Mail") == 0)					return Keys::kLaunchMail;
		if (strcmp(name, "Select Media") == 0)	return Keys::kSelectMedia;
		if (strcmp(name, "Launch App 1") == 0)	return Keys::kLaunchApp1;
		if (strcmp(name, "Launch App 2") == 0)	return Keys::kLaunchApp2;
		if (strcmp(name, "OEM1") == 0)					return Keys::kOEM1;
		if (strcmp(name, "OEM Plus") == 0)			return Keys::kOEMPlus;
		if (strcmp(name, "OEM Comma") == 0)			return Keys::kOEMComma;
		if (strcmp(name, "OEM Minus") == 0)			return Keys::kOEMMinus;
		if (strcmp(name, "OEM Period") == 0)		return Keys::kOEMPeriod;
		if (strcmp(name, "OEM2") == 0)					return Keys::kOEM2;
		if (strcmp(name, "OEM3") == 0)					return Keys::kOEM3;
		if (strcmp(name, "OEM4") == 0)					return Keys::kOEM4;
		if (strcmp(name, "OEM5") == 0)					return Keys::kOEM5;
		if (strcmp(name, "OEM6") == 0)					return Keys::kOEM6;
		if (strcmp(name, "OEM7") == 0)					return Keys::kOEM7;
		if (strcmp(name, "OEM8") == 0)					return Keys::kOEM8;
		if (strcmp(name, "OEM102") == 0)				return Keys::kOEM102;
		if (strcmp(name, "Process") == 0)				return Keys::kProcess;
		if (strcmp(name, "Packet") == 0)				return Keys::kPacket;
		if (strcmp(name, "Attn") == 0)					return Keys::kAttn;
		if (strcmp(name, "CrSel") == 0)					return Keys::kCrSel;
		if (strcmp(name, "ExSel") == 0)					return Keys::kExSel;
		if (strcmp(name, "Erase EOF") == 0)			return Keys::kEraseEOF;
		if (strcmp(name, "Play") == 0)					return Keys::kPlay;
		if (strcmp(name, "Zoom") == 0)					return Keys::kZoom;
		if (strcmp(name, "PA1") == 0)						return Keys::kPA1;
		if (strcmp(name, "OEM Clear") == 0)			return Keys::kOEMClear;
		if (strcmp(name, "None") == 0)					return Keys::kNone;

		return Keys::kNone;
	}

	//-------------------------------------------------------------------------------------------
	std::string Key::KeyToString(Key::Keys key)
	{
		switch (key)
		{
		case Keys::kBackspace:						return "Backspace";
		case Keys::kTab:									return "Tab";
		case Keys::kClear:								return "Clear";
		case Keys::kEnter:								return "Enter";
		case Keys::kShift:								return "Shift";
		case Keys::kControl:							return "Control";
		case Keys::kAlt:									return "Alt";
		case Keys::kPause:								return "Pause";
		case Keys::kCapsLock:							return "Caps Lock";
		case Keys::kEscape:								return "Escape";
		case Keys::kSpace:								return "Space";
		case Keys::kPageUp:								return "Page Up";
		case Keys::kEnd:									return "End";
		case Keys::kHome:									return "Home";
		case Keys::kLeft:									return "Left";
		case Keys::kUp:										return "Up";
		case Keys::kRight:								return "Right";
		case Keys::kDown:									return "Down";
		case Keys::kSelect:								return "Select";
		case Keys::kPrint:								return "Print";
		case Keys::kExecute:							return "Execute";
		case Keys::kPrintScreen:					return "Print Screen";
		case Keys::kInsert:								return "Insert";
		case Keys::kDelete:								return "Delete";
		case Keys::kHelp:									return "Help";
		case Keys::kZero:									return "0";
		case Keys::kOne:									return "1";
		case Keys::kTwo:									return "2";
		case Keys::kThree:								return "3";
		case Keys::kFour:									return "4";
		case Keys::kFive:									return "5";
		case Keys::kSix:									return "6";
		case Keys::kSeven:								return "7";
		case Keys::kEight:								return "8";
		case Keys::kNine:									return "9";
		case Keys::kA:										return "A";
		case Keys::kB:										return "B";
		case Keys::kC:										return "C";
		case Keys::kD:										return "D";
		case Keys::kE:										return "E";
		case Keys::kF:										return "F";
		case Keys::kG:										return "G";
		case Keys::kH:										return "H";
		case Keys::kI:										return "I";
		case Keys::kJ:										return "J";
		case Keys::kK:										return "K";
		case Keys::kL:										return "L";
		case Keys::kM:										return "M";
		case Keys::kN:										return "N";
		case Keys::kO:										return "O";
		case Keys::kP:										return "P";
		case Keys::kQ:										return "Q";
		case Keys::kR:										return "R";
		case Keys::kS:										return "S";
		case Keys::kT:										return "T";
		case Keys::kU:										return "U";
		case Keys::kV:										return "V";
		case Keys::kW:										return "W";
		case Keys::kX:										return "X";
		case Keys::kY:										return "Y";
		case Keys::kZ:										return "Z";
		case Keys::kLeftWindowsKey:				return "Left Windows";
		case Keys::kRightWindowsKey:			return "Right Windows";
		case Keys::kApplicationsKey:			return "Applications";
		case Keys::kSleep:								return "Sleep";
		case Keys::kNumPad0:							return "Numpad0";
		case Keys::kNumPad1:							return "Numpad1";
		case Keys::kNumPad2:							return "Numpad2";
		case Keys::kNumPad3:							return "Numpad3";
		case Keys::kNumPad4:							return "Numpad4";
		case Keys::kNumPad5:							return "Numpad5";
		case Keys::kNumPad6:							return "Numpad6";
		case Keys::kNumPad7:							return "Numpad7";
		case Keys::kNumPad8:							return "Numpad8";
		case Keys::kNumPad9:							return "Numpad9";
		case Keys::kMultiply:							return "Multiply";
		case Keys::kPlus:									return "Plus";
		case Keys::kSeperator:						return "Seperator";
		case Keys::kMinus:								return "Minus";
		case Keys::kDecimal:							return "Decimal";
		case Keys::kDivide:								return "Divide";
		case Keys::kF1:										return "F1";
		case Keys::kF2:										return "F2";
		case Keys::kF3:										return "F3";
		case Keys::kF4:										return "F4";
		case Keys::kF5:										return "F5";
		case Keys::kF6:										return "F6";
		case Keys::kF7:										return "F7";
		case Keys::kF8:										return "F8";
		case Keys::kF9:										return "F9";
		case Keys::kF10:									return "F10";
		case Keys::kF11:									return "F11";
		case Keys::kF12:									return "F12";
		case Keys::kF13:									return "F13";
		case Keys::kF14:									return "F14";
		case Keys::kF15:									return "F15";
		case Keys::kF16:									return "F16";
		case Keys::kF17:									return "F17";
		case Keys::kF18:									return "F18";
		case Keys::kF19:									return "F19";
		case Keys::kF20:									return "F20";
		case Keys::kF21:									return "F21";
		case Keys::kF22:									return "F22";
		case Keys::kF23:									return "F23";
		case Keys::kF24:									return "F24";
		case Keys::kNumlock:							return "Numlock";
		case Keys::kScrollLock:						return "Scroll Lock";
		case Keys::kLeftShift:						return "Left Shift";
		case Keys::kRightShift:						return "Right Shift";
		case Keys::kLeftControl:					return "Left Control";
		case Keys::kRightControl:					return "Right Control";
		case Keys::kLeftMenu:							return "Left Menu";
		case Keys::kRightMenu:						return "Right Menu";
		case Keys::kBrowserBack:					return "Browser Back";
		case Keys::kBrowserForward:				return "Browser Forward";
		case Keys::kBrowserRefresh:				return "Browser Refresh";
		case Keys::kBrowserStop:					return "Browser Stop";
		case Keys::kBrowserSearch:				return "Browser Search";
		case Keys::kBrowserFavorites:			return "Browser Fave";
		case Keys::kBrowserHome:					return "Browser Home";
		case Keys::kVolumeMute:						return "Volume Mute";
		case Keys::kVolumeDown:						return "Volume Down";
		case Keys::kVolumeUp:							return "Volume Up";
		case Keys::kNextTrack:						return "Next Track";
		case Keys::kPreviousTrack:				return "Previous Track";
		case Keys::kStopMedia:						return "Stop Media";
		case Keys::kPlayPause:						return "Play Pause";
		case Keys::kLaunchMail:						return "Mail";
		case Keys::kSelectMedia:					return "Select Media";
		case Keys::kLaunchApp1:						return "Launch App 1";
		case Keys::kLaunchApp2:						return "Launch App 2";
		case Keys::kOEM1:									return "OEM1";
		case Keys::kOEMPlus:							return "OEM Plus";
		case Keys::kOEMComma:							return "OEM Comma";
		case Keys::kOEMMinus:							return "OEM Minus";
		case Keys::kOEMPeriod:						return "OEM Period";
		case Keys::kOEM2:									return "OEM2";
		case Keys::kOEM3:									return "OEM3";
		case Keys::kOEM4:									return "OEM4";
		case Keys::kOEM5:									return "OEM5";
		case Keys::kOEM6:									return "OEM6";
		case Keys::kOEM7:									return "OEM7";
		case Keys::kOEM8:									return "OEM8";
		case Keys::kOEM102:								return "OEM102";
		case Keys::kProcess:							return "Process";
		case Keys::kPacket:								return "Packet";
		case Keys::kAttn:									return "Attn";
		case Keys::kCrSel:								return "CrSel";
		case Keys::kExSel:								return "ExSel";
		case Keys::kEraseEOF:							return "Erase EOF";
		case Keys::kPlay:									return "Play";
		case Keys::kZoom:									return "Zoom";
		case Keys::kPA1:									return "PA1";
		case Keys::kOEMClear:							return "OEM Clear";
		case Keys::kNone:									return "None";
		default:													return "unknown";
		}
	}
}