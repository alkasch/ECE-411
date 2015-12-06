//	Copyright 2003 Ashley Wise
//	University Of Illinois Urbana-Champaign
//	awise@crhc.uiuc.edu

#include "DataValuesWindow.h"
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include "Project.h"
#include "MainWindow.h"
#include "FileWindow.h"
#include "SimulatorWindow.h"

using namespace std;
using namespace JMT;
using namespace Assembler;
using namespace LC3;
using namespace LC3b;

namespace AshIDE	{

Fl_Menu_Item DataValuesWindow::MenuItems[] =
	{
		{ "&Data Values", 0, 0, 0, FL_SUBMENU },
			{ "&Save",			FL_CTRL + 's',	SaveCB, 0, FL_MENU_DIVIDER },
			{ "&Copy",			FL_CTRL + 'c',	CopyCB, 0, FL_MENU_DIVIDER },
			{ "&Help",			FL_F + 1,		HelpCB, 0, FL_MENU_DIVIDER },
			{ "C&lose",			FL_Escape,		CloseCB },
			{ 0 },

		{ "&Search", 0, 0, 0, FL_SUBMENU },
			{ "&Find...",		FL_CTRL + 'f',	FindCB },
			{ "F&ind Again",	FL_CTRL + 'g',	FindAgainCB },
			{ 0 },

		{ "S&imulate", 0, 0, 0, FL_SUBMENU },
			{ "&Go",							FL_F + 5,		SimulatorWindow::GoCB },
			{ "Go One &Instruction",			FL_F + 6,		SimulatorWindow::GoOneCB, 0, FL_MENU_DIVIDER },
			{ "Step I&nto Next Function",		FL_F + 8,		SimulatorWindow::StepInCB },
			{ "Step O&ver Next Function",		FL_F + 9,		SimulatorWindow::StepOverCB },
			{ "Step &Out Of Current Function",	FL_F + 10,		SimulatorWindow::StepOutCB, 0, FL_MENU_DIVIDER },
			{ "&Signal Interrupt",				0,				SimulatorWindow::InterruptCB },
			{ "Brea&k",							FL_F + 11,		SimulatorWindow::BreakCB },
			{ "&Reset",							FL_F + 12,		SimulatorWindow::ResetCB },
			{ "&End Simulation",				FL_CTRL + 'q',	SimulatorWindow::StopCB, 0, FL_MENU_DIVIDER },

			{ "&Breakpoints", 0, 0, 0, FL_SUBMENU },
				{ "&Breakpoints",	FL_CTRL + 'b',	SimulatorWindow::BreakpointsCB },
				{ 0 },

			{ "Vie&w", 0, 0, 0, FL_SUBMENU },
				{ "&Console I/O",	FL_CTRL + 'i',	SimulatorWindow::ConsoleCB, 0, FL_MENU_DIVIDER },
				{ "&Disassembly",	FL_CTRL + 'd',	SimulatorWindow::DisassemblyCB },
				{ "Call &Stack",	FL_CTRL + 'k',	SimulatorWindow::CallStackCB, 0, FL_MENU_DIVIDER },
				{ "&Instructions",	FL_CTRL + '1',	SimulatorWindow::InstructionsCB },
				{ "D&ata Values",	FL_CTRL + '2',	SimulatorWindow::DataValuesCB },
				{ "&Memory Bytes",	FL_CTRL + '3',	SimulatorWindow::MemoryBytesCB },
				{ "&Registers",		FL_CTRL + '4',	SimulatorWindow::RegistersCB },
				{ "Pipe&lines",		FL_CTRL + '5',	SimulatorWindow::PipelinesCB },
				{ "&Programs",		FL_CTRL + '6',	SimulatorWindow::ProgramsCB },
				{ 0 },

			{ "&Edit && Continue", 0, 0, 0, FL_SUBMENU },
				{ "Edit &Instruction/Data/Memory",	FL_F + 2,	SimulatorWindow::WriteDataCB },
				{ "Edit &Register",					FL_F + 3,	SimulatorWindow::WriteRegisterCB },
				{ 0 },

			{ 0 },

		{ 0 }
	};

/*
FL_WHITE
FL_BLACK
FL_CYAN
FL_DARK_CYAN
FL_BLUE
FL_DARK_BLUE
FL_GREEN
FL_DARK_GREEN
FL_YELLOW
FL_DARK_YELLOW
FL_MAGENTA
FL_DARK_MAGENTA
FL_RED
FL_DARK_RED
enum TokenEnum {TUnknown, TBad, TComment, TCharConst, TCharacter, TString, TInteger, TReal, TIdentifier, TOperator, TDirective, TAttribute, TExpand, TData, TISA, TOpcode, TRegister};
*/
Fl_Text_Display::Style_Table_Entry DataValuesWindow::StyleTable[] = 
	{	// Style table
		{ FL_BLACK,			FL_COURIER,			14 },	// A - Unknown
		{ FL_BLACK,			FL_COURIER,			14 },	// B - Bad
		{ FL_DARK_GREEN,	FL_COURIER,			14 },	// C - Comment
		{ FL_BLACK,			FL_COURIER,			14 },	// D - CharConst
		{ FL_MAGENTA,		FL_COURIER,			14 },	// E - Character
		{ FL_DARK_MAGENTA,	FL_COURIER,			14 },	// F - String
		{ FL_RED,			FL_COURIER,			14 },	// G - Integer
		{ FL_RED,			FL_COURIER_ITALIC,	14 },	// H - Real
		{ FL_BLACK,			FL_COURIER,			14 },	// I - Identifier
		{ FL_BLUE,			FL_COURIER,			14 },	// J - Operator
		{ FL_BLUE,			FL_COURIER,			14 },	// K - Directive
		{ FL_DARK_CYAN,		FL_COURIER_ITALIC,	14 },	// L - Attribute
		{ FL_BLACK,			FL_COURIER,			14 },	// M - Expand
		{ FL_DARK_CYAN,		FL_COURIER_BOLD,	14 },	// N - Data
		{ FL_BLACK,			FL_COURIER,			14 },	// O - ISA
		{ FL_DARK_BLUE,		FL_COURIER_BOLD,	14 },	// P - Opcode
		{ FL_DARK_RED,		FL_COURIER_BOLD,	14 },	// Q - Register

		{ FL_DARK_CYAN,		FL_COURIER,			14 },	// R - Filename 
		{ FL_BLACK,			FL_COURIER_BOLD,	14 },	// S - Info
		{ FL_DARK_BLUE,		FL_COURIER_BOLD,	14 },	// T - Warning
		{ FL_DARK_RED,		FL_COURIER_BOLD,	14 },	// U - Error
		{ FL_RED,			FL_COURIER_BOLD,	14 },	// V - Fatal
		{ FL_DARK_MAGENTA,	FL_COURIER_BOLD,	14 },	// W - Exception
		{ FL_DARK_GREEN,	FL_COURIER_BOLD,	14 }	// X - Breakpoint
	};

DataValuesWindow::DataValuesWindow() : Fl_Double_Window(300, 360)
{
	{
		//Setup the menu
		pMainMenu = new Fl_Menu_Bar(0, 0, w(), 28);
		pMainMenu->copy(MenuItems, this);

		//Setup the data values display
		pTextDisplay = new ReadOnlyEditor(0, 28+20*4+16, w(), h()-28*2-20*4-16, StyleTable, sizeof(StyleTable)/sizeof(StyleTable[0]));
		pTextDisplay->pTextBuffer->add_modify_callback(UpdateStyleCB, this);

		//Setup the status bar
		pStatus = new Fl_Output(0, h()-28, w(), 28);
		pStatus->box(FL_PLASTIC_UP_BOX);

		//Setup the address query
		pLineInput = new Fl_Round_Button(0, 28, 120, 20, "Line Of Code");
		pLineInput->type(FL_RADIO_BUTTON);
		pLineInput->callback(ChangeInputCB, this);
		pLineNumber = new Fl_Input(220, 28, 80, 20, "Line Number");
		pLineNumber->deactivate();
		pLineNumber->value("1");

		pMemoryInput = new Fl_Round_Button(0, 28+20, 120, 20, "Physical Memory");
		pMemoryInput->type(FL_RADIO_BUTTON);
		pMemoryInput->callback(ChangeInputCB, this);
		pMemoryName = new Fl_Choice(220, 28+20, 80, 20, "Memory");
		pMemoryName->box(FL_PLASTIC_DOWN_BOX);
		pMemoryName->deactivate();
		for(Architecture::MemoryMap::iterator MemIter = TheArch(Memories.begin()); MemIter != TheArch(Memories.end()); MemIter++)
			pMemoryName->add(MemIter->second.sName.c_str());
		pMemoryName->value(0);

		pDataInput = new Fl_Round_Button(0, 28+20*2, 120, 20, "Logical Data");
		pDataInput->type(FL_RADIO_BUTTON);
		pDataInput->callback(ChangeInputCB, this);
		pDataInput->value(1);
		pDataType = new Fl_Choice(220, 28+20*2, 80, 20, "Data Type");
		pDataType->box(FL_PLASTIC_DOWN_BOX);
		pDataType->add("DATA1");
		pDataType->add("DATA2");
		pDataType->add("DATA4");
		pDataType->add("DATA8");
		pDataType->add("REAL1");
		pDataType->add("REAL2");
		pDataType->add("REAL4");
		pDataType->add("REAL8");
		pDataType->add("Auto");
		pDataType->callback(ChangeDataTypeCB, this);
		pDataType->value(STRUCT);

		pSigned = new Fl_Check_Button(220, 28+20*3, 80, 16, "Signed");

		pDataAddress = new Fl_Input(0, 28+20*3+16, 160, 20);	//Symbol Or Byte Address
		pDataAddress->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		pDataAddress->value("0");
		pLength = new Fl_Input(160, 28+20*3+16, 60, 20, "Length");
		pLength->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
		pLength->value("10");
		pEnter = new Fl_Return_Button(220, 28+20*3+16, 80, 20, "Display");
		pEnter->callback(EnterCB, this);

		ChangeInput();
	}
	end();
	resizable(pTextDisplay);
	callback(CloseCB, this);

	show();
	focus(pDataAddress);
	SetTitle();
	Enter();
}

DataValuesWindow::~DataValuesWindow()
{
	hide();
	SimulatorWindow::DataValuesMap::iterator DataValueIter = TheSimulatorWindow.DataValues.find(this);
	if(DataValueIter == TheSimulatorWindow.DataValues.end())
		throw "DataValues does not contain this window!";
	TheSimulatorWindow.DataValues.erase(DataValueIter);
	Fl::first_window()->show();
}

void DataValuesWindow::HelpCB(Fl_Widget *pW, void *pV)
{
	TheMainWindow.Help(AshIDEIndex, "DataValuesWindow");
}

bool DataValuesWindow::Close()
{
	Fl::delete_widget(this);
	return true;
}

bool DataValuesWindow::SetTitle()
{
	string sTitle = "Data Values";

	label(sTitle.c_str());
	return true;
}

bool DataValuesWindow::ChangeInput()
{
	if(pMemoryInput->value())
	{
		pMemoryName->activate();
		pDataAddress->label("Symbol or Byte Address");
	}
	else
	{
		pMemoryName->deactivate();
		pDataAddress->label(TheProject.SimISA == LangLC3 ? "Symbol or Word Address" : "Symbol or Byte Address");
	}

	if(pLineInput->value())
	{
		pLineNumber->activate();
		pDataAddress->deactivate();
	}
	else
	{
		pLineNumber->deactivate();
		pDataAddress->activate();
	}

	return true;
}

bool DataValuesWindow::ChangeDataType()
{
	DataEnum DataType = (DataEnum)pDataType->value();
	if(DataType >= DATA1 && DataType <= DATA8)
	{
		pSigned->activate();
	}
	else
	{
		pSigned->deactivate();
	}
	return true;
}

bool DataValuesWindow::Enter()
{
	bool fRetVal = true;

	if(pLineInput->value())
	{
		const char *sLineNumber = pLineNumber->value();
		const char *sLength = pLength->value();
		if(sLineNumber[0] == 0 || sLength[0] == 0)
		{
			fl_alert("You must provide a line number and display length.");
			return false;
		}
		DataEnum DataType = (DataEnum)pDataType->value();
		string sCommand;
		sCommand = string("dl ")+sLineNumber+" ";
		if(DataType >= DATA1 && DataType <= DATA8 && pSigned->value())
			sCommand += "- ";
		if(DataType < STRUCT)
			(sCommand += sDataTypes[DataType]) += " ";
		sCommand += sLength;
		TheSimulatorWindow.RedirectWindow = SimulatorWindow::SimDataValuesWindow;
		TheSimulatorWindow.pRedirectWindow = this;
		pTextDisplay->Clear();
		fRetVal = SIM_COMMAND(sCommand);
		TheSimulatorWindow.RedirectWindow = SimulatorWindow::SimNoWindow;
	}
	else if(pMemoryInput->value())
	{
		const char *sMemoryName = pMemoryName->text();
		const char *sDataAddress = pDataAddress->value();
		const char *sLength = pLength->value();
		if(!sMemoryName)
		{
			fl_alert("Architecture does not contain any memories.");
			return false;
		}
		if(sDataAddress[0] == 0 || sLength[0] == 0)
		{
			fl_alert("You must provide a memory name, symbol or byte address identifier, and display length.");
			return false;
		}
		DataEnum DataType = (DataEnum)pDataType->value();
		string sCommand;
		sCommand = string("dm ")+sMemoryName+" "+sDataAddress+" ";
		if(DataType >= DATA1 && DataType <= DATA8 && pSigned->value())
			sCommand += "- ";
		if(DataType < STRUCT)
			(sCommand += sDataTypes[DataType]) += " ";
		sCommand += sLength;
		TheSimulatorWindow.RedirectWindow = SimulatorWindow::SimDataValuesWindow;
		TheSimulatorWindow.pRedirectWindow = this;
		pTextDisplay->Clear();
		fRetVal = SIM_COMMAND(sCommand);
		TheSimulatorWindow.RedirectWindow = SimulatorWindow::SimNoWindow;
	}
	else
	{
		const char *sDataAddress = pDataAddress->value();
		const char *sLength = pLength->value();
		if(sDataAddress[0] == 0 || sLength[0] == 0)
		{
			fl_alert(TheProject.SimISA == LangLC3 ?
				"You must provide a symbol or word address identifier, and display length." :
				"You must provide a symbol or byte address identifier, and display length.");
			return false;
		}
		DataEnum DataType = (DataEnum)pDataType->value();
		string sCommand;
		sCommand = string("dd ")+sDataAddress+" ";
		if(DataType >= DATA1 && DataType <= DATA8 && pSigned->value())
			sCommand += "- ";
		if(DataType < STRUCT)
			(sCommand += sDataTypes[DataType]) += " ";
		sCommand += sLength;
		TheSimulatorWindow.RedirectWindow = SimulatorWindow::SimDataValuesWindow;
		TheSimulatorWindow.pRedirectWindow = this;
		pTextDisplay->Clear();
		fRetVal = SIM_COMMAND(sCommand);
		TheSimulatorWindow.RedirectWindow = SimulatorWindow::SimNoWindow;
	}

	if(!fRetVal)
	{
		fl_alert("Error displaying data.");
		show();
	}
	return fRetVal;
}

bool DataValuesWindow::ViewData(unsigned int ProgramNumber, const string &sIdentifier)
{
	pDataInput->value(1);
	ChangeInput();
	char sProgramNumber[16];
	sprintf(sProgramNumber, "{%u} ", ProgramNumber);
	string sDataAddress = string(sProgramNumber) + sIdentifier;
	pDataAddress->value(sDataAddress.c_str());
	pLength->value("1");
	return Enter();
}

bool DataValuesWindow::UpdateStyle(int Pos, int Inserted, int Deleted, int Restyled, const char *pszDeleted)
{
	int	Start, End;

	char *pText;

	//If this is just a selection change, then unselect the style buffer
	if(Inserted == 0 && Deleted == 0)
	{
		pTextDisplay->pStyleBuffer->unselect();
		return true;
	}

	//Select the area that was just updated to avoid unnecessary callbacks
	pTextDisplay->pStyleBuffer->select(Pos, Pos + Inserted - Deleted);

	//Re-parse the changed region; we do this by parsing from the
	//beginning of the line of the changed region to the end of
	//the line of the changed region.
	Start = pTextDisplay->pTextBuffer->line_start(Pos);
	End   = pTextDisplay->pTextBuffer->line_end(Pos + Inserted);
	string sText(pText = pTextDisplay->pTextBuffer->text_range(Start, End));
	delete [] pText;
	string sStyle(pText = pTextDisplay->pStyleBuffer->text_range(Start, End));
	delete [] pText;

	ParseStyle(sText, sStyle);

	pTextDisplay->pStyleBuffer->replace(Start, End, sStyle.c_str());
	pTextDisplay->redisplay_range(Start, End);
	return true;
}

bool DataValuesWindow::ParseStyle(const string &sText, string &sStyle)
{
	string::size_type TempLoc;

	//look for colons
	TempLoc = sText.find(':');
	if(TempLoc != string::npos && TempLoc != 0 && sStyle[TempLoc-1] != 'A')
		//This line was already styled
		return true;

	if(TheProject.SimISA == LangLC3)
		SimulatorWindow::TheLC3HighlightLexer.Lex(sText, sStyle);
	else if(TheProject.SimISA == LangLC3b)
		SimulatorWindow::TheLC3bHighlightLexer.Lex(sText, sStyle);

	return true;
}

}	//namespace AshIDE
