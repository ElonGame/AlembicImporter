#include "pch.h"
#include "AlembicImporter.h"
#include "aiGeometry.h"
#include "aiContext.h"
#include "aiObject.h"

aiObject::aiObject(aiContext *ctx, abcObject &abc)
    : m_ctx(ctx)
    , m_abc(abc)
    , m_time(0.0f)
    , m_reverse_x(true)
    , m_triangulate(true)
    , m_reverse_index(false)
{
    if (m_abc.valid())
    {
        const auto& metadata = m_abc.getMetaData();
        m_has_xform = AbcGeom::IXformSchema::matches(metadata);
        m_has_polymesh = AbcGeom::IPolyMeshSchema::matches(metadata);
        m_has_curves = AbcGeom::ICurvesSchema::matches(metadata);
        m_has_points = AbcGeom::IPointsSchema::matches(metadata);
        m_has_camera = AbcGeom::ICameraSchema::matches(metadata);
        m_has_material = AbcMaterial::IMaterial::matches(metadata);

        if (m_has_xform)
        {
            m_xform = aiXForm(this);
            m_schemas.push_back(&m_xform);
        }
        if (m_has_polymesh)
        {
            m_polymesh = aiPolyMesh(this);
            m_schemas.push_back(&m_polymesh);
        }
        if (m_has_curves)
        {
            m_curves = aiCurves(this);
            m_schemas.push_back(&m_curves);
        }
        if (m_has_points)
        {
            m_points = aiPoints(this);
            m_schemas.push_back(&m_points);
        }
        if (m_has_camera)
        {
            m_camera = aiCamera(this);
            m_schemas.push_back(&m_camera);
        }
        if (m_has_material)
        {
            m_material = aiMaterial(this);
            m_schemas.push_back(&m_material);
        }

        //for (auto i : metadata) {
        //    aiDebugLogVerbose("%s: %s\n", i.first.c_str(), i.second.c_str());
        //}
    }
}

aiObject::~aiObject()
{
}

void aiObject::addChild(aiObject *c)
{
    m_children.push_back(c);
}

aiContext*  aiObject::getContext()          { return m_ctx; }
abcObject&  aiObject::getAbcObject()        { return m_abc; }
const char* aiObject::getName() const       { return m_abc.getName().c_str(); }
const char* aiObject::getFullName() const   { return m_abc.getFullName().c_str(); }
uint32_t    aiObject::getNumChildren() const{ return m_children.size(); }
aiObject*   aiObject::getChild(int i)       { return m_children[i]; }

void aiObject::setCurrentTime(float time)
{
    m_time = time;
    for (auto s : m_schemas) {
        s->updateSample();
    }
}
void aiObject::enableReverseX(bool v)       { m_reverse_x = v; }
void aiObject::enableTriangulate(bool v)    { m_triangulate = v; }
void aiObject::enableReverseIndex(bool v)   { m_reverse_index = v; }

float aiObject::getCurrentTime() const      { return m_time; }
bool aiObject::getReverseX() const          { return m_reverse_x; }
bool aiObject::getReverseIndex() const      { return m_reverse_index; }
bool aiObject::getTriangulate() const       { return m_triangulate; }


bool aiObject::hasXForm() const    { return m_has_xform; }
bool aiObject::hasPolyMesh() const { return m_has_polymesh; }
bool aiObject::hasCurves() const   { return m_has_curves; }
bool aiObject::hasPoints() const   { return m_has_points; }
bool aiObject::hasCamera() const   { return m_has_camera; }
bool aiObject::hasMaterial() const { return m_has_material; }

aiXForm&    aiObject::getXForm()      { return m_xform; }
aiPolyMesh& aiObject::getPolyMesh()   { return m_polymesh; }
aiCurves&   aiObject::getCurves()     { return m_curves; }
aiPoints&   aiObject::getPoints()     { return m_points; }
aiCamera&   aiObject::getCamera()     { return m_camera; }
aiMaterial& aiObject::getMaterial()   { return m_material; }


