#include "00_ComponentBar.h"
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include "01_FrontEnd/02_Canvas/00_Canvas.h"
#include "02_BackEnd/00_ClassStructures/00_Project.h"

namespace
{
QString keyFor(Canvas::Tool tool, const std::string& definitionId)
{
    switch (tool)
    {
    case Canvas::Tool::Wire:   return "wire";
    case Canvas::Tool::Move:   return "move";
    case Canvas::Tool::Delete: return "delete";
    case Canvas::Tool::Place:  return "place:" + QString::fromStdString(definitionId);
    case Canvas::Tool::Select: break;
    }
    return {};
}

void activate(Canvas* canvas, const QString& key)
{
    if (key == "wire")        canvas->setTool(Canvas::Tool::Wire);
    else if (key == "move")   canvas->setTool(Canvas::Tool::Move);
    else if (key == "delete") canvas->setTool(Canvas::Tool::Delete);
    else if (key.startsWith("place:"))
        canvas->setTool(Canvas::Tool::Place, key.mid(6).toStdString());
}
}

void buildComponentBar(QToolBar* bar, const Project& project, Canvas* canvas)
{
    auto* group = new QActionGroup(bar);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);

    auto addTool = [&](const QString& text, const QString& key) {
        QAction* action = bar->addAction(text);
        action->setCheckable(true);
        action->setData(key);
        group->addAction(action);
    };

    addTool("Wire", "wire");
    addTool("Move", "move");
    addTool("Delete", "delete");
    bar->addSeparator();

    for (const auto& [libraryName, library] : project.libraries)
    {
        for (const auto& [definitionName, definition] : library.getDefinitions())
            addTool(QString::fromStdString(definitionName),
                    "place:" + QString::fromStdString(libraryName + ":" + definitionName));
        bar->addSeparator();
    }

    QObject::connect(group, &QActionGroup::triggered, canvas, [canvas](QAction* action) {
        if (action->isChecked()) activate(canvas, action->data().toString());
        else canvas->setTool(Canvas::Tool::Select);
    });

    canvas->onToolChanged([group](Canvas::Tool tool, const std::string& definitionId) {
        const QString key = keyFor(tool, definitionId);
        for (QAction* action : group->actions())
            if (action->data().toString() == key)
            {
                action->setChecked(true);
                return;
            }
        if (QAction* checked = group->checkedAction())
            checked->setChecked(false);
    });
}