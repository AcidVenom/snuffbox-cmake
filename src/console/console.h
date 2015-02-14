#pragma once
#include "ui_console.h"

#include "../application/logging.h"

namespace snuffbox
{
  class Console : public QObject
  {
  public:
    Q_OBJECT;

  public:
    /// Default constructor
    Console();

    /// Default destructor
    ~Console();

    static Console* Instance();

    void CheckEnabled();
    void Initialise(QMainWindow* parent);
    void Show();

    void Log(DebugLogging::LogColour colour, std::string message);

    bool IsVisible();

    const bool& enabled() const;

  private:
    Ui::Console* ui_; //!< The Qt UI associated with the console
    QMainWindow* parent_; //!< The parent window of the console
    bool enabled_; //!< Is the console enabled?
  };
}
