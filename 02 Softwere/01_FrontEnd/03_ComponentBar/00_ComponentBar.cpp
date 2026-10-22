#include "00_ComponentBar.h"
#include <QAction>
#include <QToolBar>
#include "01_FrontEnd/02_Canvas/00_Canvas.h"
#include "02_BackEnd/00_ClassStructures/00_Project.h"

void buildComponentBar(QToolBar* bar, const Project& project, Canvas* canvas)
{
    for (const auto& [libraryName, library] : project.libraries)
    {
        for (const auto& [definitionName, definition] : library.getDefinitions())
        {
            const std::string definitionId = libraryName + ":" + definitionName;
            QAction* action = bar->addAction(QString::fromStdString(definitionName));
            QObject::connect(action, &QAction::triggered, canvas,
                             [canvas, definitionId] { canvas->addGate(definitionId); });
        }
        bar->addSeparator();
    }

    // Delete tool: while on, clicking a gate or wire removes it. Esc turns it off.
    QAction* deleteAction = bar->addAction("Delete");
    deleteAction->setCheckable(true);
    QObject::connect(deleteAction, &QAction::toggled, canvas,
                     [canvas](bool on) { canvas->setDeleteMode(on); });
    canvas->setDeleteAction(deleteAction);
}