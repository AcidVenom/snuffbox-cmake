#pragma once

#include "ui_console.h"

#include "../application/logging.h"

namespace snuffbox
{
  /**
  * @class snuffbox::Console
  * @brief A console created in Qt, for debugging purposes on cross-platform systems
  * @author Daniël Konings
  */
  class Console : public QObject
  {
  public:
    Q_OBJECT;

  public:
    /// Default constructor
    Console();

    /// Default destructor
    ~Console();

    /**
    * @brief Returns the singleton instance of this object
    * @return snuffbox::Console* The pointer to the object
    */
    static Console* Instance();

    /// Checks if the CVar was set for a console, enable the console if it was set
    void CheckEnabled();

    /**
    * @brief Initialsies the console on a main window
    * @param[in] parent (QMainWindow*) The main window parent
    */
    void Initialise(QMainWindow* parent);

    /// Shows the console if hidden
    void Show();
    
    /**
    * @brief Logs to the console
    * @param[in] colour (snuffbox::DebugLogging::LogColour) The colour scheme to log this message with
    * @param[in] message (std::string) The message to log
    */
    void Log(DebugLogging::LogColour colour, std::string message);

    /**
    * @return bool Is the console visible?
    */
    bool IsVisible();

    /**
    * @return const bool& Is the console enabled?
    */
    const bool& enabled() const;

  private:
    Ui::Console* ui_; //!< The Qt UI associated with the console
    QMainWindow* parent_; //!< The parent window of the console
    bool enabled_; //!< Is the console enabled?
  };
}
