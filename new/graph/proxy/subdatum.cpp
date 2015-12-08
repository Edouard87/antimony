#include <Python.h>

#include "graph/proxy/subdatum.h"
#include "graph/proxy/graph.h"
#include "graph/proxy/util.h"

#include "graph/datum.h"
#include "canvas/subdatum/subdatum_frame.h"
#include "canvas/connection/connection.h"
#include "canvas/subdatum/subdatum_row.h"
#include "canvas/scene.h"

SubdatumProxy::SubdatumProxy(Datum* d, GraphProxy* parent)
    : BaseDatumProxy(d, parent),
      frame(new SubdatumFrame(d, parent->canvasScene()))
{
    d->installWatcher(this);
    NULL_ON_DESTROYED(frame);
}

SubdatumProxy::~SubdatumProxy()
{
    if (frame)
        delete frame;
    for (auto c : connections)
        delete c;
}

void SubdatumProxy::trigger(const DatumState& state)
{
    frame->update(state);

    if (state.sigil == Datum::SIGIL_SUBGRAPH_OUTPUT ||
        state.sigil == Datum::SIGIL_SUBGRAPH_CONNECTION)
    {
        updateHash(state.links, &connections, this);
    }
}

CanvasInfo SubdatumProxy::canvasInfo() const
{
    CanvasInfo out;
    out.subdatum[datum] = frame->pos();
    return out;
}

void SubdatumProxy::setPositions(const CanvasInfo& info)
{
    if (info.subdatum.contains(datum))
        frame->setPos(info.subdatum[datum]);
}

////////////////////////////////////////////////////////////////////////////////

InputPort* SubdatumProxy::inputPort() const
{
    return frame->getRow()->inputPort();
}

OutputPort* SubdatumProxy::outputPort() const
{
    return frame->getRow()->outputPort();
}

GraphProxy* SubdatumProxy::graphProxy() const
{
    return static_cast<GraphProxy*>(parent());
}
