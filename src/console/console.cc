#include "../console/console.h"
#include "moc_console.cpp"

#include "../memory/allocated_memory.h"
#include "../memory/shared_ptr.h"

#include "../cvar/cvar.h"

#include <qstylefactory.h>
#include <qscrollbar.h>

namespace snuffbox
{
  //---------------------------------------------------------------------------------------------------------
  Console::Console() :
    ui_(nullptr),
    parent_(nullptr),
    enabled_(false)
  {
    
  }

  //---------------------------------------------------------------------------------------------------------
  void Console::CheckEnabled()
  {
    bool found = false;
    CVar::Value* cvar = CVar::Instance()->Get("console", &found);

    enabled_ = found == true && cvar->IsBool() && cvar->As<CVar::Boolean>()->value() == true;
  }

  //---------------------------------------------------------------------------------------------------------
  Console* Console::Instance()
  {
    static Console* console = new Console();
    return console;
  }

  //---------------------------------------------------------------------------------------------------------
  void Console::Initialise(QMainWindow* parent)
  {
    ui_ = new Ui::Console();
    ui_->setupUi(parent);

    parent_ = parent;

    QApplication* app = qApp;

    app->setStyle(QStyleFactory::create("Fusion"));

    QPalette p;
    p.setColor(QPalette::Window, QColor(75, 75, 75));
    p.setColor(QPalette::WindowText, Qt::white);
    p.setColor(QPalette::Base, QColor(48, 10, 36));
    p.setColor(QPalette::AlternateBase, QColor(75 / 2, 75 / 2, 75 / 2));
    p.setColor(QPalette::ToolTipBase, Qt::white);
    p.setColor(QPalette::ToolTipText, Qt::white);
    p.setColor(QPalette::Text, Qt::white);
    p.setColor(QPalette::Button, QColor(75, 75, 75));
    p.setColor(QPalette::ButtonText, Qt::white);
    p.setColor(QPalette::BrightText, Qt::red);
    p.setColor(QPalette::Link, QColor(159, 164, 98));

    p.setColor(QPalette::Highlight, QColor(240, 115, 75));
    p.setColor(QPalette::HighlightedText, Qt::black);

    app->setPalette(p);

    Show();
  }

  //---------------------------------------------------------------------------------------------------------
  void Console::Show()
  {
    SNUFF_ASSERT_NOTNULL(ui_, "Console::Show::ui_");
    SNUFF_ASSERT_NOTNULL(parent_, "Console::Show::parent_");

    parent_->show();
  }

  //---------------------------------------------------------------------------------------------------------
  void Console::Log(DebugLogging::LogColour colour, std::string message)
  {
    if (enabled_ == false || ui_ == nullptr || parent_ == nullptr)
    {
      return;
    }
  
    ui_->terminal->append(message.c_str());
    
    ui_->terminal->verticalScrollBar()->setValue(ui_->terminal->verticalScrollBar()->maximum());
  }

  //---------------------------------------------------------------------------------------------------------
  bool Console::IsVisible()
  {
    return parent_ != nullptr && parent_->isVisible();
  }

  //---------------------------------------------------------------------------------------------------------
  const bool& Console::enabled() const
  {
    return enabled_;
  }

  //---------------------------------------------------------------------------------------------------------
  Console::~Console()
  {
    SNUFF_ASSERT_NOTNULL(parent_, "Console::~Console::parent_");
    delete parent_;
    parent_ = nullptr;

    SNUFF_ASSERT_NOTNULL(ui_, "Console::~Console::ui_");
    delete ui_;
    ui_ = nullptr;
  }
}