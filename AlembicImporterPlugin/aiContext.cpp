#include "pch.h"
#include "AlembicImporter.h"
#include "aiObject.h"
#include "aiGeometry.h"
#include "aiContext.h"



aiContext* aiContext::create()
{
    return new aiContext();
}

void aiContext::destroy(aiContext* ctx)
{
    delete ctx;
}

aiContext::aiContext()
{
#ifdef aiDebug
    m_magic = aiMagicCtx;
#endif // aiDebug
}

aiContext::~aiContext()
{
    waitTasks();
    for (auto n : m_nodes) { delete n; }
    m_nodes.clear();
}

void aiContext::gatherNodesRecursive(aiObject *n)
{
    m_nodes.push_back(n);
    abcObject &abc = n->getAbcObject();
    int num_children = abc.getNumChildren();
    for (int i = 0; i < num_children; ++i) {
        aiObject *child = new aiObject(this, abc.getChild(i));
        n->addChild(child);
        gatherNodesRecursive(child);
    }
}

bool aiContext::load(const char *path)
{
    if (path == nullptr) return false;

    try {
        aiDebugLog("trying to open AbcCoreHDF5::ReadArchive...\n");
        m_archive = abcArchivePtr(new Abc::IArchive(AbcCoreHDF5::ReadArchive(), path));
    }
    catch (Alembic::Util::Exception e)
    {
        aiDebugLog("exception: %s\n", e.what());

        try {
            aiDebugLog("trying to open AbcCoreOgawa::ReadArchive...\n");
            m_archive = abcArchivePtr(new Abc::IArchive(AbcCoreOgawa::ReadArchive(), path));
        }
        catch (Alembic::Util::Exception e)
        {
            aiDebugLog("exception: %s\n", e.what());
        }
    }

    if (m_archive && m_archive->valid()) {
        aiObject *top = new aiObject(this, m_archive->getTop());
        gatherNodesRecursive(top);

        aiDebugLog("succeeded\n");
        return true;
    }
    else {
        m_archive.reset();
        return false;
    }
}

aiObject* aiContext::getTopObject()
{
    return m_nodes.empty() ? nullptr : m_nodes.front();
}

void aiContext::runTask(const std::function<void()> &task)
{
    m_tasks.run(task);
}

void aiContext::waitTasks()
{
    m_tasks.wait();
}
